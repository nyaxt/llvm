//===-- ValueEnumerator.cpp - Number values and types for bitcode writer --===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the ValueEnumerator class.
//
//===----------------------------------------------------------------------===//

#include "ValueEnumerator.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/SmallPtrSet.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/UseListOrder.h"
#include "llvm/IR/ValueSymbolTable.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/raw_ostream.h"
#include <algorithm>
using namespace llvm;

namespace {
struct OrderMap {
  DenseMap<const Value *, std::pair<unsigned, bool>> IDs;
  unsigned LastGlobalConstantID;
  unsigned LastGlobalValueID;

  OrderMap() : LastGlobalConstantID(0), LastGlobalValueID(0) {}

  bool isGlobalConstant(unsigned ID) const {
    return ID <= LastGlobalConstantID;
  }
  bool isGlobalValue(unsigned ID) const {
    return ID <= LastGlobalValueID && !isGlobalConstant(ID);
  }

  unsigned size() const { return IDs.size(); }
  std::pair<unsigned, bool> &operator[](const Value *V) { return IDs[V]; }
  std::pair<unsigned, bool> lookup(const Value *V) const {
    return IDs.lookup(V);
  }
  void index(const Value *V) {
    // Explicitly sequence get-size and insert-value operations to avoid UB.
    unsigned ID = IDs.size() + 1;
    IDs[V].first = ID;
  }
};
}

static void orderValue(const Value *V, OrderMap &OM) {
  if (OM.lookup(V).first)
    return;

  if (const Constant *C = dyn_cast<Constant>(V))
    if (C->getNumOperands() && !isa<GlobalValue>(C))
      for (const Value *Op : C->operands())
        if (!isa<BasicBlock>(Op) && !isa<GlobalValue>(Op))
          orderValue(Op, OM);

  // Note: we cannot cache this lookup above, since inserting into the map
  // changes the map's size, and thus affects the other IDs.
  OM.index(V);
}

static OrderMap orderModule(const Module &M) {
  // This needs to match the order used by ValueEnumerator::ValueEnumerator()
  // and ValueEnumerator::incorporateFunction().
  OrderMap OM;

  // In the reader, initializers of GlobalValues are set *after* all the
  // globals have been read.  Rather than awkwardly modeling this behaviour
  // directly in predictValueUseListOrderImpl(), just assign IDs to
  // initializers of GlobalValues before GlobalValues themselves to model this
  // implicitly.
  for (const GlobalVariable &G : M.globals())
    if (G.hasInitializer())
      if (!isa<GlobalValue>(G.getInitializer()))
        orderValue(G.getInitializer(), OM);
  for (const GlobalAlias &A : M.aliases())
    if (!isa<GlobalValue>(A.getAliasee()))
      orderValue(A.getAliasee(), OM);
  for (const Function &F : M)
    if (F.hasPrefixData())
      if (!isa<GlobalValue>(F.getPrefixData()))
        orderValue(F.getPrefixData(), OM);
  OM.LastGlobalConstantID = OM.size();

  // Initializers of GlobalValues are processed in
  // BitcodeReader::ResolveGlobalAndAliasInits().  Match the order there rather
  // than ValueEnumerator, and match the code in predictValueUseListOrderImpl()
  // by giving IDs in reverse order.
  //
  // Since GlobalValues never reference each other directly (just through
  // initializers), their relative IDs only matter for determining order of
  // uses in their initializers.
  for (const Function &F : M)
    orderValue(&F, OM);
  for (const GlobalAlias &A : M.aliases())
    orderValue(&A, OM);
  for (const GlobalVariable &G : M.globals())
    orderValue(&G, OM);
  OM.LastGlobalValueID = OM.size();

  for (const Function &F : M) {
    if (F.isDeclaration())
      continue;
    // Here we need to match the union of ValueEnumerator::incorporateFunction()
    // and WriteFunction().  Basic blocks are implicitly declared before
    // anything else (by declaring their size).
    for (const BasicBlock &BB : F)
      orderValue(&BB, OM);
    for (const Argument &A : F.args())
      orderValue(&A, OM);
    for (const BasicBlock &BB : F)
      for (const Instruction &I : BB)
        for (const Value *Op : I.operands())
          if ((isa<Constant>(*Op) && !isa<GlobalValue>(*Op)) ||
              isa<InlineAsm>(*Op))
            orderValue(Op, OM);
    for (const BasicBlock &BB : F)
      for (const Instruction &I : BB)
        orderValue(&I, OM);
  }
  return OM;
}

