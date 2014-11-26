//===-- llvm/Target/NkmmTargetObjectFile.h - Nkmm Object Info ---*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_NKMM_NKMMTARGETOBJECTFILE_H
#define LLVM_LIB_TARGET_NKMM_NKMMTARGETOBJECTFILE_H

#include "llvm/CodeGen/TargetLoweringObjectFileImpl.h"

namespace llvm {

  class NkmmTargetObjectFile : public TargetLoweringObjectFileELF {
    const MCSection *SmallDataSection;
    const MCSection *SmallBSSSection;
  public:
    void Initialize(MCContext &Ctx, const TargetMachine &TM) override;
  };

} // end namespace llvm

#endif
