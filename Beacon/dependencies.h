#pragma once
#include <string>
#include <vector>
#include "dependencies.h"

class dependencies
{
public:
	static std::string sanitize_identifier(std::string text);

	static std::vector<std::string> deserialize_array(std::string text);

};

