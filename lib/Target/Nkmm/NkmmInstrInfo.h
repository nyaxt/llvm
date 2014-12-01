//===-- NkmmInstrInfo.h - Nkmm Instruction Information ----------*- C++ -*-===//
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
// FIXME: We need to override TargetInstrInfo::getInlineAsmLength method in
// order for NkmmLongBranch pass to work correctly when the code has inline
// assembly.  The returned value doesn't have to be the asm instruction's exact
// size in bytes; NkmmLongBranch only expects it to be the correct upper bound.
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_NKMM_NKMMINSTRINFO_H
#define LLVM_LIB_TARGET_NKMM_NKMMINSTRINFO_H

#include "Nkmm.h"
#include "NkmmRegisterInfo.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/Target/TargetInstrInfo.h"

#define GET_INSTRINFO_HEADER
#include "NkmmGenInstrInfo.inc"

namespace llvm {

class NkmmInstrInfo : public NkmmGenInstrInfo {
  NkmmRegisterInfo RI;

public:
  NkmmInstrInfo();
  ~NkmmInstrInfo();

  /// getRegisterInfo - TargetInstrInfo is a superset of MRegister info.  As
  /// such, whenever a client has an instance of instruction info, it should
  /// always be able to get register info as well (through this method).
  ///
  virtual const NkmmRegisterInfo &getRegisterInfo() const;

  bool expandPostRAPseudo(MachineBasicBlock::iterator MI) const override;

  // Branch analysis.
  bool AnalyzeBranch(MachineBasicBlock &MBB, MachineBasicBlock *&TBB,
                     MachineBasicBlock *&FBB,
                     SmallVectorImpl<MachineOperand> &Cond,
                     bool AllowModify = false) const override;
  unsigned RemoveBranch(MachineBasicBlock &MBB) const override;
  unsigned InsertBranch(MachineBasicBlock &MBB, MachineBasicBlock *TBB,
                        MachineBasicBlock *FBB,
                        const SmallVectorImpl<MachineOperand> &Cond,
                        DebugLoc DL) const override;

  bool isPredicated(const MachineInstr *MI) const override;
  bool
  ReverseBranchCondition(SmallVectorImpl<MachineOperand> &Cond) const override;
};

static inline bool isUncondBranchOpcode(int Opc) { return Opc == Nkmm::JMPi; }

static inline bool isCondBranchOpcode(int Opc) { return Opc == Nkmm::Jccri; }

static inline bool isJumpTableBranchOpcode(int Opc) {
  return false; // FIXME: we don't have one yet.
}

static inline bool isIndirectBranchOpcode(int Opc) {
  return Opc == Nkmm::JMPr || Opc == Nkmm::JMPm;
}
}

#endif
