#pragma once

#include "pe_instrumentation.h"

BOOL CompareAndPrintInstructionsBytes(struct instruction* in_memory_bytes, struct instruction* on_disk_bytes, int dwNumberInstructions);
struct instruction* GetFileOnDiskBytes(PINFO pInfo, DWORD dwNumberInstructions);
struct instruction* GetInMemoryBytes(PINFO pInfo, DWORD dwNumberInstructions);
void UnhookFunction(PINFO pInfo, struct instruction* on_disk_bytes, int dwNumberInstructions);