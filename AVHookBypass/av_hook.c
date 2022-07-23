#include "av_hook.h"


int main()
{
    LPCSTR pstrFuncName = "NtReadVirtualMemory";
    LPCSTR pstrModuleName = "ntdll.dll";
    PINFO pInfo = GetFunctionFileOffset(pstrModuleName, pstrFuncName);

    DWORD dwNumberInstructions = 8;
    
    if (pInfo)
    {
        printf("Function %s, raw offset: %X, Pointer to function code %p:\n", pstrFuncName, pInfo->dwOffset, pInfo->pInMemoryFunction);

        struct instruction* in_memory_bytes = GetInMemoryBytes(pInfo, dwNumberInstructions);
        struct instruction* on_disk_bytes = GetFileOnDiskBytes(pInfo, dwNumberInstructions);

        if (! CompareAndPrintInstructionsBytes(in_memory_bytes, on_disk_bytes, dwNumberInstructions)) {
            UnhookFunction(pInfo, on_disk_bytes, dwNumberInstructions);
        }
        
        pInfo->pInMemoryFunction = NULL;
        free(pInfo);
        free(in_memory_bytes);
        free(on_disk_bytes);
        pInfo = NULL;

        printf("\n\nPRINT COMPARE AFTER UNKOOK\n");
        pInfo = GetFunctionFileOffset(pstrModuleName, pstrFuncName);

        in_memory_bytes = GetInMemoryBytes(pInfo, dwNumberInstructions);
        on_disk_bytes = GetFileOnDiskBytes(pInfo, dwNumberInstructions);
        
        if (CompareAndPrintInstructionsBytes(in_memory_bytes, on_disk_bytes, dwNumberInstructions)) {
            printf("\nInstrutions are equals.\n");
        }

        free(pInfo);
    }
    else
    {
        printf("Errors locating function %s in module %s\n", pstrFuncName, pstrModuleName);
    }

    getchar();

    return 0;
}