static void predictValueUseListOrderImpl(const Value *V, const Function *F,
                                         unsigned ID, const OrderMap &OM,
                                         UseListOrderStack &Stack) {
  // Predict use-list order for this one.
  typedef std::pair<const Use *, unsigned> Entry;
  SmallVector<Entry, 64> List;
  for (const Use &U : V->uses())
    // Check if this user will be serialized.
    if (OM.lookup(U.getUser()).first)
      List.push_back(std::make_pair(&U, List.size()));

  if (List.size() < 2)
    // We may have lost some users.
    return;

  bool IsGlobalValue = OM.isGlobalValue(ID);
  std::sort(List.begin(), List.end(), [&](const Entry &L, const Entry &R) {
    const Use *LU = L.first;
    const Use *RU = R.first;
    if (LU == RU)
      return false;

    auto LID = OM.lookup(LU->getUser()).first;
    auto RID = OM.lookup(RU->getUser()).first;

    // Global values are processed in reverse order.
    //
    // Moreover, initializers of GlobalValues are set *after* all the globals
    // have been read (despite having earlier IDs).  Rather than awkwardly
    // modeling this behaviour here, orderModule() has assigned IDs to
    // initializers of GlobalValues before GlobalValues themselves.
    if (OM.isGlobalValue(LID) && OM.isGlobalValue(RID))
      return LID < RID;

    // If ID is 4, then expect: 7 6 5 1 2 3.
    if (LID < RID) {
      if (RID <= ID)
        if (!IsGlobalValue) // GlobalValue uses don't get reversed.
          return true;
      return false;
    }
    if (RID < LID) {
      if (LID <= ID)
        if (!IsGlobalValue) // GlobalValue uses don't get reversed.
          return false;
      return true;
    }

    // LID and RID are equal, so we have different operands of the same user.
    // Assume operands are added in order for all instructions.
    if (LID <= ID)
      if (!IsGlobalValue) // GlobalValue uses don't get reversed.
        return LU->getOperandNo() < RU->getOperandNo();
    return LU->getOperandNo() > RU->getOperandNo();
  });

  if (std::is_sorted(
          List.begin(), List.end(),
          [](const Entry &L, const Entry &R) { return L.second < R.second; }))
    // Order is already correct.
    return;

  // Store the shuffle.
  Stack.emplace_back(V, F, List.size());
  assert(List.size() == Stack.back().Shuffle.size() && "Wrong size");
  for (size_t I = 0, E = List.size(); I != E; ++I)
    Stack.back().Shuffle[I] = List[I].second;
}

static void predictValueUseListOrder(const Value *V, const Function *F,
                                     OrderMap &OM, UseListOrderStack &Stack) {
  auto &IDPair = OM[V];
  assert(IDPair.first && "Unmapped value");
  if (IDPair.second)
    // Already predicted.
    return;

  // Do the actual prediction.
  IDPair.second = true;
  if (!V->use_empty() && std::next(V->use_begin()) != V->use_end())
    predictValueUseListOrderImpl(V, F, IDPair.first, OM, Stack);

  // Recursive descent into constants.
  if (const Constant *C = dyn_cast<Constant>(V))
    if (C->getNumOperands()) // Visit GlobalValues.
      for (const Value *Op : C->operands())
        if (isa<Constant>(Op)) // Visit GlobalValues.
          predictValueUseListOrder(Op, F, OM, Stack);
}

