#pragma once

#include <Windows.h>

typedef struct INFO {
	PBYTE pInMemoryFunction;
	DWORD dwOffset;
} INFO, *PINFO;

PINFO GetFunctionFileOffset(LPCSTR module_name, LPCSTR func_name);