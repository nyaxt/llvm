//===-- NkmmISelDAGToDAG.cpp - A Dag to Dag Inst Selector for Nkmm --------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines an instruction selector for the Nkmm target.
//
//===----------------------------------------------------------------------===//

#define DEBUG_TYPE "sample-isel"
#include "Nkmm.h"
#include "NkmmRegisterInfo.h"
#include "NkmmSubtarget.h"
#include "NkmmTargetMachine.h"
#include "MCTargetDesc/NkmmMCTargetDesc.h"
#include "llvm/CodeGen/MachineConstantPool.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/SelectionDAGNodes.h"
#include "llvm/IR/CFG.h"
#include "llvm/IR/GlobalValue.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Intrinsics.h"
#include "llvm/IR/Type.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Target/TargetMachine.h"

using namespace llvm;

//===----------------------------------------------------------------------===//
// Instruction Selector Implementation
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// NkmmDAGToDAGISel - Nkmm specific code to select Nkmm machine
// instructions for SelectionDAG operations.
//===----------------------------------------------------------------------===//
namespace {

class NkmmDAGToDAGISel : public SelectionDAGISel {

  /// TM - Keep a reference to NkmmTargetMachine.
  const NkmmTargetMachine &TM;

  /// Subtarget - Keep a pointer to the NkmmSubtarget around so that we can
  /// make the right decision when generating code for different targets.
  const NkmmSubtarget &Subtarget;

public:
  explicit NkmmDAGToDAGISel(NkmmTargetMachine &tm) :
  SelectionDAGISel(tm),
  TM(tm),
  Subtarget(tm.getSubtarget<NkmmSubtarget>()) {}

  // Pass Name
  virtual const char *getPassName() const {
    return "Nkmm DAG->DAG Pattern Instruction Selection";
  }

private:
  // Include the pieces autogenerated from the target description.
  #include "NkmmGenDAGISel.inc"

  /// getTargetMachine - Return a reference to the TargetMachine, casted
  /// to the target-specific type.
  const NkmmTargetMachine &getTargetMachine() {
    return static_cast<const NkmmTargetMachine &>(TM);
  }

  /// getInstrInfo - Return a reference to the TargetInstrInfo, casted
  /// to the target-specific type.
  const NkmmInstrInfo *getInstrInfo() {
    return getTargetMachine().getInstrInfo();
  }

  SDNode *Select(SDNode *N) /*override*/;

  // Complex Pattern.
  bool SelectAddr(SDValue N, SDValue &Base, SDValue &Offset);
};
}

/// ComplexPattern used on NkmmInstrInfo
/// Used on Nkmm Load/Store instructions
bool NkmmDAGToDAGISel::
SelectAddr(SDValue N, SDValue &Base, SDValue &Offset) {
  EVT ValTy = N.getValueType();

  if (FrameIndexSDNode *FIN = dyn_cast<FrameIndexSDNode>(N)) {
    Base   = CurDAG->getTargetFrameIndex(FIN->getIndex(), ValTy);
    Offset = CurDAG->getTargetConstant(0, ValTy);
    return true;
  }

  llvm_unreachable("Unknown pattern");
  return true;
}

/// Select instructions not customized! Used for
/// expanded, promoted and normal instructions
SDNode* NkmmDAGToDAGISel::
Select(SDNode *Node) {
  // Select the default instruction
  SDNode *ResNode = SelectCode(Node);

  DEBUG(errs() << "=> ");
  if (ResNode == NULL || ResNode == Node)
    DEBUG(Node->dumpr(CurDAG));
  else
    DEBUG(ResNode->dumpr(CurDAG));
  DEBUG(errs() << "\n");
  return ResNode;
}

/// createNkmmISelDag - This pass converts a legalized DAG into a
/// Nkmm-specific DAG, ready for instruction scheduling.
FunctionPass *llvm::createNkmmISelDag(NkmmTargetMachine &TM) {
  return new NkmmDAGToDAGISel(TM);
}
