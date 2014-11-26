//===-- NkmmSEISelDAGToDAG.h - A Dag to Dag Inst Selector for NkmmSE -----===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// Subclass of NkmmDAGToDAGISel specialized for mips32/64.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_NKMM_NKMMSEISELDAGTODAG_H
#define LLVM_LIB_TARGET_NKMM_NKMMSEISELDAGTODAG_H

#include "NkmmISelDAGToDAG.h"

namespace llvm {

class NkmmSEDAGToDAGISel : public NkmmDAGToDAGISel {

public:
  explicit NkmmSEDAGToDAGISel(NkmmTargetMachine &TM) : NkmmDAGToDAGISel(TM) {}

private:

  bool runOnMachineFunction(MachineFunction &MF) override;

  void addDSPCtrlRegOperands(bool IsDef, MachineInstr &MI,
                             MachineFunction &MF);

  unsigned getMSACtrlReg(const SDValue RegIdx) const;

  bool replaceUsesWithZeroReg(MachineRegisterInfo *MRI, const MachineInstr&);

  std::pair<SDNode*, SDNode*> selectMULT(SDNode *N, unsigned Opc, SDLoc dl,
                                         EVT Ty, bool HasLo, bool HasHi);

  SDNode *selectAddESubE(unsigned MOp, SDValue InFlag, SDValue CmpLHS,
                         SDLoc DL, SDNode *Node) const;

  bool selectAddrFrameIndex(SDValue Addr, SDValue &Base, SDValue &Offset) const;
  bool selectAddrFrameIndexOffset(SDValue Addr, SDValue &Base, SDValue &Offset,
                                  unsigned OffsetBits) const;

  bool selectAddrRegImm(SDValue Addr, SDValue &Base,
                        SDValue &Offset) const override;

  bool selectAddrRegReg(SDValue Addr, SDValue &Base,
                        SDValue &Offset) const override;

  bool selectAddrDefault(SDValue Addr, SDValue &Base,
                         SDValue &Offset) const override;

  bool selectIntAddr(SDValue Addr, SDValue &Base,
                     SDValue &Offset) const override;

  bool selectAddrRegImm10(SDValue Addr, SDValue &Base,
                          SDValue &Offset) const;

  bool selectAddrRegImm12(SDValue Addr, SDValue &Base,
                          SDValue &Offset) const;

  bool selectIntAddrMM(SDValue Addr, SDValue &Base,
                       SDValue &Offset) const override;

  bool selectIntAddrMSA(SDValue Addr, SDValue &Base,
                        SDValue &Offset) const override;

  /// \brief Select constant vector splats.
  bool selectVSplat(SDNode *N, APInt &Imm) const override;
  /// \brief Select constant vector splats whose value fits in a given integer.
  bool selectVSplatCommon(SDValue N, SDValue &Imm, bool Signed,
                                  unsigned ImmBitSize) const;
  /// \brief Select constant vector splats whose value fits in a uimm1.
  bool selectVSplatUimm1(SDValue N, SDValue &Imm) const override;
  /// \brief Select constant vector splats whose value fits in a uimm2.
  bool selectVSplatUimm2(SDValue N, SDValue &Imm) const override;
  /// \brief Select constant vector splats whose value fits in a uimm3.
  bool selectVSplatUimm3(SDValue N, SDValue &Imm) const override;
  /// \brief Select constant vector splats whose value fits in a uimm4.
  bool selectVSplatUimm4(SDValue N, SDValue &Imm) const override;
  /// \brief Select constant vector splats whose value fits in a uimm5.
  bool selectVSplatUimm5(SDValue N, SDValue &Imm) const override;
  /// \brief Select constant vector splats whose value fits in a uimm6.
  bool selectVSplatUimm6(SDValue N, SDValue &Imm) const override;
  /// \brief Select constant vector splats whose value fits in a uimm8.
  bool selectVSplatUimm8(SDValue N, SDValue &Imm) const override;
  /// \brief Select constant vector splats whose value fits in a simm5.
  bool selectVSplatSimm5(SDValue N, SDValue &Imm) const override;
  /// \brief Select constant vector splats whose value is a power of 2.
  bool selectVSplatUimmPow2(SDValue N, SDValue &Imm) const override;
  /// \brief Select constant vector splats whose value is the inverse of a
  /// power of 2.
  bool selectVSplatUimmInvPow2(SDValue N, SDValue &Imm) const override;
  /// \brief Select constant vector splats whose value is a run of set bits
  /// ending at the most significant bit
  bool selectVSplatMaskL(SDValue N, SDValue &Imm) const override;
  /// \brief Select constant vector splats whose value is a run of set bits
  /// starting at bit zero.
  bool selectVSplatMaskR(SDValue N, SDValue &Imm) const override;

  std::pair<bool, SDNode*> selectNode(SDNode *Node) override;

  void processFunctionAfterISel(MachineFunction &MF) override;

  // Insert instructions to initialize the global base register in the
  // first MBB of the function.
  void initGlobalBaseReg(MachineFunction &MF);
};

FunctionPass *createNkmmSEISelDag(NkmmTargetMachine &TM);

}

#endif
