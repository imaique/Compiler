#pragma once
#include <vector>
#include <string>

#include "CompilerError.h"

class Compiler
{
	static std::vector<CompilerError> all_errors;
	static void reset_logger();
	static void print_errors(std::string filename);
public:
	static void add_error(CompilerError error);
	void compile(std::string filename);
};

