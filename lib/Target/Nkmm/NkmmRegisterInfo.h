//===-- NkmmRegisterInfo.h - Nkmm Register Information Impl -----*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the Nkmm implementation of the TargetRegisterInfo class.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_NKMM_NKMMREGISTERINFO_H
#define LLVM_LIB_TARGET_NKMM_NKMMREGISTERINFO_H

#include "Nkmm.h"
#include "llvm/Target/TargetRegisterInfo.h"

#define GET_REGINFO_HEADER
#include "NkmmGenRegisterInfo.inc"

namespace llvm {
class NkmmSubtarget;
class TargetInstrInfo;
class Type;

class NkmmRegisterInfo : public NkmmGenRegisterInfo {
  const TargetInstrInfo& TII;

public:
  NkmmRegisterInfo(const TargetInstrInfo &tii);

  /// Code Generation virtual methods...
  const MCPhysReg *
  getCalleeSavedRegs(const MachineFunction *MF = nullptr) const override;
  const uint32_t *getCallPreservedMask(CallingConv::ID) const override;

  BitVector getReservedRegs(const MachineFunction &MF) const override;

  /// Stack Frame Processing Methods
  void eliminateFrameIndex(MachineBasicBlock::iterator II,
                           int SPAdj, unsigned FIOperandNum,
                           RegScavenger *RS = nullptr) const override;

  /// Debug information queries.
  unsigned getFrameRegister(const MachineFunction &MF) const override;
};

} // end namespace llvm

#endif
