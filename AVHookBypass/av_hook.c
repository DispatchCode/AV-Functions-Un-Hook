#include "av_hook.h"
#include "pe_instrumentation.h"

int main()
{
	LPCSTR pstrFuncName = "NtReadVirtualMemory";
	LPCSTR pstrModuleName = "ntdll.dll";
	PINFO pInfo = GetFunctionFileOffset(pstrModuleName, pstrFuncName);
	
	if (pInfo) 
	{
		printf("Function %s, raw offset: %X, Pointer to function code %p:\n", pstrFuncName, pInfo->dwOffset, pInfo->pInMemoryFunction);
		printf("Reading 4-bytes from function code: %X %X %X %X\n", *pInfo->pInMemoryFunction, *(pInfo->pInMemoryFunction + 1), *(pInfo->pInMemoryFunction + 2), *(pInfo->pInMemoryFunction + 3));

		free(pInfo);
	}
	else 
	{
		printf("Errors locating function %s in module %s\n", pstrFuncName, pstrModuleName);
	}

	getchar();
	
	return 0;
}
