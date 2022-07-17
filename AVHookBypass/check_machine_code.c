#include "av_hook.h"


size_t GetSize(FILE* hfile)
{
    fseek(hfile, 0, SEEK_END);
    size_t file_size = (size_t)ftell(hfile);
    fseek(hfile, 0, SEEK_SET);
    return file_size;
}

struct instruction* MemoryDisassembly(char* in_buffer, DWORD dwNumberInstructions)
{
    struct instruction *instr = calloc(dwNumberInstructions, sizeof(struct instruction));

    int offset = 0;
    for(int i=0; i<dwNumberInstructions; i++)
    {
        mca_decode(instr + i, 2, in_buffer, offset);
        offset += instr[i].length;
    }

    return instr;
}

struct instruction* GetInMemoryBytes(PINFO pInfo, DWORD dwNumberInstructions)
{
    return MemoryDisassembly((char*)pInfo->pInMemoryFunction, dwNumberInstructions);
}

struct instruction* GetFileOnDiskBytes(PINFO pInfo, DWORD dwNumberInstructions)
{

    LPCSTR path_ntdll32 = "C:\\Windows\\SysWOW64\\ntdll.dll";
    LPCSTR path_ntdll64 = "C:\\Windows\\System32\\ntdll.dll";
    
    LPCSTR ntdll_file_path = pInfo->is32Bit ? path_ntdll32 : path_ntdll64;

    FILE *hFile = fopen(ntdll_file_path, "rb");
    
    size_t file_size = GetSize(hFile);
    uint8_t* file_on_disk_bytes = calloc(file_size, sizeof(uint8_t));

    fread(file_on_disk_bytes, sizeof(uint8_t), file_size, hFile);
    fclose(hFile);
    
    struct instruction* instr = MemoryDisassembly((char*)(file_on_disk_bytes + pInfo->dwOffset), dwNumberInstructions);

    free(file_on_disk_bytes);

    return instr;
}

void CompareInMemoryAndPhysicalBytes(PINFO pInfo, int dwNumberInstructions)
{
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    SetConsoleMode(hOut, dwMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);

    struct instruction* in_memory_bytes = GetInMemoryBytes(pInfo, dwNumberInstructions);
    struct instruction* on_disk_bytes = GetFileOnDiskBytes(pInfo, dwNumberInstructions);

    printf("Comparing %d instructions.\nOutput will show \"in memory\" bytes; a red output means that bytes are not equals and probably the function is hooked.\n", dwNumberInstructions);
    for(int i=0; i<dwNumberInstructions; i++)
    {
        printf("\nInstruction #%d\n", i + 1);
        for(int j=0; j<in_memory_bytes[i].length; j++)
        {
            if(in_memory_bytes[i].instr[j] != on_disk_bytes[i].instr[j])
            {
                printf("\x1b[31m%02X \x1b[0m", in_memory_bytes[i].instr[j]);
            }
            else
            {
                printf("\x1b[32m%02X \x1b[0m", in_memory_bytes[i].instr[j]);
            }
        }
    }
}
