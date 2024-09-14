#include<Windows.h>

typedef LSTATUS (WINAPI* RegCreateKeyExAFunctionPointer)(
    HKEY                        hKey,
    LPCSTR                      lpSubKey,
    DWORD                       Reserved,
    LPSTR                       lpClass,
    DWORD                       dwOptions,
    REGSAM                      samDesired,
    const LPSECURITY_ATTRIBUTES lpSecurityAttributes,
    PHKEY                       phkResult,
    LPDWORD                     lpdwDisposition
);

typedef LSTATUS(WINAPI* RegOpenKeyExAFunctionPointer)(
    HKEY   hKey,
    LPCSTR lpSubKey,
    DWORD  ulOptions,
    REGSAM samDesired,
    PHKEY  phkResult
);


#pragma once
class dependencies
{
public:
    RegCreateKeyExAFunctionPointer RegCreateKeyExA;
    RegOpenKeyExAFunctionPointer RegOpenKeyExA;

    void setup();
};

