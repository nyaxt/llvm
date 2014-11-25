//===-- NkmmSubtarget.cpp - Nkmm Subtarget Information --------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the Nkmm specific subclass of TargetSubtargetInfo.
//
//===----------------------------------------------------------------------===//

#include "NkmmSubtarget.h"
#include "Nkmm.h"
#include "llvm/Support/TargetRegistry.h"

#define DEBUG_TYPE "nkmm-subtarget"

#define GET_SUBTARGETINFO_CTOR
#define GET_SUBTARGETINFO_TARGET_DESC
#include "NkmmGenSubtargetInfo.inc"

using namespace llvm;

NkmmSubtarget::NkmmSubtarget(const std::string &TT,
                             const std::string &CPU,
                             const std::string &FS)
    : NkmmGenSubtargetInfo(TT, CPU, FS) {
  ParseSubtargetFeatures("generic", FS);    
}

void NkmmSubtarget::anchor() {}
