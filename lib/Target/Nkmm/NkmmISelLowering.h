//===-- NkmmISelLowering.h - Nkmm DAG Lowering Interface --------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the interfaces that Nkmm uses to lower LLVM code into a
// selection DAG.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_NKMM_NKMMISELLOWERING_H
#define LLVM_LIB_TARGET_NKMM_NKMMISELLOWERING_H

#include "Nkmm.h"
#include "llvm/CodeGen/CallingConvLower.h"
#include "llvm/CodeGen/SelectionDAG.h"
#include "llvm/IR/Function.h"
#include "llvm/Target/TargetLowering.h"
#include <deque>
#include <string>

namespace llvm {
  namespace NkmmISD {
    enum NodeType {
      // Start the numbering from where ISD NodeType finishes.
      FIRST_NUMBER = ISD::BUILTIN_OP_END,

      Call,
      Ret,
    };
  }

  //===--------------------------------------------------------------------===//
  // TargetLowering Implementation
  //===--------------------------------------------------------------------===//
  class NkmmFunctionInfo;
  class NkmmSubtarget;
  class NkmmCCState;

  class NkmmTargetLowering : public TargetLowering  {
    const NkmmSubtarget &Subtarget;
  public:
    explicit NkmmTargetLowering(const NkmmTargetMachine &TM);

    MVT getScalarShiftAmountTy(EVT LHSTy) const override { return MVT::i32; }

    /// getTargetNodeName - This method returns the name of a target specific
    //  DAG node.
    const char *getTargetNodeName(unsigned Opcode) const override;

    bool CanLowerReturn(CallingConv::ID CallConv, MachineFunction &MF,
                        bool isVarArg,
                        const SmallVectorImpl<ISD::OutputArg> &Outs,
                        LLVMContext &Context) const override;

    SDValue
      LowerFormalArguments(SDValue Chain,
                           CallingConv::ID CallConv, bool isVarArg,
                           const SmallVectorImpl<ISD::InputArg> &Ins,
                           SDLoc dl, SelectionDAG &DAG,
                           SmallVectorImpl<SDValue> &InVals) const override;

    SDValue LowerCall(TargetLowering::CallLoweringInfo &CLI,
                      SmallVectorImpl<SDValue> &InVals) const override;

    SDValue LowerReturn(SDValue Chain,
                        CallingConv::ID CallConv, bool isVarArg,
                        const SmallVectorImpl<ISD::OutputArg> &Outs,
                        const SmallVectorImpl<SDValue> &OutVals,
                        SDLoc dl, SelectionDAG &DAG) const override;
  private:
    // Lower Operand helpers
    SDValue LowerCallResult(SDValue Chain, SDValue InFlag,
                            CallingConv::ID CallConv, bool isVarArg,
                            const SmallVectorImpl<ISD::InputArg> &Ins, SDLoc dl,
                            SelectionDAG &DAG, SmallVectorImpl<SDValue> &InVals,
                            TargetLowering::CallLoweringInfo &CLI) const;
  };
}

#endif
