//===- NkmmInstrInfo.td - Target Description for Nkmm Target -*- tablegen -*-=//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the Nkmm implementation of the TargetInstrInfo class.
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// Nkmm profiles and nodes
//===----------------------------------------------------------------------===//

// Type profiles.
def SDT_NkmmBranchConditional :
    SDTypeProfile<0, 3,
    [SDTCisVT<0, OtherVT>, SDTCisVT<1, i32>, SDTCisVT<2, i32>]>;

def SDT_NkmmCompare : SDTypeProfile<0, 2, [SDTCisSameAs<0, 1>]>;

// Return
def NkmmRet : SDNode<"NkmmISD::Ret", SDTNone,
                     [SDNPHasChain, SDNPOptInGlue, SDNPVariadic]>;

def NkmmBranchConditional :
    SDNode<"NkmmISD::BranchConditional", SDT_NkmmBranchConditional,
    [SDNPHasChain, SDNPOptInGlue, SDNPOutGlue]>;

def NkmmCompare : SDNode<"NkmmISD::Compare", SDT_NkmmCompare,
    [SDNPOutGlue]>;

//===----------------------------------------------------------------------===//
// Nkmm Operand, Complex Patterns and Transformations Definitions.
//===----------------------------------------------------------------------===//

// unsigned immediate 16bit
def uimm16 : Operand<i32> {
  // let PrintMethod = "printUnsignedImm";
}

def uimm16Pat : PatLeaf<(imm), [{ return isInt<16>(N->getZExtValue()); }]>;

def condition : Operand<i32> {
  let PrintMethod = "printCondition";
} 

def conditionPat : PatLeaf<(imm), [{ return N->getZExtValue() < NkmmCC::Count; }]>;

def jmpiTarget    : Operand<OtherVT> {
  //let EncoderMethod = "getBranchTargetOpValue";
  let OperandType = "OPERAND_PCREL";
  // let PrintMethod = 
  //let DecoderMethod = "DecodeBranchTarget";
  //let ParserMatchClass = NkmmJumpTargetAsmOperand;
}

def NkmmMemAsmOperand : AsmOperandClass {
  let Name = "Mem";
  let ParserMethod = "parseMemOperand";
}

def mem : Operand<iPTR> {
  let PrintMethod = "printMemOperand";
  let MIOperandInfo = (ops CPURegs, uimm16);
  let EncoderMethod = "getMemEncoding";
  let ParserMatchClass = NkmmMemAsmOperand;
  let OperandType = "OPERAND_MEMORY";
}

// Nkmm Address Mode! SDNode frameindex could possibily be a match
// since load and store instructions from stack used it.
def addrRegWithImmOffset :
  ComplexPattern<iPTR, 2, "selectAddrRegWithImmOffset", []>;

class IsCommutable {
  bit isCommutable = 1;
}

//===----------------------------------------------------------------------===//
// Instruction format superclass
//===----------------------------------------------------------------------===//

class NkmmInst<dag outs, dag ins, string asmstr, list<dag> pattern,
  InstrItinClass itin> : Instruction {
  field bits<32> Inst;

  let Namespace = "Nkmm";
  let DecoderNamespace = "Nkmm";
  let Size = 4;

  bit RepZ = 0;
  bit MemWrite = 0;
  bits<3> DSel = 0;
  bits<3> ALUSel = 0;
  bits<3> ASel = 0;
  bits<3> BSel = 0;
  bit ImmEn = 0;
  bits<16> Imm16 = 0;

  let Inst{31} = RepZ;
  let Inst{30} = MemWrite;
  let Inst{29-27} = DSel;
  let Inst{26-24} = ALUSel;
  let Inst{23-21} = ASel;
  let Inst{20-18} = BSel;
  let Inst{17} = ImmEn;
  let Inst{16} = 0;
  let Inst{15-0} = Imm16;

  let OutOperandList = outs;
  let InOperandList  = ins;

  let AsmString   = asmstr;
  let Pattern     = pattern;
  let Itinerary   = itin;

  field bits<32> SoftFail = 0;
}

class NkmmInstPseudo<dag outs, dag ins, string asmstr, list<dag> pattern>
  : NkmmInst<outs, ins, asmstr, pattern, IIPseudo> {
  let isCodeGenOnly = 1;
  let isPseudo = 1;
}

//===----------------------------------------------------------------------===//
// Instructions specific format
//===----------------------------------------------------------------------===//

class ArithLogicInstRR<bits<3> op, string opstr, SDNode OpNode, bit isComm = 0>
  : NkmmInst<(outs CPURegs:$rc), (ins CPURegs:$ra, CPURegs:$rb),
    !strconcat("$rc <= $ra ", opstr, " $rb"),
    [(set CPURegs:$rc, (OpNode CPURegs:$ra, CPURegs:$rb))], IIAlu> {
  let isReMaterializable = 1;
  let isCommutable = isComm;

  let ALUSel = op;
}

