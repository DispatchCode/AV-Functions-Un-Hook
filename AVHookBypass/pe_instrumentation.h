#pragma once

#include <Windows.h>

typedef struct INFO {
    PBYTE pInMemoryFunction;
    DWORD dwOffset;
    BOOL is32Bit;
} INFO, * PINFO;

PINFO GetFunctionFileOffset(LPCSTR module_name, LPCSTR func_name);