set(LLVM_TARGET_DEFINITIONS Nkmm.td)

tablegen(LLVM NkmmGenCallingConv.inc -gen-callingconv)
tablegen(LLVM NkmmGenInstrInfo.inc -gen-instr-info)
tablegen(LLVM NkmmGenMCPseudoLowering.inc -gen-pseudo-lowering)
tablegen(LLVM NkmmGenRegisterInfo.inc -gen-register-info)
tablegen(LLVM NkmmGenSubtargetInfo.inc -gen-subtarget)
add_public_tablegen_target(NkmmCommonTableGen)

add_llvm_target(NkmmCodeGen
  NkmmISelLowering.cpp
  NkmmInstrInfo.cpp
  NkmmRegisterInfo.cpp
  NkmmSubtarget.cpp
  NkmmTargetMachine.cpp
  )

add_subdirectory(TargetInfo)
add_subdirectory(MCTargetDesc)
