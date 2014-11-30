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

#define DEBUG_TYPE "nkmm-lower"

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

// addLiveIn - This helper function adds the specified physical register to the
// MachineFunction as a live in value.  It also creates a corresponding
// virtual register for it.
static unsigned
addLiveIn(MachineFunction &MF, unsigned PReg, const TargetRegisterClass *RC)
{
  unsigned VReg = MF.getRegInfo().createVirtualRegister(RC);
  MF.getRegInfo().addLiveIn(PReg, VReg);
  return VReg;
}

SDValue NkmmTargetLowering::LowerFormalArguments(
    SDValue Chain, CallingConv::ID CallConv, bool isVarArg,
    const SmallVectorImpl<ISD::InputArg> &Ins, SDLoc DL, SelectionDAG &DAG,
    SmallVectorImpl<SDValue> &InVals) const
{
  MachineFunction &MF = DAG.getMachineFunction();
  MachineFrameInfo *MFI = MF.getFrameInfo();
  NkmmFunctionInfo *NkmmFI = MF.getInfo<NkmmFunctionInfo>();

  // Used with vargs to acumulate store chains.
  std::vector<SDValue> OutChains;

  // Assign locations to all of the incoming arguments.
  SmallVector<CCValAssign, 16> ArgLocs;
  CCState CCInfo(CallConv, isVarArg, DAG.getMachineFunction(), ArgLocs,
      *DAG.getContext());
  Function::const_arg_iterator FuncArg =
    DAG.getMachineFunction().getFunction()->arg_begin();

  CCInfo.AnalyzeFormalArguments(Ins, CC_Nkmm);

  unsigned CurArgIdx = 0;
  CCInfo.rewindByValRegsInfo();

  for (unsigned i = 0, e = ArgLocs.size(); i != e; ++i) {
    CCValAssign &VA = ArgLocs[i];
    std::advance(FuncArg, Ins[i].OrigArgIndex - CurArgIdx);
    CurArgIdx = Ins[i].OrigArgIndex;
    EVT ValVT = VA.getValVT();
    ISD::ArgFlagsTy Flags = Ins[i].Flags;
    bool IsRegLoc = VA.isRegLoc();

    if (Flags.isByVal()) {
      llvm_unreachable("struct arg not implemented!");
      continue;
    }

    if (!IsRegLoc)
      llvm_unreachable("recv args from stack not implemented!");

    // Arguments stored on registers
    MVT RegVT = VA.getLocVT();
    unsigned ArgReg = VA.getLocReg();
    const TargetRegisterClass *RC = getRegClassFor(RegVT);

    // Transform the arguments stored on
    // physical registers into virtual ones
    unsigned Reg = addLiveIn(DAG.getMachineFunction(), ArgReg, RC);
    SDValue ArgValue = DAG.getCopyFromReg(Chain, DL, Reg, RegVT);

    if (VA.getLocInfo() != CCValAssign::Full)
      llvm_unreachable("recv arg with non full val assign not implemented!");
      
    InVals.push_back(ArgValue);
  }

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
  // CCValAssign - represent the assignment of
  // the return value to a location
  SmallVector<CCValAssign, 16> RVLocs;
  MachineFunction &MF = DAG.getMachineFunction();

  CCState CCInfo(CallConv, isVarArg, DAG.getMachineFunction(), RVLocs,
      *DAG.getContext());
  CCInfo.AnalyzeReturn(Outs, RetCC_Nkmm);

  SDValue Flag;
  SmallVector<SDValue, 4> RetOps(1, Chain);

  // Copy the result values into the output registers.
  for (unsigned i = 0; i != RVLocs.size(); ++i) {
    SDValue Val = OutVals[i];
    CCValAssign &VA = RVLocs[i];
    assert(VA.isRegLoc() && "Can only return in registers!");

    Chain = DAG.getCopyToReg(Chain, DL, VA.getLocReg(), Val, Flag);

    Flag = Chain.getValue(1);
    RetOps.push_back(DAG.getRegister(VA.getLocReg(), VA.getLocVT()));
  }

  if (MF.getFunction()->hasStructRetAttr())
    llvm_unreachable("struct return not implemented!");

  RetOps[0] = Chain;  // Update chain.
  // Add the flag if we have it.
  if (Flag.getNode())
    RetOps.push_back(Flag);

  return DAG.getNode(NkmmISD::Ret, DL, MVT::Other, RetOps);
}
