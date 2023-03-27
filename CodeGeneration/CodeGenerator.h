#pragma once

#include "../Semantic/AST.h"
class CodeGenerator {
private:
	std::ofstream moon_file;
public:
	CodeGenerator(std::string filename);
	void generate(AST* root, SymbolTable* global_table);
};