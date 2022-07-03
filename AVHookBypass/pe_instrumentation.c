#include <stdio.h>
#include <windows.h>
#include "pe_instrumentation.h"


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

PINFO GetFileOffsetFunction(LPCBYTE pImageBase, PIMAGE_NT_HEADERS pNtHeader, DWORD dwRvaFunction)
{
    PINFO pInfo = calloc(1, sizeof(INFO));

    PIMAGE_SECTION_HEADER pFirstSection = IMAGE_FIRST_SECTION(pNtHeader);
    PBYTE pFunctions = (PBYTE)(pImageBase + dwRvaFunction);

    for (int j = 0; j < pNtHeader->FileHeader.NumberOfSections; j++)
    {
        LPCBYTE pSectionStart = pImageBase + pFirstSection[j].VirtualAddress;
        LPCBYTE pSectionEnd = pSectionStart + pFirstSection[j].Misc.VirtualSize;

        if (pSectionStart <= pFunctions && pFunctions < pSectionEnd)
        {
            pInfo->pInMemoryFunction = pFunctions;
            pInfo->dwOffset = pFirstSection[j].PointerToRawData + (dwRvaFunction - pFirstSection[j].VirtualAddress);
            return pInfo;
        }
    }

    return NULL;
}

PINFO GetFileOffset(LPCBYTE pImageBase, PIMAGE_NT_HEADERS pNtHeader, PIMAGE_EXPORT_DIRECTORY pExportDirectory, LPCSTR lpstrFuncName)
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

    return NULL;
}

PINFO GetFunctionFileOffset(LPCSTR lpstrModuleName, LPCSTR lpstrFuncName)
{
    HMODULE hModule = GetModuleHandle(lpstrModuleName);

    if (!hModule)
    {
        return NULL;
    }

    LPCBYTE pImageBase = (LPCBYTE)hModule;
    PIMAGE_NT_HEADERS pNtHeader = GetNtHeader(pImageBase, hModule);

    DWORD dwVirtualAddress = GetExportDataDirectoryVa(pNtHeader);
    if (!dwVirtualAddress)
    {
        return NULL;
    }

    PIMAGE_EXPORT_DIRECTORY pExportDirectory = (PIMAGE_EXPORT_DIRECTORY)(pImageBase + dwVirtualAddress);

    PINFO pInfo = GetFileOffset(pImageBase, pNtHeader, pExportDirectory, lpstrFuncName);

    if (!pInfo)
    {
        return NULL;
    }

    return pInfo;
}