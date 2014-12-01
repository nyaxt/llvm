//===-- NkmmFrameLowering.h - Define frame lowering for Nkmm ----*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_NKMM_NKMMFRAMELOWERING_H
#define LLVM_LIB_TARGET_NKMM_NKMMFRAMELOWERING_H

#include "Nkmm.h"
#include "llvm/Target/TargetFrameLowering.h"

namespace llvm {
class NkmmSubtarget;

class NkmmFrameLowering : public TargetFrameLowering {
protected:
  const NkmmSubtarget &STI;

public:
  NkmmFrameLowering(const NkmmSubtarget &sti, unsigned Alignment)
      : TargetFrameLowering(StackGrowsDown, Alignment, 0, Alignment), STI(sti) {
  }

  bool hasFP(const MachineFunction &MF) const override;

  void emitPrologue(MachineFunction &MF) const override;
  void emitEpilogue(MachineFunction &MF, MachineBasicBlock &MBB) const override;

protected:
  uint64_t estimateStackSize(const MachineFunction &MF) const;
};

} // End llvm namespace

#endif
