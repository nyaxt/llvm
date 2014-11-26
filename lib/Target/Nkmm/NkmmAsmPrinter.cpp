//===-- NkmmAsmPrinter.cpp - Nkmm LLVM Assembly Printer -------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains a printer that converts from our internal representation
// of machine-dependent LLVM code to GAS-format NKMM assembly language.
//
//===----------------------------------------------------------------------===//

#include "Nkmm.h"
#include "NkmmAsmPrinter.h"
#include "NkmmInstrInfo.h"
#include "NkmmMCInstLower.h"
#include "llvm/ADT/SmallString.h"
#include "llvm/ADT/StringExtras.h"
#include "llvm/ADT/Twine.h"
#include "llvm/CodeGen/MachineConstantPool.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/CodeGen/MachineJumpTableInfo.h"
#include "llvm/CodeGen/MachineMemOperand.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/InlineAsm.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Mangler.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCELFStreamer.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCSection.h"
#include "llvm/MC/MCSectionELF.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/Support/ELF.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Target/TargetLoweringObjectFile.h"
#include "llvm/Target/TargetOptions.h"
#include <string>

using namespace llvm;

#define DEBUG_TYPE "nkmm-asm-printer"

#include "NkmmGenMCPseudoLowering.inc"

void NkmmAsmPrinter::EmitInstruction(const MachineInstr *MI) {
  DEBUG(dbgs() << ">> NkmmAsmPinter::EmitInstruction <<\n");
  DEBUG(MI->dump());
  NkmmMCInstLower MCInstLowering(*this);
  MCInst TmpInst;
  MCInstLowering.Lower(MI, TmpInst);
  OutStreamer.EmitInstruction(TmpInst, getSubtargetInfo());
}

// Force static initialization.
extern "C" void LLVMInitializeNkmmAsmPrinter() {
  RegisterAsmPrinter<NkmmAsmPrinter> X(TheNkmmTarget);
}
