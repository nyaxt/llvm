//===-- NkmmBaseInfo.h - Top level definitions for Nkmm -------- --*- C++
//-*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains small standalone helper functions and enum definitions for
// the Nkmm target useful for the compiler back-end and the MC libraries.
// As such, it deliberately does not include references to LLVM core
// code gen types, passes, etc..
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_NKMM_MCTARGETDESC_NKMMBASEINFO_H
#define LLVM_LIB_TARGET_NKMM_MCTARGETDESC_NKMMBASEINFO_H

#include "NkmmMCTargetDesc.h"
#include "llvm/Support/ErrorHandling.h"

namespace llvm {

// Enums corresponding to Nkmm condition codes
namespace NkmmCC {
enum CondCodes { // Meaning (integer)          Meaning (floating-point)
  EQ,            // Equal                      Equal
  NE,            // Not equal                  Not equal, or unordered
  GE,            // Greater than or equal      Greater than or equal
  LT,            // Less than                  Less than, or unordered
  GT,            // Greater than               Greater than
  LE,            // Less than or equal         <, ==, or unordered
  Count };

inline static CondCodes getOppositeCondition(CondCodes CC) {
  switch (CC) {
  default:
    llvm_unreachable("Unknown condition code");
  case EQ:
    return NE;
  case NE:
    return EQ;
  case GE:
    return LT;
  case LT:
    return GE;
  case GT:
    return LE;
  case LE:
    return GT;
  }
}
} // namespace NkmmCC

inline static const char *NkmmCondCodeToString(NkmmCC::CondCodes CC) {
  switch (CC) {
  case NkmmCC::EQ:
    return "==";
  case NkmmCC::NE:
    return "!=";
  case NkmmCC::GE:
    return ">=";
  case NkmmCC::LT:
    return "<";
  case NkmmCC::GT:
    return ">";
  case NkmmCC::LE:
    return "<=";
  }
  llvm_unreachable("Unknown condition code");
}

} // end namespace llvm;

#endif
