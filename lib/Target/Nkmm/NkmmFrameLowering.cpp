//===-- NkmmFrameLowering.cpp - Nkmm Frame Information --------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the Nkmm implementation of TargetFrameLowering class.
//
//===----------------------------------------------------------------------===//

#include "NkmmFrameLowering.h"
#include "NkmmInstrInfo.h"
#include "NkmmMachineFunction.h"
#include "NkmmTargetMachine.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineModuleInfo.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Debug.h"
#include "llvm/Target/TargetOptions.h"

#define DEBUG_TYPE "nkmm-framelowering"

using namespace llvm;

// hasFP - Return true if the specified function should have a dedicated frame
// pointer register.  This is true if the function has variable sized allocas or
// if frame pointer elimination is disabled.
bool NkmmFrameLowering::hasFP(const MachineFunction &MF) const {
  const MachineFrameInfo *MFI = MF.getFrameInfo();
  return MF.getTarget().Options.DisableFramePointerElim(MF) ||
      MFI->hasVarSizedObjects() || MFI->isFrameAddressTaken();
}

uint64_t NkmmFrameLowering::estimateStackSize(const MachineFunction &MF) const {
  const MachineFrameInfo *MFI = MF.getFrameInfo();
  const TargetRegisterInfo &TRI = *MF.getSubtarget().getRegisterInfo();

  int64_t Offset = 0;

  // Iterate over fixed sized objects.
  for (int I = MFI->getObjectIndexBegin(); I != 0; ++I)
    Offset = std::max(Offset, -MFI->getObjectOffset(I));

  // Conservatively assume all callee-saved registers will be saved.
  for (const MCPhysReg *R = TRI.getCalleeSavedRegs(&MF); *R; ++R) {
    unsigned Size = TRI.getMinimalPhysRegClass(*R)->getSize();
    Offset = RoundUpToAlignment(Offset + Size, Size);
  }

  unsigned MaxAlign = MFI->getMaxAlignment();

  // Check that MaxAlign is not zero if there is a stack object that is not a
  // callee-saved spill.
  assert(!MFI->getObjectIndexEnd() || MaxAlign);

  // Iterate over other objects.
  for (unsigned I = 0, E = MFI->getObjectIndexEnd(); I != E; ++I)
    Offset = RoundUpToAlignment(Offset + MFI->getObjectSize(I), MaxAlign);

  // Call frame.
  if (MFI->adjustsStack() && hasReservedCallFrame(MF))
    Offset = RoundUpToAlignment(Offset + MFI->getMaxCallFrameSize(),
                                std::max(MaxAlign, getStackAlignment()));

  return RoundUpToAlignment(Offset, getStackAlignment());
}

void NkmmFrameLowering::
emitPrologue(MachineFunction &MF) const {
  DEBUG(dbgs() << ">> NkmmFrameLowering::emitPrologue <<\n");

  MachineBasicBlock &MBB   = MF.front();
  MachineFrameInfo *MFI = MF.getFrameInfo();

  const NkmmInstrInfo &TII =
    *static_cast<const NkmmInstrInfo*>(STI.getInstrInfo());

  MachineBasicBlock::iterator MBBI = MBB.begin();
  DebugLoc dl = MBBI != MBB.end() ? MBBI->getDebugLoc() : DebugLoc();

  // allocate fixed size for simplicity
  uint64_t StackSize = 4 * 16;

   // Update stack size
  MFI->setStackSize(StackSize);

  BuildMI(MBB, MBBI, dl, TII.get(Nkmm::SUBri), Nkmm::SP)
      .addReg(Nkmm::SP)
      .addImm(StackSize);
}

void NkmmFrameLowering::
emitEpilogue(MachineFunction &MF, MachineBasicBlock &MBB) const {
  DEBUG(dbgs() << ">> NkmmFrameLowering::emitEpilogue <<\n");

  MachineBasicBlock::iterator MBBI = MBB.getLastNonDebugInstr();
  MachineFrameInfo *MFI            = MF.getFrameInfo();
  const NkmmInstrInfo &TII =
    *static_cast<const NkmmInstrInfo*>(STI.getInstrInfo());
  DebugLoc dl = MBBI->getDebugLoc();

  // Get the number of bytes from FrameInfo
  uint64_t StackSize = MFI->getStackSize();

  // Adjust stack.
  BuildMI(MBB, MBBI, dl, TII.get(Nkmm::ADDri), Nkmm::SP)
      .addReg(Nkmm::SP)
      .addImm(StackSize);
}
