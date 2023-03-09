#pragma once
#include "../LexicalAnalyzer/Token.h"

class AST {
public:
	std::string get_type();
	std::string get_value();
	AST(std::string type, std::string value, Token token);
	AST(std::string type, std::vector<AST*> children);

	//private:
	std::string type;
	Token* token;
	bool is_leaf;
	std::string value;
	std::vector<AST*> children;
};