class ArithLogicInstRI<bits<3> op, string opstr, SDNode OpNode, bit isComm = 0>
  : NkmmInst<(outs CPURegs:$rc), (ins CPURegs:$ra, uimm16:$imm),
    !strconcat("$rc <= $ra ", opstr, " $imm"),
    [(set CPURegs:$rc, (OpNode CPURegs:$ra, uimm16Pat:$imm))], IIAlu> {
  let isReMaterializable = 1;
  let isCommutable = isComm;

  let ALUSel = op;
  let ImmEn = 1;
}

//===----------------------------------------------------------------------===//
// Nkmm Instructions
//===----------------------------------------------------------------------===//

def ADDrr : ArithLogicInstRR<0x0, "+", add, 1>;
def SUBrr : ArithLogicInstRR<0x1, "-", sub, 0>;
def ADDri : ArithLogicInstRI<0x0, "+", add, 1>;
def SUBri : ArithLogicInstRI<0x1, "-", sub, 0>;

def CMPrr : NkmmInst<(outs), (ins CPURegs:$lhs, CPURegs:$rhs),
  "ST <= CMP($lhs, $rhs)", [(NkmmCompare CPURegs:$lhs, CPURegs:$rhs)], IIAlu> {
  let Defs = [ST];
}
def CMPri : NkmmInst<(outs), (ins CPURegs:$lhs, uimm16:$rhs),
  "ST <= CMP($lhs, $rhs)", [(NkmmCompare CPURegs:$lhs, uimm16Pat:$rhs)], IIAlu> {
  let Defs = [ST];
}

def MOVri : NkmmInst<(outs CPURegs:$dst), (ins uimm16:$imm),
  "$dst <= $imm", [(set CPURegs:$dst, uimm16Pat:$imm)], IIAlu> {
  // FIXME: for disasm: let DecoderMethod = "DecodeImm";
}

def MOVrr : NkmmInst<(outs CPURegs:$dst), (ins CPURegs:$src),
  "$dst <= $src", [(set CPURegs:$dst, CPURegs:$src)], IIAlu> {
  let ImmEn = 1;
  let ALUSel = 0x0;
}

def LOAD : NkmmInst<(outs CPURegs:$dst), (ins mem:$addr),
  "$dst <= [$addr]", [(set CPURegs:$dst, (load addrRegWithImmOffset:$addr))], IIAlu> {
  let DecoderMethod = "DecodeMem";
  let canFoldAsLoad = 1;
  let mayLoad = 1;
}
def STORE : NkmmInst<(outs), (ins CPURegs:$src, mem:$addr),
  "[$addr] <= $src", [(store CPURegs:$src, addrRegWithImmOffset:$addr)], IIAlu> {
  let DecoderMethod = "DecodeMem";
  let mayStore  = 1;
}

def JMPi : NkmmInst<(outs), (ins jmpiTarget:$jmptgt), "PC <= $jmptgt", [(br bb:$jmptgt)], IIAlu> {
  let isBranch = 1;
  let isTerminator = 1;
  let isBarrier = 1;
  let hasDelaySlot = 1;
}

def JMPr : NkmmInst<(outs), (ins CPURegs:$src),
  "PC <= $src", [(brind CPURegs:$src)], IIAlu> {
  let isBranch = 1;
  let isIndirectBranch = 1;
  // let isBarrier = 1;
}

def JMPm : NkmmInst<(outs), (ins mem:$addr),
  "PC <= [$addr]", [(brind (load addrRegWithImmOffset:$addr))], IIAlu> {
  let isBranch = 1;
  let isIndirectBranch = 1;
  // let isBarrier = 1;
}

def Jccri : NkmmInst<(outs), (ins jmpiTarget:$jmptgt, condition:$cc, CPUSTReg:$ccr),
  "if($cc) PC <= $jmptgt", [(NkmmBranchConditional bb:$jmptgt, conditionPat:$cc, CPUSTReg:$ccr)], IIAlu> {
  let isBranch = 1;
  let isBarrier = 1;
  let isTerminator = 1;
}

def RetPSP : NkmmInstPseudo<(outs), (ins), "pseudo_retpsp", [(NkmmRet)]> {
  let isReturn=1;
  let isTerminator=1;
  let hasDelaySlot=1;
  let isBarrier=1;
  let hasCtrlDep=1;
}

def PseudoRET : NkmmInst<(outs), (ins CPUSPReg:$target), "nise_ret", [], IIAlu> {
  let isReturn = 1;
  let isTerminator = 1;
  let isBarrier = 1;
  let hasCtrlDep = 1;
  let hasDelaySlot = 1;
  let hasExtraSrcRegAllocReq = 1;
}

def NOP : NkmmInst<(outs), (ins), "nop", [], IIAlu>;
