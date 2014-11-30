//===-- NkmmCallingConv.td - Calling Conventions for Nkmm --*- tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
// This describes the calling conventions for Nkmm architecture.
//===----------------------------------------------------------------------===//

def CC_Nkmm : CallingConv<[
  CCIfType<[i32], CCAssignToReg<[R0, R1, R2, R3]>>
]>;

def RetCC_Nkmm : CallingConv<[
  CCIfType<[i32], CCAssignToReg<[R0]>>
]>;

def CSR : CalleeSavedRegs<(add R4, R5)>;
