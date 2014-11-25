//===-- NkmmSchedule.td - Nkmm Scheduling Definitions ------*- tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// Functional units across Nkmm chips sets.
//===----------------------------------------------------------------------===//
def ALU : FuncUnit;

//===----------------------------------------------------------------------===//
// Instruction Itinerary classes used for Nkmm
//===----------------------------------------------------------------------===//
def IIAlu    : InstrItinClass;
def IIBranch : InstrItinClass;
def IIPseudo : InstrItinClass;

//===----------------------------------------------------------------------===//
// Nkmm Generic instruction itineraries.
//===----------------------------------------------------------------------===//
def NkmmGenericItineraries : ProcessorItineraries<[ALU], [], [
	InstrItinData<IIAlu,    [InstrStage<1, [ALU]>]>,
	InstrItinData<IIBranch, [InstrStage<1, [ALU]>]>,
	InstrItinData<IIPseudo, [InstrStage<1, [ALU]>]>
]>;