static UseListOrderStack predictUseListOrder(const Module &M) {
  OrderMap OM = orderModule(M);

  // Use-list orders need to be serialized after all the users have been added
  // to a value, or else the shuffles will be incomplete.  Store them per
  // function in a stack.
  //
  // Aside from function order, the order of values doesn't matter much here.
  UseListOrderStack Stack;

  // We want to visit the functions backward now so we can list function-local
  // constants in the last Function they're used in.  Module-level constants
  // have already been visited above.
  for (auto I = M.rbegin(), E = M.rend(); I != E; ++I) {
    const Function &F = *I;
    if (F.isDeclaration())
      continue;
    for (const BasicBlock &BB : F)
      predictValueUseListOrder(&BB, &F, OM, Stack);
    for (const Argument &A : F.args())
      predictValueUseListOrder(&A, &F, OM, Stack);
    for (const BasicBlock &BB : F)
      for (const Instruction &I : BB)
        for (const Value *Op : I.operands())
          if (isa<Constant>(*Op) || isa<InlineAsm>(*Op)) // Visit GlobalValues.
            predictValueUseListOrder(Op, &F, OM, Stack);
    for (const BasicBlock &BB : F)
      for (const Instruction &I : BB)
        predictValueUseListOrder(&I, &F, OM, Stack);
  }

  // Visit globals last, since the module-level use-list block will be seen
  // before the function bodies are processed.
  for (const GlobalVariable &G : M.globals())
    predictValueUseListOrder(&G, nullptr, OM, Stack);
  for (const Function &F : M)
    predictValueUseListOrder(&F, nullptr, OM, Stack);
  for (const GlobalAlias &A : M.aliases())
    predictValueUseListOrder(&A, nullptr, OM, Stack);
  for (const GlobalVariable &G : M.globals())
    if (G.hasInitializer())
      predictValueUseListOrder(G.getInitializer(), nullptr, OM, Stack);
  for (const GlobalAlias &A : M.aliases())
    predictValueUseListOrder(A.getAliasee(), nullptr, OM, Stack);
  for (const Function &F : M)
    if (F.hasPrefixData())
      predictValueUseListOrder(F.getPrefixData(), nullptr, OM, Stack);

  return Stack;
}

static bool isIntOrIntVectorValue(const std::pair<const Value*, unsigned> &V) {
  return V.first->getType()->isIntOrIntVectorTy();
}

ValueEnumerator::ValueEnumerator(const Module &M) {
  if (shouldPreserveBitcodeUseListOrder())
    UseListOrders = predictUseListOrder(M);

  // Enumerate the global variables.
  for (Module::const_global_iterator I = M.global_begin(), E = M.global_end();
       I != E; ++I)
    EnumerateValue(I);

  // Enumerate the functions.
  for (Module::const_iterator I = M.begin(), E = M.end(); I != E; ++I) {
    EnumerateValue(I);
    EnumerateAttributes(cast<Function>(I)->getAttributes());
  }

  // Enumerate the aliases.
  for (Module::const_alias_iterator I = M.alias_begin(), E = M.alias_end();
       I != E; ++I)
    EnumerateValue(I);

  // Remember what is the cutoff between globalvalue's and other constants.
  unsigned FirstConstant = Values.size();

  // Enumerate the global variable initializers.
  for (Module::const_global_iterator I = M.global_begin(), E = M.global_end();
       I != E; ++I)
    if (I->hasInitializer())
      EnumerateValue(I->getInitializer());

  // Enumerate the aliasees.
  for (Module::const_alias_iterator I = M.alias_begin(), E = M.alias_end();
       I != E; ++I)
    EnumerateValue(I->getAliasee());

  // Enumerate the prefix data constants.
  for (Module::const_iterator I = M.begin(), E = M.end(); I != E; ++I)
    if (I->hasPrefixData())
      EnumerateValue(I->getPrefixData());

  // Insert constants and metadata that are named at module level into the slot
  // pool so that the module symbol table can refer to them...
  EnumerateValueSymbolTable(M.getValueSymbolTable());
  EnumerateNamedMetadata(M);

  SmallVector<std::pair<unsigned, MDNode *>, 8> MDs;

  // Enumerate types used by function bodies and argument lists.
  for (const Function &F : M) {
    for (const Argument &A : F.args())
      EnumerateType(A.getType());

    for (const BasicBlock &BB : F)
      for (const Instruction &I : BB) {
        for (const Use &Op : I.operands()) {
          if (MDNode *MD = dyn_cast<MDNode>(&Op))
            if (MD->isFunctionLocal() && MD->getFunction())
              // These will get enumerated during function-incorporation.
              continue;
          EnumerateOperandType(Op);
        }
        EnumerateType(I.getType());
        if (const CallInst *CI = dyn_cast<CallInst>(&I))
          EnumerateAttributes(CI->getAttributes());
        else if (const InvokeInst *II = dyn_cast<InvokeInst>(&I))
          EnumerateAttributes(II->getAttributes());

        // Enumerate metadata attached with this instruction.
        MDs.clear();
        I.getAllMetadataOtherThanDebugLoc(MDs);
        for (unsigned i = 0, e = MDs.size(); i != e; ++i)
          EnumerateMetadata(MDs[i].second);

        if (!I.getDebugLoc().isUnknown()) {
          MDNode *Scope, *IA;
          I.getDebugLoc().getScopeAndInlinedAt(Scope, IA, I.getContext());
          if (Scope) EnumerateMetadata(Scope);
          if (IA) EnumerateMetadata(IA);
        }
      }
  }

  // Optimize constant ordering.
  OptimizeConstants(FirstConstant, Values.size());
}

