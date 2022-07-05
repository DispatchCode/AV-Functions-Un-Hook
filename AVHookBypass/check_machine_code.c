#include "av_hook.h"

extern pFunctionInfo getFunctionLength(char*, enum eFunctionType, bool, bool);


size_t GetSize(FILE* hfile)
{
    fseek(hfile, 0, SEEK_END);
    size_t file_size = (size_t)ftell(hfile);
    fseek(hfile, 0, SEEK_SET);
    return file_size;
}

function_code* MemoryDisassembly(char* in_buffer, size_t size) {
    function_code* fc = calloc(1, sizeof(function_code));

    fc->code_buffer = calloc(size, sizeof(uint8_t));

    int offset = 0;
    int len = size;
    while (len)
    {
        struct instruction instr;
        mca_decode(&instr, 2, in_buffer, offset);

        memcpy(fc->code_buffer + offset, instr.instr, instr.length);

        len -= instr.length;
        offset += instr.length;
    }

    fc->code_size = size;
    
    return fc;
}

function_code* GetInMemoryBytes(PINFO pInfo)
{
    pFunctionInfo pFunctionInfo = getFunctionLength((char*)pInfo->pInMemoryFunction, 2, true, true);

    printf("Function length: %d\n", pFunctionInfo->length);

    return MemoryDisassembly((char*)pInfo->pInMemoryFunction, pFunctionInfo->length);
}

function_code* GetFileOnDiskBytes(PINFO pInfo)
{

    LPCSTR path_ntdll32 = "C:\\Windows\\SysWOW64\\ntdll.dll";
    LPCSTR path_ntdll64 = "C:\\Windows\\System32\\ntdll.dll";
    
    LPCSTR ntdll_file_path = pInfo->is32Bit ? path_ntdll32 : path_ntdll64;

    FILE *hFile = fopen(ntdll_file_path, "rb");
    
    size_t file_size = GetSize(hFile);
    uint8_t* file_on_disk_bytes = calloc(file_size, sizeof(uint8_t));

    fread(file_on_disk_bytes, sizeof(uint8_t), file_size, hFile);
    fclose(hFile);
    
    pFunctionInfo pFunctionInfo = getFunctionLength((char*)(file_on_disk_bytes + pInfo->dwOffset), 2, true, true);

    uint8_t *out_buffer = MemoryDisassembly((char*)(file_on_disk_bytes + pInfo->dwOffset), pFunctionInfo->length);

    free(file_on_disk_bytes);

    return out_buffer;
}

void CompareInMemoryAndPhysicalBytes(PINFO pInfo)
{
    function_code* in_memory_bytes = GetInMemoryBytes(pInfo);
    function_code* on_disk_bytes = GetFileOnDiskBytes(pInfo);

    printf("Bytes in memory: ");
    for (int i = 0; i < in_memory_bytes->code_size; i++) {
        printf("%X ", in_memory_bytes->code_buffer[i]);
    }
    printf("\n");

    printf("Bytes on disk: ");
    for (int i = 0; i < on_disk_bytes->code_size; i++) {
        printf("%X ", on_disk_bytes->code_buffer[i]);
    }
    printf("\n");
}
