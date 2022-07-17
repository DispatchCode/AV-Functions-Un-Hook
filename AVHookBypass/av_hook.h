#pragma once

#include <stdio.h>
#include <stdint.h>

#include "check_machine_code.h"
#include "pe_instrumentation.h"
#include "../Machine-Code-Analyzer/src/mca.h"

typedef struct _function_code {
    uint8_t *code_buffer;
    size_t code_size;
} function_code;