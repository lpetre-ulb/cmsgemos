include(CheckCXXCompilerFlag)

check_cxx_compiler_flag(-fdiagnostics-color=always GCC_DIAGNOSTICS_COLOR)
check_cxx_compiler_flag(-fcolor-diagnostics CLANG_DIAGNOSTICS_COLOR)

if (GCC_DIAGNOSTICS_COLOR)
   add_compile_options (-fdiagnostics-color=always)
elseif (CLANG_DIAGNOSTICS_COLOR)
   add_compile_options (-fcolor-diagnostics)
endif ()