unsigned ValueEnumerator::getInstructionID(const Instruction *Inst) const {
  InstructionMapType::const_iterator I = InstructionMap.find(Inst);
  assert(I != InstructionMap.end() && "Instruction is not mapped!");
  return I->second;
}

unsigned ValueEnumerator::getComdatID(const Comdat *C) const {
  unsigned ComdatID = Comdats.idFor(C);
  assert(ComdatID && "Comdat not found!");
  return ComdatID;
}

void ValueEnumerator::setInstructionID(const Instruction *I) {
  InstructionMap[I] = InstructionCount++;
}

unsigned ValueEnumerator::getValueID(const Value *V) const {
  if (isa<MDNode>(V) || isa<MDString>(V)) {
    ValueMapType::const_iterator I = MDValueMap.find(V);
    assert(I != MDValueMap.end() && "Value not in slotcalculator!");
    return I->second-1;
  }

  ValueMapType::const_iterator I = ValueMap.find(V);
  assert(I != ValueMap.end() && "Value not in slotcalculator!");
  return I->second-1;
}

void ValueEnumerator::dump() const {
  print(dbgs(), ValueMap, "Default");
  dbgs() << '\n';
  print(dbgs(), MDValueMap, "MetaData");
  dbgs() << '\n';
}

void ValueEnumerator::print(raw_ostream &OS, const ValueMapType &Map,
                            const char *Name) const {

  OS << "Map Name: " << Name << "\n";
  OS << "Size: " << Map.size() << "\n";
  for (ValueMapType::const_iterator I = Map.begin(),
         E = Map.end(); I != E; ++I) {

    const Value *V = I->first;
    if (V->hasName())
      OS << "Value: " << V->getName();
    else
      OS << "Value: [null]\n";
    V->dump();

    OS << " Uses(" << std::distance(V->use_begin(),V->use_end()) << "):";
    for (const Use &U : V->uses()) {
      if (&U != &*V->use_begin())
        OS << ",";
      if(U->hasName())
        OS << " " << U->getName();
      else
        OS << " [null]";

    }
    OS <<  "\n\n";
  }
}

/// OptimizeConstants - Reorder constant pool for denser encoding.
void ValueEnumerator::OptimizeConstants(unsigned CstStart, unsigned CstEnd) {
  if (CstStart == CstEnd || CstStart+1 == CstEnd) return;

  if (shouldPreserveBitcodeUseListOrder())
    // Optimizing constants makes the use-list order difficult to predict.
    // Disable it for now when trying to preserve the order.
    return;

  std::stable_sort(Values.begin() + CstStart, Values.begin() + CstEnd,
                   [this](const std::pair<const Value *, unsigned> &LHS,
                          const std::pair<const Value *, unsigned> &RHS) {
    // Sort by plane.
    if (LHS.first->getType() != RHS.first->getType())
      return getTypeID(LHS.first->getType()) < getTypeID(RHS.first->getType());
    // Then by frequency.
    return LHS.second > RHS.second;
  });

  // Ensure that integer and vector of integer constants are at the start of the
  // constant pool.  This is important so that GEP structure indices come before
  // gep constant exprs.
  std::partition(Values.begin()+CstStart, Values.begin()+CstEnd,
                 isIntOrIntVectorValue);

  // Rebuild the modified portion of ValueMap.
  for (; CstStart != CstEnd; ++CstStart)
    ValueMap[Values[CstStart].first] = CstStart+1;
}


/// EnumerateValueSymbolTable - Insert all of the values in the specified symbol
/// table into the values table.
void ValueEnumerator::EnumerateValueSymbolTable(const ValueSymbolTable &VST) {
  for (ValueSymbolTable::const_iterator VI = VST.begin(), VE = VST.end();
       VI != VE; ++VI)
    EnumerateValue(VI->getValue());
}

