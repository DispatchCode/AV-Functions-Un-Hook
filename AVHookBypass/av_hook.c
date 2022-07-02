#include "av_hook.h"
#include "pe_instrumentation.h"

int main()
{
	LPCSTR pstrFuncName = "NtReadVirtualMemory";
	LPCSTR pstrModuleName = "ntdll.dll";
	printf("Function %s, raw offset: %X\n", pstrFuncName, GetFunctionFileOffset(pstrModuleName, pstrFuncName));
	return 0;
}
