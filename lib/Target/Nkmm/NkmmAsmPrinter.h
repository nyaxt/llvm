//===-- NkmmAsmPrinter.h - Nkmm LLVM Assembly Printer ----------*- C++ -*--===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// Nkmm Assembly printer class.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_NKMM_NKMMASMPRINTER_H
#define LLVM_LIB_TARGET_NKMM_NKMMASMPRINTER_H

#include "NkmmMCInstLower.h"
#include "NkmmMachineFunction.h"
#include "NkmmSubtarget.h"
#include "llvm/CodeGen/AsmPrinter.h"
#include "llvm/Support/Compiler.h"
#include "llvm/Target/TargetMachine.h"

namespace llvm {
class MCStreamer;
class MachineInstr;
class MachineBasicBlock;
class NkmmTargetStreamer;
class Module;

class LLVM_LIBRARY_VISIBILITY NkmmAsmPrinter : public AsmPrinter {
  const NkmmSubtarget *Subtarget;
  const NkmmFunctionInfo *NkmmFI;
  NkmmMCInstLower MCInstLowering;

public:
  // We initialize the subtarget here and in runOnMachineFunction
  // since there are certain target specific flags (ABI) that could
  // reside on the TargetMachineh but are on the subtarget currently
  // and we need them for the beginning of file output before we've
  // seen a single function.
  explicit NkmmAsmPrinter(TargetMachine &TM, MCStreamer &Streamer)
      : AsmPrinter(TM, Streamer), Subtarget(&TM.getSubtarget<NkmmSubtarget>()),
        MCInstLowering(*this) {}

  const char *getPassName() const override { return "Nkmm Assembly Printer"; }

  void EmitInstruction(const MachineInstr *MI) override;

private:
  // tblgen'erated function.
  bool emitPseudoExpansionLowering(MCStreamer &OutStreamer,
                                   const MachineInstr *MI);

  void emitPseudoRET(MCStreamer &OutStreamer, const MachineInstr *MI);
};
}

#endif