/// Insert all of the values referenced by named metadata in the specified
/// module.
void ValueEnumerator::EnumerateNamedMetadata(const Module &M) {
  for (Module::const_named_metadata_iterator I = M.named_metadata_begin(),
                                             E = M.named_metadata_end();
       I != E; ++I)
    EnumerateNamedMDNode(I);
}

void ValueEnumerator::EnumerateNamedMDNode(const NamedMDNode *MD) {
  for (unsigned i = 0, e = MD->getNumOperands(); i != e; ++i)
    EnumerateMetadata(MD->getOperand(i));
}

/// EnumerateMDNodeOperands - Enumerate all non-function-local values
/// and types referenced by the given MDNode.
void ValueEnumerator::EnumerateMDNodeOperands(const MDNode *N) {
  for (unsigned i = 0, e = N->getNumOperands(); i != e; ++i) {
    if (Value *V = N->getOperand(i)) {
      if (isa<MDNode>(V) || isa<MDString>(V))
        EnumerateMetadata(V);
      else if (!isa<Instruction>(V) && !isa<Argument>(V))
        EnumerateValue(V);
    } else
      EnumerateType(Type::getVoidTy(N->getContext()));
  }
}

void ValueEnumerator::EnumerateMetadata(const Value *MD) {
  assert((isa<MDNode>(MD) || isa<MDString>(MD)) && "Invalid metadata kind");

  // Skip function-local nodes themselves, but walk their operands.
  const MDNode *N = dyn_cast<MDNode>(MD);
  if (N && N->isFunctionLocal() && N->getFunction()) {
    EnumerateMDNodeOperands(N);
    return;
  }

  // Insert a dummy ID to block the co-recursive call to
  // EnumerateMDNodeOperands() from re-visiting MD in a cyclic graph.
  //
  // Return early if there's already an ID.
  if (!MDValueMap.insert(std::make_pair(MD, 0)).second)
    return;

  // Enumerate the type of this value.
  EnumerateType(MD->getType());

  // Visit operands first to minimize RAUW.
  if (N)
    EnumerateMDNodeOperands(N);

  // Replace the dummy ID inserted above with the correct one.  MDValueMap may
  // have changed by inserting operands, so we need a fresh lookup here.
  MDValues.push_back(MD);
  MDValueMap[MD] = MDValues.size();
}

/// EnumerateFunctionLocalMetadataa - Incorporate function-local metadata
/// information reachable from the given MDNode.
void ValueEnumerator::EnumerateFunctionLocalMetadata(const MDNode *N) {
  assert(N->isFunctionLocal() && N->getFunction() &&
         "EnumerateFunctionLocalMetadata called on non-function-local mdnode!");

  // Enumerate the type of this value.
  EnumerateType(N->getType());

  // Check to see if it's already in!
  unsigned &MDValueID = MDValueMap[N];
  if (MDValueID)
    return;

  MDValues.push_back(N);
  MDValueID = MDValues.size();

  // To incoroporate function-local information visit all function-local
  // MDNodes and all function-local values they reference.
  for (unsigned i = 0, e = N->getNumOperands(); i != e; ++i)
    if (Value *V = N->getOperand(i)) {
      if (MDNode *O = dyn_cast<MDNode>(V)) {
        if (O->isFunctionLocal() && O->getFunction())
          EnumerateFunctionLocalMetadata(O);
      } else if (isa<Instruction>(V) || isa<Argument>(V))
        EnumerateValue(V);
    }

  // Also, collect all function-local MDNodes for easy access.
  FunctionLocalMDs.push_back(N);
}

