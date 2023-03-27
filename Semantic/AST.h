#pragma once
#include "../LexicalAnalyzer/Token.h"
#include "Decorator.h"
#include <vector>

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
	Decorator decorator;
	int line_start;
};

class IdNode : public AST {
	std::string id;
};

class TypeNode : public AST {
	std::string type;
};

class IdNode : public AST {
	std::string id;
};