//===-- NkmmMachineFunctionInfo.h - Private data used for Nkmm ----*- C++ -*-=//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the Nkmm specific subclass of MachineFunctionInfo.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_NKMM_NKMMMACHINEFUNCTION_H
#define LLVM_LIB_TARGET_NKMM_NKMMMACHINEFUNCTION_H

#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"

namespace llvm {

/// NkmmFunctionInfo - This class is derived from MachineFunction private
/// Nkmm target-specific information for each MachineFunction.
class NkmmFunctionInfo : public MachineFunctionInfo {
public:
  NkmmFunctionInfo(MachineFunction &MF)
    : MF(MF) {}

private:
  virtual void anchor();

  MachineFunction& MF;
};

} // end of namespace llvm

#endif
