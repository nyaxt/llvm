//===-- NkmmMCTargetDesc.cpp - Nkmm Target Descriptions -------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file provides Nkmm specific target descriptions.
//
//===----------------------------------------------------------------------===//

#include "NkmmMCTargetDesc.h"

#include "llvm/MC/MachineLocation.h"
#include "llvm/MC/MCCodeGenInfo.h"
#include "llvm/MC/MCELFStreamer.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/FormattedStream.h"
#include "llvm/Support/TargetRegistry.h"

using namespace llvm;

#define GET_INSTRINFO_MC_DESC
#include "NkmmGenInstrInfo.inc"

#define GET_SUBTARGETINFO_MC_DESC
#include "NkmmGenSubtargetInfo.inc"

#define GET_REGINFO_MC_DESC
#include "NkmmGenRegisterInfo.inc"

extern "C" void LLVMInitializeNkmmTargetMC() {
}
