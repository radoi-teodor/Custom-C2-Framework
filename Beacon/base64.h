#pragma once
#include <Windows.h>
#include <vector>
#include <string>

class base64
{
public:
	static std::string encode(BYTE const* buf, unsigned int bufLen);
	static std::vector<BYTE> decode(std::string const&);
};

