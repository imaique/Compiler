#pragma once
#include "../SyntaxAnalyzer/Parser.h"
#include <stack>

class AST {
public:
	std::string get_type();
	std::string get_value();
	AST(std::string type, std::string value);
	AST(std::string type, std::vector<AST*> children);

//private:
	std::string type;
	bool is_leaf;
	std::string value;
	std::vector<AST*> children;
};

class SemanticAnalyzer {
private:
	std::stack<AST*> m_stack;
	Parser* parser;
public:
	AST* get_AST();
	SemanticAnalyzer(std::string filename);
	bool perform_semantic_action(std::string action, const Token& token);
};