# AV Hook Bypass C

You can find more on this subject on the web, so nothing new.  The project is just a working example of a function un-hook, tested on a VM with Windows 10 x64 and BitDefender AV.

What makes AV Function Unhook different is the use of my own project, [MCA](https://github.com/DispatchCode/Machine-Code-Analyzer), an x86/x86-64 instruction disassembler.

_I released the entire code only for study pourposes._

You can see in the GIF below how it looks like when executed:
<p align="center">
  <img src="https://imgur.com/p0APck5.gif">
  _AV HookBypass execution_
</p>

## How it works?

It search the desired function into the Export Table and after found performs a conversion from VA to the file offset. The address at witch VA is pointint to, will be then overwritten by the machine code found on disk skipping the amount of bytes got in the previous step.

You can find an example looking at the screenshots below:

| Code with function hook | Code after un-hook |
| ----------- | ----------- |
| ![hooked_execution](https://user-images.githubusercontent.com/4256708/187036933-9145c8ba-45d0-4fdb-b440-94e9a94f0179.png) | ![unhooked_execution](https://user-images.githubusercontent.com/4256708/187036952-9c06861f-d6b6-4508-aae5-d149df061dee.png) |

The unhook process is performed by this function:

```C
void UnhookFunction(PINFO pInfo, struct instruction* on_disk_bytes, int dwNumberInstructions) 
{
    DWORD totalInstrLengths = 0;
    for (int i = 0; i < dwNumberInstructions; i++) 
    {
        totalInstrLengths += on_disk_bytes[i].length;
    }

    DWORD dwOldProtect;
    VirtualProtect(pInfo->pInMemoryFunction, totalInstrLengths, PAGE_EXECUTE_READWRITE, &dwOldProtect);

    DWORD dwLenPreviousIntr = 0;
    for (int i = 0; i < dwNumberInstructions; i++) 
    {
        memcpy((pInfo->pInMemoryFunction + dwLenPreviousIntr), on_disk_bytes[i].instr, on_disk_bytes[i].length);
        dwLenPreviousIntr += on_disk_bytes[i].length;
    }

    VirtualProtect(pInfo->pInMemoryFunction, totalInstrLengths, dwOldProtect, &dwOldProtect);
}
```

Because MCA decode the instruction length (and its fields), you can specify the number of instructions from the function entry point.

`av_hook.c` is the file (program, in a real case scenario) that set all that things up: call disasm, store references to memory, parse PE header & get the code, and patch the function. Furthermore compare memory before and after the unhook (its code is just to do the unhook on a simple "program").

![output_hooked_system](https://user-images.githubusercontent.com/4256708/187037502-e8a33737-228a-41a9-8f48-32737e08462d.png)