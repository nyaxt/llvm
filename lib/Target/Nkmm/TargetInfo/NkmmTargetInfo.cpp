//===-- NkmmTargetInfo.cpp - Nkmm Target Implementation -------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "Nkmm.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/TargetRegistry.h"
using namespace llvm;

Target llvm::TheNkmmTarget;

extern "C" void LLVMInitializeNkmmTargetInfo() {
  RegisterTarget<Triple::nkmm> X(TheNkmmTarget, "nkmm", "Nkmm");
}
