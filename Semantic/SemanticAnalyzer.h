#pragma once
#include <string>
#include "AST.h"

class SemanticAnalyzer {
private:
	std::string filename;
	void construct_symbol_tables(AST* node);
public:
	SemanticAnalyzer(std::string filename);
	bool analyze();
};