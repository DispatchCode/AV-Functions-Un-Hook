#include "av_hook.h"


int main()
{
    LPCSTR pstrFuncName = "NtReadVirtualMemory";
    LPCSTR pstrModuleName = "ntdll.dll";
    PINFO pInfo = GetFunctionFileOffset(pstrModuleName, pstrFuncName);

    if (pInfo)
    {
        printf("Function %s, raw offset: %X, Pointer to function code %p:\n", pstrFuncName, pInfo->dwOffset, pInfo->pInMemoryFunction);

        CompareInMemoryAndPhysicalBytes(pInfo, 8);

        free(pInfo);
    }
    else
    {
        printf("Errors locating function %s in module %s\n", pstrFuncName, pstrModuleName);
    }

    getchar();

    return 0;
}
