//===-- NkmmMCAsmInfo.h - Nkmm Asm Info ------------------------*- C++ -*--===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the declaration of the NkmmMCAsmInfo class.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_NKMM_MCTARGETDESC_NKMMMCASMINFO_H
#define LLVM_LIB_TARGET_NKMM_MCTARGETDESC_NKMMMCASMINFO_H

#include "llvm/MC/MCAsmInfoELF.h"

namespace llvm {
class StringRef;

class NkmmMCAsmInfo : public MCAsmInfoELF {
  void anchor() override;

public:
  explicit NkmmMCAsmInfo(StringRef TT);
};

} // namespace llvm

#endif
