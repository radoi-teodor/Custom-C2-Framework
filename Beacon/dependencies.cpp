#include <string>
#include <nlohmann/json.hpp>
#include "dependencies.h"
#include "Windows.h"

using json = nlohmann::json;

std::string dependencies::sanitize_identifier(std::string text) {
	std::string result = "";
	for (int i = 0; i < text.size(); i++) {
		if (isalnum(text[i])) {
			result += text[i];
		}
	}
	return result;
}

std::vector<std::string> dependencies::deserialize_array(std::string text) {
	json j = json::parse(text);
	std::vector<std::string> array;
	j.get_to(array);
	return array;
}