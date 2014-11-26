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
#include "MCTargetDesc/NkmmMCTargetDesc.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/ADT/STLExtras.h"

#define GET_INSTRINFO_CTOR_DTOR
#include "NkmmGenInstrInfo.inc"

using namespace llvm;

NkmmInstrInfo::NkmmInstrInfo()
    : RI(*this) { }

NkmmInstrInfo::~NkmmInstrInfo() {}

const NkmmRegisterInfo &NkmmInstrInfo::getRegisterInfo() const {
  return RI;
}

bool NkmmInstrInfo::expandPostRAPseudo(MachineBasicBlock::iterator MI) const {
  MachineBasicBlock &MBB = *MI->getParent();
  switch(MI->getDesc().getOpcode()) {
  default:
    return false;
  case Nkmm::RetSP:
    BuildMI(MBB, MI, MI->getDebugLoc(), get(Nkmm::PseudoRET)).addReg(Nkmm::SP);
    break;
  }

  MBB.erase(MI);
  return true;
}

