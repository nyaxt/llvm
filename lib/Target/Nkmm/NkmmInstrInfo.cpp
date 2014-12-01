//===-- NkmmInstrInfo.cpp - Nkmm Instruction Information ------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the Nkmm implementation of the TargetInstrInfo class.
//
//===----------------------------------------------------------------------===//

#include "NkmmInstrInfo.h"
#include "NkmmTargetMachine.h"
#include "NkmmMachineFunction.h"
#include "MCTargetDesc/NkmmBaseInfo.h"
#include "MCTargetDesc/NkmmMCTargetDesc.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/ADT/STLExtras.h"

#define GET_INSTRINFO_CTOR_DTOR
#include "NkmmGenInstrInfo.inc"

using namespace llvm;

NkmmInstrInfo::NkmmInstrInfo() : RI(*this) {}

NkmmInstrInfo::~NkmmInstrInfo() {}

const NkmmRegisterInfo &NkmmInstrInfo::getRegisterInfo() const { return RI; }

bool NkmmInstrInfo::expandPostRAPseudo(MachineBasicBlock::iterator MI) const {
  MachineBasicBlock &MBB = *MI->getParent();
  switch (MI->getDesc().getOpcode()) {
  default:
    return false;
  case Nkmm::RetPSP:
    BuildMI(MBB, MI, MI->getDebugLoc(), get(Nkmm::PseudoRET)).addReg(Nkmm::SP);
    break;
  }

  MBB.erase(MI);
  return true;
}

bool NkmmInstrInfo::AnalyzeBranch(MachineBasicBlock &MBB,
                                  MachineBasicBlock *&TBB,
                                  MachineBasicBlock *&FBB,
                                  SmallVectorImpl<MachineOperand> &Cond,
                                  bool AllowModify) const {
  TBB = nullptr;
  FBB = nullptr;

  MachineBasicBlock::iterator I = MBB.end();
  if (I == MBB.begin())
    return false; // Empty blocks are easy.
  --I;

  // Walk backwards from the end of the basic block until the branch is
  // analyzed or we give up.
  while (isPredicated(I) || I->isTerminator() || I->isDebugValue()) {

    // Flag to be raised on unanalyzeable instructions. This is useful in cases
    // where we want to clean up on the end of the basic block before we bail
    // out.
    bool CantAnalyze = false;

    // Skip over DEBUG values and predicated nonterminators.
    while (I->isDebugValue() || !I->isTerminator()) {
      if (I == MBB.begin())
        return false;
      --I;
    }

    if (isIndirectBranchOpcode(I->getOpcode()) ||
        isJumpTableBranchOpcode(I->getOpcode())) {
      // Indirect branches and jump tables can't be analyzed, but we still want
      // to clean up any instructions at the tail of the basic block.
      CantAnalyze = true;
    } else if (isUncondBranchOpcode(I->getOpcode())) {
      TBB = I->getOperand(0).getMBB();
    } else if (isCondBranchOpcode(I->getOpcode())) {
      // Bail out if we encounter multiple conditional branches.
      if (!Cond.empty())
        return true;

      assert(!FBB && "FBB should have been null.");
      FBB = TBB;
      TBB = I->getOperand(0).getMBB();
      Cond.push_back(I->getOperand(1)); // CondCode
      Cond.push_back(I->getOperand(2)); // SP
    } else if (I->isReturn()) {
      // Returns can't be analyzed, but we should run cleanup.
      CantAnalyze = !isPredicated(I);
    } else {
      // We encountered other unrecognized terminator. Bail out immediately.
      return true;
    }

    // Cleanup code - to be run for unpredicated unconditional branches and
    //                returns.
    if (!isPredicated(I) &&
        (isUncondBranchOpcode(I->getOpcode()) ||
         isIndirectBranchOpcode(I->getOpcode()) ||
         isJumpTableBranchOpcode(I->getOpcode()) || I->isReturn())) {
      // Forget any previous condition branch information - it no longer
      // applies.
      Cond.clear();
      FBB = nullptr;

      // If we can modify the function, delete everything below this
      // unconditional branch.
      if (AllowModify) {
        MachineBasicBlock::iterator DI = std::next(I);
        while (DI != MBB.end()) {
          MachineInstr *InstToDelete = DI;
          ++DI;
          InstToDelete->eraseFromParent();
        }
      }
    }

    if (CantAnalyze)
      return true;

    if (I == MBB.begin())
      return false;

    --I;
  }

  // We made it past the terminators without bailing out - we must have
  // analyzed this branch successfully.
  return false;
}

unsigned NkmmInstrInfo::RemoveBranch(MachineBasicBlock &MBB) const {
  MachineBasicBlock::iterator I = MBB.end();
  if (I == MBB.begin())
    return 0;
  --I;
  while (I->isDebugValue()) {
    if (I == MBB.begin())
      return 0;
    --I;
  }
  if (!isUncondBranchOpcode(I->getOpcode()) &&
      !isCondBranchOpcode(I->getOpcode()))
    return 0;

  // Remove the branch.
  I->eraseFromParent();

  I = MBB.end();

  if (I == MBB.begin())
    return 1;
  --I;
  if (!isCondBranchOpcode(I->getOpcode()))
    return 1;

  // Remove the branch.
  I->eraseFromParent();
  return 2;
}

unsigned NkmmInstrInfo::InsertBranch(
    MachineBasicBlock &MBB, MachineBasicBlock *TBB, MachineBasicBlock *FBB,
    const SmallVectorImpl<MachineOperand> &Cond, DebugLoc DL) const {
  // Shouldn't be a fall through.
  assert(TBB && "InsertBranch must not be told to insert a fallthrough");
  assert((Cond.size() == 2 || Cond.size() == 0) &&
         "Nkmm::Jccri use 2nd/3rd operands (cc, ST) for condition!");

  if (!FBB) {
    if (Cond.empty()) { // Unconditional branch?
      BuildMI(&MBB, DL, get(Nkmm::JMPi)).addMBB(TBB);
    } else
      BuildMI(&MBB, DL, get(Nkmm::Jccri))
          .addMBB(TBB)
          .addImm(Cond[0].getImm())
          .addReg(Cond[1].getReg());
    return 1;
  }

  // Two-way conditional branch.
  BuildMI(&MBB, DL, get(Nkmm::Jccri))
      .addMBB(TBB)
      .addImm(Cond[0].getImm())
      .addReg(Cond[1].getReg());
  BuildMI(&MBB, DL, get(Nkmm::JMPi)).addMBB(FBB);
  return 2;
}

bool NkmmInstrInfo::isPredicated(const MachineInstr *MI) const { return false; }

bool NkmmInstrInfo::ReverseBranchCondition(
    SmallVectorImpl<MachineOperand> &Cond) const {
  NkmmCC::CondCodes CC = (NkmmCC::CondCodes)(int)Cond[0].getImm();
  Cond[0].setImm(NkmmCC::getOppositeCondition(CC));
  return false;
}
