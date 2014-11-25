set(LLVM_TARGET_DEFINITIONS Nkmm.td)

tablegen(LLVM NkmmGenRegisterInfo.inc -gen-register-info)
tablegen(LLVM NkmmGenInstrInfo.inc -gen-instr-info)
tablegen(LLVM NkmmGenSubtargetInfo.inc -gen-subtarget)
tablegen(LLVM NkmmGenMCPseudoLowering.inc -gen-pseudo-lowering)
add_public_tablegen_target(NkmmCommonTableGen)

add_llvm_target(NkmmCodeGen
  NkmmInstrInfo.cpp
  NkmmRegisterInfo.cpp
  NkmmSubtarget.cpp
  NkmmTargetMachine.cpp
  )

add_subdirectory(TargetInfo)
add_subdirectory(MCTargetDesc)
