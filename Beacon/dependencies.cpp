#include "Windows.h"
#include <string>
#include "dependencies.h"

std::string dependencies::sanitize_identifier(std::string text) {
	std::string result = "";
	for (int i = 0; i < text.size(); i++) {
		if (isalnum(text[i])) {
			result += text[i];
		}
	}
	return result;
}