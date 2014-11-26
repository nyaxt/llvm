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
#include "NkmmMCAsmInfo.h"
#include "InstPrinter/NkmmInstPrinter.h"
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

static MCAsmInfo *createNkmmMCAsmInfo(const MCRegisterInfo &MRI, StringRef TT) {
  MCAsmInfo *MAI = new NkmmMCAsmInfo(TT);

  unsigned SP = MRI.getDwarfRegNum(Nkmm::SP, true);
  MCCFIInstruction Inst = MCCFIInstruction::createDefCfa(nullptr, SP, 0);
  MAI->addInitialFrameState(Inst);

  return MAI;
}

static MCInstrInfo *createNkmmMCInstrInfo() {
  MCInstrInfo *X = new MCInstrInfo();
  InitNkmmMCInstrInfo(X);
  return X;
}

/*
static MCStreamer *
createMCAsmStreamer(MCContext &Ctx, formatted_raw_ostream &OS,
                    bool isVerboseAsm, bool useDwarfDirectory,
                    MCInstPrinter *InstPrint, MCCodeEmitter *CE,
                    MCAsmBackend *TAB, bool ShowInst) {
  MCStreamer *S = llvm::createAsmStreamer(
      Ctx, OS, isVerboseAsm, useDwarfDirectory, InstPrint, CE, TAB, ShowInst);
  new NkmmTargetAsmStreamer(*S, OS);
  return S;
}
*/
static MCInstPrinter *createNkmmMCInstPrinter(const Target &T,
                                              unsigned SyntaxVariant,
                                              const MCAsmInfo &MAI,
                                              const MCInstrInfo &MII,
                                              const MCRegisterInfo &MRI,
                                              const MCSubtargetInfo &STI) {
  return new NkmmInstPrinter(MAI, MII, MRI);
}

extern "C" void LLVMInitializeNkmmTargetMC() {
  RegisterMCAsmInfoFn X(TheNkmmTarget, createNkmmMCAsmInfo);
  /*
  TargetRegistry::RegisterMCCodeGenInfo(TheNkmmTarget,
                                        createNkmmMCCodeGenInfo);
                                        */
  TargetRegistry::RegisterMCInstrInfo(TheNkmmTarget, createNkmmMCInstrInfo);
  /*
  TargetRegistry::RegisterMCRegInfo(TheNkmmTarget, createNkmmMCRegisterInfo);
  TargetRegistry::RegisterMCCodeEmitter(TheNkmmTarget,
                                        createNkmmMCCodeEmitterEB);
  TargetRegistry::RegisterMCObjectStreamer(TheNkmmTarget, createMCStreamer);
  TargetRegistry::RegisterAsmStreamer(TheNkmmTarget, createMCAsmStreamer);
  */
  /*
  TargetRegistry::RegisterNullStreamer(TheNkmmTarget, createNkmmNullStreamer);
  TargetRegistry::RegisterMCAsmBackend(TheNkmmTarget,
                                       createNkmmAsmBackendEB32);
  TargetRegistry::RegisterMCSubtargetInfo(TheNkmmTarget,
                                          createNkmmMCSubtargetInfo);
                                        */
  TargetRegistry::RegisterMCInstPrinter(TheNkmmTarget,
                                        createNkmmMCInstPrinter);
}
