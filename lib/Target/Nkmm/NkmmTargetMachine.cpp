//===-- NkmmTargetMachine.cpp - Define TargetMachine for Nkmm -------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// Implements the info about Nkmm target spec.
//
//===----------------------------------------------------------------------===//

#include "NkmmTargetMachine.h"
#include "Nkmm.h"
#include "NkmmSubtarget.h"
#include "llvm/PassManager.h"
#include "llvm/CodeGen/Passes.h"
#include "llvm/Support/TargetRegistry.h"
using namespace llvm;

#define DEBUG_TYPE "Nkmm"

NkmmTargetMachine::NkmmTargetMachine(const Target &T, StringRef TT,
                                     StringRef CPU, StringRef FS,
                                     const TargetOptions &Options,
                                     Reloc::Model RM, CodeModel::Model CM,
                                     CodeGenOpt::Level OL)
    : LLVMTargetMachine(T, TT, CPU, FS, Options, RM, CM, OL),
      DefaultSubtarget(TT, CPU, FS, *this) {
  initAsmInfo();
}

NkmmTargetMachine::~NkmmTargetMachine() {}

void NkmmTargetMachine::anchor() {}

extern "C" void LLVMInitializeNkmmTarget() {
  RegisterTargetMachine<NkmmTargetMachine> X(TheNkmmTarget);
}
