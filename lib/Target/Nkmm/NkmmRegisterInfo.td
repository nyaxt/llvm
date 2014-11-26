//===-- NkmmRegisterInfo.td - Nkmm Register defs -----------*- tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
//  Declarations that describe the NKMM register file
//===----------------------------------------------------------------------===//

class Unallocatable {
  bit isAllocatable = 0;
}

class NkmmReg<bits<16> Enc, string n> : Register<n> {
  let HWEncoding = Enc;
  let Namespace = "Nkmm";
}

// Nkmm CPU Registers
class NkmmGPRReg<bits<16> Enc, string n> : NkmmReg<Enc, n>;

// FIXME: special case registers for SMULADD?

//===----------------------------------------------------------------------===//
//  Registers
//===----------------------------------------------------------------------===//

let Namespace = "Nkmm" in {
  // General Purpose Registers
  def R0 : NkmmGPRReg<0, "R0">, DwarfRegNum<[0]>;
  def R1 : NkmmGPRReg<1, "R1">, DwarfRegNum<[1]>;
  def R2 : NkmmGPRReg<2, "R2">, DwarfRegNum<[2]>;
  def R3 : NkmmGPRReg<3, "R3">, DwarfRegNum<[3]>;
  def R4 : NkmmGPRReg<4, "R4">, DwarfRegNum<[4]>;
  def R5 : NkmmGPRReg<5, "R5">, DwarfRegNum<[5]>;

  // SP register
  def SP : NkmmGPRReg<6, "SP">, DwarfRegNum<[6]>;

  // PC register
  def PC : NkmmGPRReg<7, "PC">, DwarfRegNum<[7]>;
}

//===----------------------------------------------------------------------===//
// Register Classes
//===----------------------------------------------------------------------===//

def CPURegs :
  RegisterClass<"Nkmm", [i32], 32, (add
  R0, R1, R2, R3, R4, R5,
  // Reserved
  SP, PC)>;

def GPROut : RegisterClass<"Nkmm", [i32], 32, (add (sub CPURegs, PC))>;

def CPUSPReg : RegisterClass<"Nkmm", [i32], 32, (add SP)>, Unallocatable;
