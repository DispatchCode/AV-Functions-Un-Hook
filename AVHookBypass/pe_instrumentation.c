#include <stdio.h>
#include <windows.h>


PIMAGE_NT_HEADERS GetNtHeader(LPCBYTE pImageBase, HMODULE hModule)
{
	return (PIMAGE_NT_HEADERS)(pImageBase + ((PIMAGE_DOS_HEADER)hModule)->e_lfanew);
}

DWORD GetExportDataDirectoryVa(PIMAGE_NT_HEADERS pNtHeader)
{
	PIMAGE_DATA_DIRECTORY pExportDataDirectory = NULL;
	switch (pNtHeader->FileHeader.Machine)
	{
	case IMAGE_FILE_MACHINE_I386:
		pExportDataDirectory = &((PIMAGE_NT_HEADERS32)pNtHeader)->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];
		break;
	case IMAGE_FILE_MACHINE_AMD64:
		pExportDataDirectory = &((PIMAGE_NT_HEADERS64)pNtHeader)->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];
		break;
	default: break;
	}

	if (!pExportDataDirectory)
	{
		return 0;
	}

	return pExportDataDirectory->VirtualAddress;
}

DWORD GetFileOffsetFunction(LPCBYTE pImageBase, PIMAGE_NT_HEADERS pNtHeader, DWORD dwRvaFunction)
{
	PIMAGE_SECTION_HEADER pFirstSection = IMAGE_FIRST_SECTION(pNtHeader);
	PBYTE pFunctions = (PBYTE)(pImageBase + dwRvaFunction);

	for (int j = 0; j < pNtHeader->FileHeader.NumberOfSections; j++)
	{
		LPCBYTE pSectionStart = pImageBase + pFirstSection[j].VirtualAddress;
		LPCBYTE pSectionEnd = pSectionStart + pFirstSection[j].Misc.VirtualSize;

		if (pSectionStart <= pFunctions && pFunctions < pSectionEnd)
		{
			return pFirstSection[j].PointerToRawData + (dwRvaFunction - pFirstSection[j].VirtualAddress);

		}
	}

	return 0;
}

DWORD GetFileOffset(LPCBYTE pImageBase, PIMAGE_NT_HEADERS pNtHeader, PIMAGE_EXPORT_DIRECTORY pExportDirectory, LPCSTR lpstrFuncName)
{
	PDWORD pAddressOfFunctions = (PDWORD)(pImageBase + pExportDirectory->AddressOfFunctions),
		pAddressOfNames = (PDWORD)(pImageBase + pExportDirectory->AddressOfNames);

	PWORD pAddressOfNameOrdinals = (PWORD)(pImageBase + pExportDirectory->AddressOfNameOrdinals);

	size_t strLen = strlen(lpstrFuncName);

	for (DWORD i = 0; i < pExportDirectory->NumberOfNames; i++)
	{
		DWORD dwNameRVA = pAddressOfNames[i];
		DWORD dwRvaFunction = pAddressOfFunctions[pAddressOfNameOrdinals[i]];

		if (dwRvaFunction > 0 && dwNameRVA > 0 && strncmp((char*)(pImageBase + dwNameRVA), lpstrFuncName, strLen + 1) == 0)
		{
			return GetFileOffsetFunction(pImageBase, pNtHeader, dwRvaFunction);
		}
	}

	return 0;
}

INT GetFunctionFileOffset(LPCSTR lpstrModuleName, LPCSTR lpstrFuncName) 
{
	HMODULE hModule = GetModuleHandle(lpstrModuleName);
	
	if(! hModule) 
	{
		return -1;
	}

	LPCBYTE pImageBase = (LPCBYTE)hModule;
	PIMAGE_NT_HEADERS pNtHeader = GetNtHeader(pImageBase, hModule);
	
	DWORD dwVirtualAddress = GetExportDataDirectoryVa(pNtHeader);
	if(! dwVirtualAddress) 
	{
		return -2;
	}

	PIMAGE_EXPORT_DIRECTORY pExportDirectory = (PIMAGE_EXPORT_DIRECTORY) (pImageBase + dwVirtualAddress);

	DWORD dwOffset = GetFileOffset(pImageBase, pNtHeader, pExportDirectory, lpstrFuncName);

	if (!dwOffset) 
	{
		return -3;
	}

	return dwOffset;
}
