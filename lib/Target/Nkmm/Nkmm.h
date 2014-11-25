//===-- Nkmm.h - Top-level interface for Nkmm representation ----*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the entry points for global functions defined in
// the LLVM Nkmm back-end.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_NKMM_NKMM_H
#define LLVM_LIB_TARGET_NKMM_NKMM_H

#include "MCTargetDesc/NkmmMCTargetDesc.h"
#include "llvm/Target/TargetMachine.h"

namespace llvm {
  class NkmmTargetMachine;
  class FunctionPass;

  FunctionPass *createNkmmISelDag(NkmmTargetMachine &TM);
} // end namespace llvm;

#endif
