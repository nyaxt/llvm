//===-- NkmmRegisterInfo.cpp - MIPS Register Information -== --------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the MIPS implementation of the TargetRegisterInfo class.
//
//===----------------------------------------------------------------------===//

#include "NkmmRegisterInfo.h"
#include "Nkmm.h"
#include "llvm/ADT/BitVector.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DebugInfo.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Type.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Target/TargetFrameLowering.h"
#include "llvm/Target/TargetInstrInfo.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetOptions.h"

#include "MCTargetDesc/NkmmMCTargetDesc.h"

#define DEBUG_TYPE "nkmm-reg-info"

#define GET_REGINFO_TARGET_DESC
#include "NkmmGenRegisterInfo.inc"

using namespace llvm;

NkmmRegisterInfo::NkmmRegisterInfo(const TargetInstrInfo& tii)
  : NkmmGenRegisterInfo(Nkmm::R0), TII(tii) {}

const MCPhysReg *
NkmmRegisterInfo::getCalleeSavedRegs(const MachineFunction *MF) const {
  return CSR_SaveList;
}

const uint32_t* NkmmRegisterInfo::getCallPreservedMask(CallingConv::ID) const {
  return CSR_RegMask;
}

BitVector NkmmRegisterInfo::getReservedRegs(const MachineFunction &MF) const {
  BitVector Reserved(getNumRegs());
  Reserved.set(Nkmm::PC);
  return Reserved;
}

void NkmmRegisterInfo::
eliminateFrameIndex(MachineBasicBlock::iterator II, int SPAdj,
                    unsigned FIOperandNum, RegScavenger *RS) const {
  DEBUG(dbgs() << ">> NkmmRegisterInfo::eliminateFrameIndex <<\n";);

  MachineInstr &MI = *II;
  const MachineFunction &MF = *MI.getParent()->getParent();

  int FrameIndex = MI.getOperand(FIOperandNum).getIndex();
  uint64_t stackSize = MF.getFrameInfo()->getStackSize();
  int64_t spOffset = MF.getFrameInfo()->getObjectOffset(FrameIndex);
  int64_t Offset = spOffset + stackSize + MI.getOperand(FIOperandNum+1).getImm();
  unsigned FrameReg = Nkmm::SP;

  DEBUG(errs() 
        << "\nFunction : " << MF.getFunction()->getName() << "\n"
        << "<--------->\n" << MI
        << "FrameIndex : " << FrameIndex << "\n"
        << "spOffset   : " << spOffset << "\n"
        << "stackSize  : " << stackSize << "\n"
        << "Offset     : " << Offset << "\n" << "<--------->\n");

  DEBUG(errs() << "Before:" << MI);
  MI.getOperand(FIOperandNum).ChangeToRegister(FrameReg, false);
  MI.getOperand(FIOperandNum+1).ChangeToImmediate(Offset);
  DEBUG(errs() << "After:" << MI);
}

unsigned NkmmRegisterInfo::getFrameRegister(const MachineFunction &) const {
  llvm_unreachable("not implemented");
  return 0;
  //return Nkmm::SP;
}
