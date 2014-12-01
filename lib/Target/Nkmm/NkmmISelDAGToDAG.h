//===---- NkmmISelDAGToDAG.h - A Dag to Dag Inst Selector for Nkmm --------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines an instruction selector for the NKMM target.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_NKMM_NKMMISELDAGTODAG_H
#define LLVM_LIB_TARGET_NKMM_NKMMISELDAGTODAG_H

namespace llvm {

class FunctionPass;

/// createNkmmISelDag - This pass converts a legalized DAG into a
/// NKMM-specific DAG, ready for instruction scheduling.
FunctionPass *createNkmmISelDag(NkmmTargetMachine &TM);
}

#endif
