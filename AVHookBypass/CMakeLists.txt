cmake_minimum_required (VERSION 3.8)

add_executable (AVHookBypass "av_hook.c" "av_hook.h" "pe_instrumentation.c" "pe_instrumentation.h" "check_machine_code.c" 
"../Machine-Code-Analyzer/src/mca.c" "../Machine-Code-Analyzer/src/queue.c" "../Machine-Code-Analyzer/src/vector.c" "../Machine-Code-Analyzer/src/utils/function_length.c")

set_property(TARGET AVHookBypass PROPERTY C_STANDARD 11)