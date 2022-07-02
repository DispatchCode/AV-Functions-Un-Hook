#include "av_hook.h"
#include "pe_instrumentation.h"

int main()
{
	LPCSTR pstrFuncName = "NtReadVirtualMemory";
	printf("Function %s, raw offset: %X\n", pstrFuncName, locate_function(pstrFuncName));
	return 0;
}
