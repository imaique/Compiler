#pragma once
#include "../SyntaxAnalyzer/Parser.h"
#include "AST.h"
#include <stack>
#include <unordered_map>
#include <unordered_set>

namespace ASTConstants {
	const string any = "";
	const string Id = "Id";
	const string Type = "Type";
	const string IntNum = "IntNum";
	const string FloatNum = "FloatNum";
	const string OpType = "OpType";

	const string ISA = "ISA";
	const string Class = "Class";
	const string FuncDef = "FuncDef";
	const string FuncDecl = "FuncDecl";
	const string Scope = "Scope";
	const string ParamList = "ParamList";
	const string AParamList = "AParamList";
	const string StatBlock = "StatBlock";
	const string AssignStat = "AssignStat";
	const string FuncCallStat = "FuncCallStat";
	const string VarDecl = "VarDecl";
	const string Variable = "Variable";
	const string Operation = "Operation";
	const string DotList = "DotList";
	const string FuncCall = "FuncCall";
	const string Dim = "Dim";
	const string DimList = "DimList";
	const string Statement = "Statement";
	const string StatType = "StatType";
	const string Sign = "Sign";
	const string Visibility = "Visibility";
}

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