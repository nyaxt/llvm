//===-- NkmmMCAsmInfo.cpp - Nkmm Asm Properties ---------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the declarations of the NkmmMCAsmInfo properties.
//
//===----------------------------------------------------------------------===//

#include "NkmmMCAsmInfo.h"
#include "llvm/ADT/Triple.h"

using namespace llvm;

void NkmmMCAsmInfo::anchor() { }

NkmmMCAsmInfo::NkmmMCAsmInfo(StringRef) {
  PointerSize = CalleeSaveStackSlotSize = 4;
  AlignmentIsInBytes          = false;
  Data16bitsDirective         = "\t.2byte\t";
  Data32bitsDirective         = "\t.4byte\t";
  Data64bitsDirective         = "\t.8byte\t";
  PrivateGlobalPrefix         = ".L";
  CommentString               = "#";
  // ZeroDirective               = "\t.space\t";
  // GPRel32Directive            = "\t.gpword\t";
  // GPRel64Directive            = "\t.gpdword\t";
  // UseAssignmentForEHBegin = true;
  // SupportsDebugInformation = true;
  // ExceptionsType = ExceptionHandling::DwarfCFI;
  // DwarfRegNumForCFI = true;
}
