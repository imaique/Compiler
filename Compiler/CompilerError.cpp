#include "CompilerError.h"

CompilerError::CompilerError(std::string text, int line_location) : text(text), line_location(line_location) {}

bool operator<(const CompilerError& e1, const CompilerError& e2)
{
	return e1.line_location < e2.line_location;
}