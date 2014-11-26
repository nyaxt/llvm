//===-- NkmmISelLowering.cpp - Nkmm DAG Lowering Implementation -----------===//
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
#include "NkmmISelLowering.h"
#include "NkmmMachineFunction.h"
#include "NkmmSubtarget.h"
#include "NkmmTargetMachine.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/ADT/StringSwitch.h"
#include "llvm/CodeGen/CallingConvLower.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineJumpTableInfo.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/SelectionDAGISel.h"
#include "llvm/CodeGen/ValueTypes.h"
#include "llvm/IR/CallingConv.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/GlobalVariable.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"
#include <cctype>

using namespace llvm;

#define DEBUG_TYPE "mips-lower"

NkmmTargetLowering::NkmmTargetLowering(const NkmmTargetMachine &TM)
  : TargetLowering(TM)
  , Subtarget(*TM.getSubtargetImpl())
{
  setBooleanContents(ZeroOrOneBooleanContent);
  setBooleanVectorContents(ZeroOrOneBooleanContent);

  addRegisterClass(MVT::i32, &Nkmm::CPURegsRegClass);

  setLoadExtAction(ISD::EXTLOAD,  MVT::i1,  Promote);
  setLoadExtAction(ISD::EXTLOAD,  MVT::i8,  Promote);
  setLoadExtAction(ISD::EXTLOAD,  MVT::i16, Promote);
  setLoadExtAction(ISD::ZEXTLOAD, MVT::i1,  Promote);
  setLoadExtAction(ISD::ZEXTLOAD, MVT::i8,  Promote);
  setLoadExtAction(ISD::ZEXTLOAD, MVT::i16, Promote);
  setLoadExtAction(ISD::SEXTLOAD, MVT::i1,  Promote);
  setLoadExtAction(ISD::SEXTLOAD, MVT::i8,  Promote);
  setLoadExtAction(ISD::SEXTLOAD, MVT::i16, Promote);

  setMinFunctionAlignment(2);

  setStackPointerRegisterToSaveRestore(Nkmm::SP);

  computeRegisterProperties();
}

const char *NkmmTargetLowering::getTargetNodeName(unsigned Opcode) const {
  switch (Opcode) {
  case NkmmISD::Call: return "NkmmISD::Call";
  case NkmmISD::Ret: return "NkmmISD::Ret";
  default: return nullptr;
  }
}

#include "NkmmGenCallingConv.inc"

SDValue NkmmTargetLowering::LowerCallResult(SDValue Chain, SDValue InFlag,
                            CallingConv::ID CallConv, bool isVarArg,
                            const SmallVectorImpl<ISD::InputArg> &Ins, SDLoc dl,
                            SelectionDAG &DAG, SmallVectorImpl<SDValue> &InVals,
                            TargetLowering::CallLoweringInfo &CLI) const
{
  // Assign locations to each value returned by this call.
  SmallVector<CCValAssign, 16> RVLocs;
  CCState CCInfo(CallConv, isVarArg, DAG.getMachineFunction(), RVLocs,
      *DAG.getContext());

  CCInfo.AnalyzeCallResult(Ins, RetCC_Nkmm);

  for (unsigned i = 0; i != RVLocs.size(); ++i) {
    Chain = DAG.getCopyFromReg(Chain, dl, RVLocs[i].getLocReg(),
                               RVLocs[i].getValVT(), InFlag).getValue(1);
    InFlag = Chain.getValue(2);
    InVals.push_back(Chain.getValue(0));
  }
  (void)InFlag;

  return Chain;  
}

SDValue NkmmTargetLowering::LowerFormalArguments(
    SDValue Chain, CallingConv::ID CallConv, bool isVarArg,
    const SmallVectorImpl<ISD::InputArg> &Ins, SDLoc dl, SelectionDAG &DAG,
    SmallVectorImpl<SDValue> &InVals) const
{
  return Chain;  
}

SDValue NkmmTargetLowering::LowerCall(TargetLowering::CallLoweringInfo &CLI,
                                      SmallVectorImpl<SDValue> &InVals) const
{
  SelectionDAG &DAG                     = CLI.DAG;
  SDLoc DL                              = CLI.DL;
  SmallVectorImpl<ISD::OutputArg> &Outs = CLI.Outs;
  SmallVectorImpl<SDValue> &OutVals     = CLI.OutVals;
  SmallVectorImpl<ISD::InputArg> &Ins   = CLI.Ins;
  SDValue Chain                         = CLI.Chain;
  SDValue Callee                        = CLI.Callee;
  bool &IsTailCall                      = CLI.IsTailCall;
  CallingConv::ID CallConv              = CLI.CallConv;
  bool IsVarArg                         = CLI.IsVarArg;

  SDValue InFlag = Chain.getValue(1);
  return LowerCallResult(Chain, InFlag, CallConv, IsVarArg, Ins, DL, DAG,
                         InVals, CLI);
}

bool
NkmmTargetLowering::CanLowerReturn(CallingConv::ID CallConv,
                                   MachineFunction &MF, bool IsVarArg,
                                   const SmallVectorImpl<ISD::OutputArg> &Outs,
                                   LLVMContext &Context) const {
  SmallVector<CCValAssign, 16> RVLocs;
  CCState CCInfo(CallConv, IsVarArg, MF, RVLocs, Context);
  return CCInfo.CheckReturn(Outs, RetCC_Nkmm);
}

SDValue NkmmTargetLowering::LowerReturn(
    SDValue Chain, CallingConv::ID CallConv, bool isVarArg,
    const SmallVectorImpl<ISD::OutputArg> &Outs,
    const SmallVectorImpl<SDValue> &OutVals, SDLoc DL, SelectionDAG &DAG) const
{
  SmallVector<SDValue, 4> RetOps(1, Chain);
  return DAG.getNode(NkmmISD::Ret, DL, MVT::Other, RetOps);
}
