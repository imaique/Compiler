#pragma once

#include <string>

class CompilerError {
public:
	int line_location;
	std::string text;

	CompilerError(std::string text, int line_location);
	friend bool operator<(const CompilerError& p1, const CompilerError& p2);
};