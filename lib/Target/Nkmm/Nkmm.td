//===-- Nkmm.td - Describe the Nkmm Target Machine ---------*- tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
// This is the top level entry point for the Nkmm target.
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// Target-independent interfaces
//===----------------------------------------------------------------------===//

include "llvm/Target/Target.td"

//===----------------------------------------------------------------------===//
// Register File, Calling Conv, Instruction Descriptions
//===----------------------------------------------------------------------===//

include "NkmmRegisterInfo.td"
include "NkmmSchedule.td"
include "NkmmInstrInfo.td"
include "NkmmCallingConv.td"

def NkmmInstrInfo : InstrInfo;

//===----------------------------------------------------------------------===//
// Nkmm supported processors.
//===----------------------------------------------------------------------===//
class Proc<string Name, list<SubtargetFeature> Features>
  : Processor<Name, NkmmGenericItineraries, Features>;

def : Proc<"generic", []>;

//===----------------------------------------------------------------------===//
// Target Declaration
//===----------------------------------------------------------------------===//

def Nkmm : Target {
  let InstructionSet = NkmmInstrInfo;
}