void ValueEnumerator::EnumerateValue(const Value *V) {
  assert(!V->getType()->isVoidTy() && "Can't insert void values!");
  assert(!isa<MDNode>(V) && !isa<MDString>(V) &&
         "EnumerateValue doesn't handle Metadata!");

  // Check to see if it's already in!
  unsigned &ValueID = ValueMap[V];
  if (ValueID) {
    // Increment use count.
    Values[ValueID-1].second++;
    return;
  }

  if (auto *GO = dyn_cast<GlobalObject>(V))
    if (const Comdat *C = GO->getComdat())
      Comdats.insert(C);

  // Enumerate the type of this value.
  EnumerateType(V->getType());

  if (const Constant *C = dyn_cast<Constant>(V)) {
    if (isa<GlobalValue>(C)) {
      // Initializers for globals are handled explicitly elsewhere.
    } else if (C->getNumOperands()) {
      // If a constant has operands, enumerate them.  This makes sure that if a
      // constant has uses (for example an array of const ints), that they are
      // inserted also.

      // We prefer to enumerate them with values before we enumerate the user
      // itself.  This makes it more likely that we can avoid forward references
      // in the reader.  We know that there can be no cycles in the constants
      // graph that don't go through a global variable.
      for (User::const_op_iterator I = C->op_begin(), E = C->op_end();
           I != E; ++I)
        if (!isa<BasicBlock>(*I)) // Don't enumerate BB operand to BlockAddress.
          EnumerateValue(*I);

      // Finally, add the value.  Doing this could make the ValueID reference be
      // dangling, don't reuse it.
      Values.push_back(std::make_pair(V, 1U));
      ValueMap[V] = Values.size();
      return;
    }
  }

  // Add the value.
  Values.push_back(std::make_pair(V, 1U));
  ValueID = Values.size();
}


void ValueEnumerator::EnumerateType(Type *Ty) {
  unsigned *TypeID = &TypeMap[Ty];

  // We've already seen this type.
  if (*TypeID)
    return;

  // If it is a non-anonymous struct, mark the type as being visited so that we
  // don't recursively visit it.  This is safe because we allow forward
  // references of these in the bitcode reader.
  if (StructType *STy = dyn_cast<StructType>(Ty))
    if (!STy->isLiteral())
      *TypeID = ~0U;

  // Enumerate all of the subtypes before we enumerate this type.  This ensures
  // that the type will be enumerated in an order that can be directly built.
  for (Type *SubTy : Ty->subtypes())
    EnumerateType(SubTy);

  // Refresh the TypeID pointer in case the table rehashed.
  TypeID = &TypeMap[Ty];

  // Check to see if we got the pointer another way.  This can happen when
  // enumerating recursive types that hit the base case deeper than they start.
  //
  // If this is actually a struct that we are treating as forward ref'able,
  // then emit the definition now that all of its contents are available.
  if (*TypeID && *TypeID != ~0U)
    return;

  // Add this type now that its contents are all happily enumerated.
  Types.push_back(Ty);

  *TypeID = Types.size();
}

// Enumerate the types for the specified value.  If the value is a constant,
// walk through it, enumerating the types of the constant.
void ValueEnumerator::EnumerateOperandType(const Value *V) {
  EnumerateType(V->getType());

  if (const Constant *C = dyn_cast<Constant>(V)) {
    // If this constant is already enumerated, ignore it, we know its type must
    // be enumerated.
    if (ValueMap.count(V)) return;

    // This constant may have operands, make sure to enumerate the types in
    // them.
    for (unsigned i = 0, e = C->getNumOperands(); i != e; ++i) {
      const Value *Op = C->getOperand(i);

      // Don't enumerate basic blocks here, this happens as operands to
      // blockaddress.
      if (isa<BasicBlock>(Op)) continue;

      EnumerateOperandType(Op);
    }

    if (const MDNode *N = dyn_cast<MDNode>(V)) {
      for (unsigned i = 0, e = N->getNumOperands(); i != e; ++i)
        if (Value *Elem = N->getOperand(i))
          EnumerateOperandType(Elem);
    }
  } else if (isa<MDString>(V) || isa<MDNode>(V))
    EnumerateMetadata(V);
}

void ValueEnumerator::EnumerateAttributes(AttributeSet PAL) {
  if (PAL.isEmpty()) return;  // null is always 0.

  // Do a lookup.
  unsigned &Entry = AttributeMap[PAL];
  if (Entry == 0) {
    // Never saw this before, add it.
    Attribute.push_back(PAL);
    Entry = Attribute.size();
  }

  // Do lookups for all attribute groups.
  for (unsigned i = 0, e = PAL.getNumSlots(); i != e; ++i) {
    AttributeSet AS = PAL.getSlotAttributes(i);
    unsigned &Entry = AttributeGroupMap[AS];
    if (Entry == 0) {
      AttributeGroups.push_back(AS);
      Entry = AttributeGroups.size();
    }
  }
}

