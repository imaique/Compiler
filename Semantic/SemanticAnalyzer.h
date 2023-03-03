#pragma once
#include "../SyntaxAnalyzer/Parser.h"
#include <stack>
#include <unordered_map>
#include <unordered_set>


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

class SemanticAnalyzer {
private:
	static const std::unordered_map<std::string, std::unordered_map<std::string, int>> m_trees;
	static const std::unordered_set<std::string> m_leaves;
	Parser* parser;
	void add_top(std::vector<AST*>& children);
	bool top_in_shape(std::unordered_map<std::string, int>& current_shape);
	void transform_tree(AST* root);
	AST* create_dot(std::vector<AST*>& children);
	void convert_dot_lists(AST* node);
public:
	std::stack<AST*> m_stack;
	AST* get_AST();
	SemanticAnalyzer(std::string filename);
	bool perform_semantic_action(std::string action, const Token& token);
};