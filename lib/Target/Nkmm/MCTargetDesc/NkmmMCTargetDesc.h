//===-- NkmmMCTargetDesc.h - Nkmm Target Descriptions -----------*- C++ -*-===//
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

#ifndef LLVM_LIB_TARGET_NKMM_MCTARGETDESC_NKMMMCTARGETDESC_H
#define LLVM_LIB_TARGET_NKMM_MCTARGETDESC_NKMMMCTARGETDESC_H

#include "llvm/Support/DataTypes.h"

namespace llvm {

class Target;

extern Target TheNkmmTarget;

} // End llvm namespace

// Defines symbolic names for Nkmm registers.  This defines a mapping from
// register name to register number.
#define GET_REGINFO_ENUM
#include "NkmmGenRegisterInfo.inc"

// Defines symbolic names for the Nkmm instructions.
#define GET_INSTRINFO_ENUM
#include "NkmmGenInstrInfo.inc"

#define GET_SUBTARGETINFO_ENUM
#include "NkmmGenSubtargetInfo.inc"

#endif
