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

//===----------------------------------------------------------------------===//
// Nkmm Operand, Complex Patterns and Transformations Definitions.
//===----------------------------------------------------------------------===//

// unsigned immediate 16bit
def uimm16 : Operand<i32> {
  let DecoderMethod = "printUnsignedImm";
}

def mem : Operand<i32> {
  let PrintMethod = "printMemOperand";
  let MIOperandInfo = (ops CPURegs, uimm16);
  let EncoderMethod = "getMemEncoding";
}

// Nkmm Address Mode! SDNode frameindex could possibily be a match
// since load and store instructions from stack used it.
def addr :
  ComplexPattern<iPTR, 2, "SelectAddr", [frameindex]>;

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

class ArithLogicInst<bits<3> op, string opstr, SDNode OpNode,
  RegisterClass RC, bit isComm = 0>
  : NkmmInst<(outs RC:$rc), (ins RC:$ra, RC:$rb),
    !strconcat("$rc <= $ra ", opstr, " $rc"),
    [(set RC:$rc, (OpNode RC:$ra, RC:$rb))], IIAlu> {
  let isReMaterializable = 1;
  let isCommutable = isComm;

  let ALUSel = op;
}

//===----------------------------------------------------------------------===//
// Nkmm Instructions
//===----------------------------------------------------------------------===//

def ADD : ArithLogicInst<0x0, "+", add, CPURegs, 1>;
def SUB : ArithLogicInst<0x1, "-", sub, CPURegs, 0>;

def LOAD : NkmmInst<(outs CPURegs:$rc), (ins uimm16:$imm),
  "$rc <= $imm", [(set CPURegs:$rc, addr:$imm)], IIAlu> {
  // FIXME: for disasm: let DecoderMethod = "DecodeImm";
}

def RET : NkmmInstPseudo<(outs), (ins GPROut:$ra), "nise_ret", [(brind GPROut:$ra)]> {
  let isReturn = 1;
  let isCodeGenOnly = 1;
  let hasCtrlDep = 1;
  let hasExtraSrcRegAllocReq = 1;
}

def NOP : NkmmInst<(outs), (ins), "nop", [], IIAlu>;

