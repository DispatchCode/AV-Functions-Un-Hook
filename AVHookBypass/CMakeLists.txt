cmake_minimum_required (VERSION 3.8)

add_executable (AVHookBypass "av_hook.c" "av_hook.h" "pe_instrumentation.c" "pe_instrumentation.h" "check_machine_code.c" 
"../MCA/src/mca.c" "../MCA/src/queue.c" "../MCA/src/vector.c" "../MCA/src/utils/function_length.c")

set_property(TARGET AVHookBypass PROPERTY C_STANDARD 11)