void ValueEnumerator::incorporateFunction(const Function &F) {
  InstructionCount = 0;
  NumModuleValues = Values.size();
  NumModuleMDValues = MDValues.size();

  // Adding function arguments to the value table.
  for (Function::const_arg_iterator I = F.arg_begin(), E = F.arg_end();
       I != E; ++I)
    EnumerateValue(I);

  FirstFuncConstantID = Values.size();

  // Add all function-level constants to the value table.
  for (Function::const_iterator BB = F.begin(), E = F.end(); BB != E; ++BB) {
    for (BasicBlock::const_iterator I = BB->begin(), E = BB->end(); I!=E; ++I)
      for (User::const_op_iterator OI = I->op_begin(), E = I->op_end();
           OI != E; ++OI) {
        if ((isa<Constant>(*OI) && !isa<GlobalValue>(*OI)) ||
            isa<InlineAsm>(*OI))
          EnumerateValue(*OI);
      }
    BasicBlocks.push_back(BB);
    ValueMap[BB] = BasicBlocks.size();
  }

  // Optimize the constant layout.
  OptimizeConstants(FirstFuncConstantID, Values.size());

  // Add the function's parameter attributes so they are available for use in
  // the function's instruction.
  EnumerateAttributes(F.getAttributes());

  FirstInstID = Values.size();

  SmallVector<MDNode *, 8> FnLocalMDVector;
  // Add all of the instructions.
  for (Function::const_iterator BB = F.begin(), E = F.end(); BB != E; ++BB) {
    for (BasicBlock::const_iterator I = BB->begin(), E = BB->end(); I!=E; ++I) {
      for (User::const_op_iterator OI = I->op_begin(), E = I->op_end();
           OI != E; ++OI) {
        if (MDNode *MD = dyn_cast<MDNode>(*OI))
          if (MD->isFunctionLocal() && MD->getFunction())
            // Enumerate metadata after the instructions they might refer to.
            FnLocalMDVector.push_back(MD);
      }

      SmallVector<std::pair<unsigned, MDNode *>, 8> MDs;
      I->getAllMetadataOtherThanDebugLoc(MDs);
      for (unsigned i = 0, e = MDs.size(); i != e; ++i) {
        MDNode *N = MDs[i].second;
        if (N->isFunctionLocal() && N->getFunction())
          FnLocalMDVector.push_back(N);
      }

      if (!I->getType()->isVoidTy())
        EnumerateValue(I);
    }
  }

  // Add all of the function-local metadata.
  for (unsigned i = 0, e = FnLocalMDVector.size(); i != e; ++i)
    EnumerateFunctionLocalMetadata(FnLocalMDVector[i]);
}

void ValueEnumerator::purgeFunction() {
  /// Remove purged values from the ValueMap.
  for (unsigned i = NumModuleValues, e = Values.size(); i != e; ++i)
    ValueMap.erase(Values[i].first);
  for (unsigned i = NumModuleMDValues, e = MDValues.size(); i != e; ++i)
    MDValueMap.erase(MDValues[i]);
  for (unsigned i = 0, e = BasicBlocks.size(); i != e; ++i)
    ValueMap.erase(BasicBlocks[i]);

  Values.resize(NumModuleValues);
  MDValues.resize(NumModuleMDValues);
  BasicBlocks.clear();
  FunctionLocalMDs.clear();
}

static void IncorporateFunctionInfoGlobalBBIDs(const Function *F,
                                 DenseMap<const BasicBlock*, unsigned> &IDMap) {
  unsigned Counter = 0;
  for (Function::const_iterator BB = F->begin(), E = F->end(); BB != E; ++BB)
    IDMap[BB] = ++Counter;
}

/// getGlobalBasicBlockID - This returns the function-specific ID for the
/// specified basic block.  This is relatively expensive information, so it
/// should only be used by rare constructs such as address-of-label.
unsigned ValueEnumerator::getGlobalBasicBlockID(const BasicBlock *BB) const {
  unsigned &Idx = GlobalBasicBlockIDs[BB];
  if (Idx != 0)
    return Idx-1;

  IncorporateFunctionInfoGlobalBBIDs(BB->getParent(), GlobalBasicBlockIDs);
  return getGlobalBasicBlockID(BB);
}

