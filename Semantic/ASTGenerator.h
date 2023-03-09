#pragma once
#include "../SyntaxAnalyzer/Parser.h"
#include "AST.h"
#include <stack>
#include <unordered_map>
#include <unordered_set>



class ASTGenerator {
private:
	
	Parser* parser;
	void add_top(std::vector<AST*>& children);
	bool top_in_shape(std::unordered_map<std::string, int>& current_shape);
	void transform_tree(AST* root);
	AST* create_dot(std::vector<AST*>& children);
	void convert_dot_lists(AST* node);
public:
	static const std::unordered_map<std::string, std::unordered_map<std::string, int>> m_trees;
	static const std::unordered_set<std::string> m_leaves;
	std::stack<AST*> m_stack;
	AST* get_AST();
	ASTGenerator(std::string filename);
	bool perform_semantic_action(std::string action, const Token& token);
};