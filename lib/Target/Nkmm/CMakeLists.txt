set(LLVM_TARGET_DEFINITIONS Nkmm.td)

tablegen(LLVM NkmmGenAsmWriter.inc -gen-asm-writer)
tablegen(LLVM NkmmGenCallingConv.inc -gen-callingconv)
tablegen(LLVM NkmmGenDAGISel.inc -gen-dag-isel)
tablegen(LLVM NkmmGenInstrInfo.inc -gen-instr-info)
tablegen(LLVM NkmmGenMCPseudoLowering.inc -gen-pseudo-lowering)
tablegen(LLVM NkmmGenRegisterInfo.inc -gen-register-info)
tablegen(LLVM NkmmGenSubtargetInfo.inc -gen-subtarget)
add_public_tablegen_target(NkmmCommonTableGen)

add_llvm_target(NkmmCodeGen
  NkmmAsmPrinter.cpp
  NkmmFrameLowering.cpp
  NkmmISelDAGToDAG.cpp
  NkmmISelLowering.cpp
  NkmmInstrInfo.cpp
  NkmmMCInstLower.cpp
  NkmmMachineFunction.cpp
  NkmmRegisterInfo.cpp
  NkmmSubtarget.cpp
  NkmmTargetMachine.cpp
  NkmmTargetObjectFile.cpp
  )

add_subdirectory(InstPrinter)
add_subdirectory(MCTargetDesc)
add_subdirectory(TargetInfo)
