#include "SemanticAnalyzer.h"
#include <iostream>
#include <string>
#include <vector>
#include <sstream>


using std::string;
using std::getline;
using std::stringstream;
using std::vector;
using std::unordered_map;

namespace SemanticAnalyzerConstants {
	const string any = "";
	const string Id = "Id";
	const string Type = "Type";
	const string IntNum = "IntNum";
	const string FloatNum = "FloatNum";
	const string OpType = "OpType";

	const string FuncDef = "FuncDef";
	const string Scope = "Scope";
	const string ParamList = "ParamList";
	const string StatBlock = "StatBlock";
	const string AssignStat = "AssignStat";
	const string FuncCallStat = "FuncCallStat";
	const string VarDecl = "VarDecl";
	const string Variable = "Variable";
	const string Operation = "Operation";
	const string Dot = "Dot";
	const string FuncCall = "FuncCall";
	const string Dim = "Dim";
	const string DimList = "DimList";
	const string Statement = "Statement";
	const string AParamList = "AParamList";
}

using namespace SemanticAnalyzerConstants;
typedef SemanticAnalyzer SA;

AST::AST(std::string type, std::string value) : is_leaf(true), type(type), value(value) {

}

AST::AST(std::string type, std::vector<AST*> children) : is_leaf(false), type(type), children(children) {

}

const std::unordered_set <std::string> SA::m_leaves = {
	Id,
	Type,
	IntNum,
	FloatNum,
	OpType,
};
// -1 indicates infinitely many
const std::unordered_map<std::string, unordered_map<std::string, int>> SA::m_trees = {
	{FuncDef, {{Scope, 1}, {Id, 1}, {ParamList, 1}, {Type, 1}, {StatBlock, 1}, }},
	{Scope, {{Id, 1}, }},
	{ParamList, {{VarDecl, -1}, }},
	{StatBlock, {{Statement, -1},{AssignStat, -1}, {FuncCallStat, -1}, }},
	{AssignStat, {{any, 2}, }},
	{FuncCallStat, {{any, 1}, }},
	{VarDecl, {{Id, 1}, {Type, 1}, {DimList, 1}, }},
	{Variable, {{Id, 1}, {DimList, 1}, }},
	{Operation, {{any, 2}, {OpType, 1}, }},
	{Dot, {{FuncCall, -1}, {Variable, -1},}},
	{FuncCall, {{Id, 1}, {AParamList, 1},}},
	{Dim, {{IntNum, 1},}},
	{DimList, {{Dim, -1},}},
};

SemanticAnalyzer::SemanticAnalyzer(std::string filename) {
	parser = new Parser(filename, this);
}

bool SemanticAnalyzer::top_in_shape(std::unordered_map<std::string, int>& current_shape)
{
	if (m_stack.empty()) return false;
	string top_type = m_stack.top()->type;
	if (current_shape.count(top_type)) {
		std::cout << current_shape[top_type] << " " << top_type << std::endl;
		return current_shape[top_type]--;
	}
	return false;
}

AST* SemanticAnalyzer::get_AST() {
	parser->parse();
	if (m_stack.empty() || parser->error) return nullptr;
	AST* result = m_stack.top();
	m_stack.pop();
	return result;
}

string AST::get_value() {
	return value;
}


string AST::get_type() {
	return type;
}

// format s_<semantic_role>_<type>

bool SemanticAnalyzer::perform_semantic_action(std::string action, const Token& token) {
	if (action[0] != '_') return false;

	string trash;
	string type;

	stringstream ss(action);

	getline(ss, trash, '_');
	getline(ss, type, '_');

	vector<AST*> children;

	AST* new_node = nullptr;

	if (m_leaves.count(type)) {
		new_node = new AST(type, token.lexeme);
		m_stack.push(new_node);
	}
	else {
		unordered_map<std::string, int> current_shape = m_trees.at(type);

		int any = 0;
		if (current_shape.count("")) {
			any = current_shape.at("");
		}

		while (top_in_shape(current_shape)) {
			children.push_back(m_stack.top());
			m_stack.pop();
		}

		while (any > 0) {
			if(!top_in_shape(current_shape)) any--;;
			children.push_back(m_stack.top());
			m_stack.pop();
		}

		for (const auto& pair : current_shape) {
			// Push empty node if part of shape
			if (pair.first != "" && pair.second > 0) {
				children.push_back(new AST(pair.first, vector<AST*>()));
			}
		}



		new_node = new AST(type, children);
		m_stack.push(new_node);
	}

	
	return true;
}

void SA::add_top(vector<AST*>& children) {
	children.push_back(m_stack.top());
	m_stack.pop();
}



const unordered_map<string, void(SA::*)()> m_map{

};

