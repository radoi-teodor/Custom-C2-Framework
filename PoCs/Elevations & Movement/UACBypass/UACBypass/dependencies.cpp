#include "dependencies.h"

void dependencies::setup() {

	HMODULE hModule = LoadLibraryA("winreg.dll");
	RegCreateKeyExA = (RegCreateKeyExAFunctionPointer)GetProcAddress(hModule, "RegCreateKeyExA");
	RegOpenKeyExA = (RegOpenKeyExAFunctionPointer)GetProcAddress(hModule, "RegOpenKeyExA");


}