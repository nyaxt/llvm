//===-- NkmmTargetMachine.h - Define TargetMachine for Nkmm -----*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the Nkmm specific subclass of TargetMachine.
//
//===----------------------------------------------------------------------===//

#ifndef CPU0TARGETMACHINE_H
#define CPU0TARGETMACHINE_H

#include "NkmmSubtarget.h"
#include "NkmmInstrInfo.h"
#include "llvm/CodeGen/Passes.h"
#include "llvm/CodeGen/SelectionDAGISel.h"
#include "llvm/Target/TargetFrameLowering.h"
#include "llvm/Target/TargetMachine.h"

namespace llvm {
class formatted_raw_ostream;

class NkmmTargetMachine : public LLVMTargetMachine {
  NkmmSubtarget DefaultSubtarget;
  NkmmInstrInfo InstrInfo;
  std::unique_ptr<TargetLoweringObjectFile> TLOF;

  virtual void anchor();
public:
  NkmmTargetMachine(const Target &T, StringRef TT, StringRef CPU, 
                    StringRef FS, const TargetOptions &Options, 
                    Reloc::Model RM, CodeModel::Model CM, 
                    CodeGenOpt::Level OL);
  ~NkmmTargetMachine() override;

  const NkmmSubtarget *getSubtargetImpl() const override {
    return &DefaultSubtarget;
  }

  const NkmmInstrInfo *getInstrInfo() const {
    return &InstrInfo; 
  }

  TargetLoweringObjectFile *getObjFileLowering() const override {
    return TLOF.get();
  }

  TargetPassConfig *createPassConfig(PassManagerBase &PM) override;
};

} // End llvm namespace

#endif
