#include "ASTGenerator.h"
#include <iostream>
#include <string>
#include <vector>
#include <sstream>


using std::string;
using std::getline;
using std::stringstream;
using std::vector;
using std::unordered_map;

namespace ASTConstants {
	const string any = "";
	const string Id = "Id";
	const string Type = "Type";
	const string IntNum = "IntNum";
	const string FloatNum = "FloatNum";
	const string OpType = "OpType";

	const string FuncDef = "FuncDef";
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

using namespace ASTConstants;
typedef ASTGenerator SA;

AST::AST(std::string type, std::string value, Token t) : is_leaf(true), type(type), value(value) {
	token = new Token(t);
}

AST::AST(std::string type, std::vector<AST*> children) : is_leaf(false), type(type), children(children), token(nullptr) {

}

const std::unordered_set <std::string> SA::m_leaves = {
	Id,
	Type,
	IntNum,
	FloatNum,
	OpType,
	StatType,
	Sign,
	Visibility,
};
// -1 indicates infinitely many
const std::unordered_map<std::string, unordered_map<std::string, int>> SA::m_trees = {
	{FuncDef, {{Scope, 1}, {Id, 1}, {ParamList, 1}, {Type, 1}, {StatBlock, 1}, }},
	{Scope, {{Id, 1}, }},
	{ParamList, {{VarDecl, -1}, }},
	{StatBlock, {{any, -1}, }},
	{Statement, {{any, -1}, }},
	{AssignStat, {{any, 2}, }},
	{FuncCallStat, {{any, 1}, }},
	{VarDecl, {{any, -1}, }},
	{Variable, {{Id, 1}, {DimList, 1}, }},
	{Operation, {{any, 2}, {OpType, 1}, }},
	{DotList, {{FuncCall, -1}, {Variable, -1},}},
	{FuncCall, {{Id, 1}, {AParamList, 1},}},
	{DimList, {{Dim, -1},}},
	{AParamList, {{any, -1}, }}
};

ASTGenerator::ASTGenerator(std::string filename) {
	parser = new Parser(filename, this);
}

bool ASTGenerator::top_in_shape(std::unordered_map<std::string, int>& current_shape)
{
	if (m_stack.empty() || !m_stack.top()) return false;
	string top_type = m_stack.top()->type;
	if (current_shape.count(top_type)) {
		std::cout << current_shape[top_type] << " " << top_type << std::endl;
		return current_shape[top_type]--;
	}
	return false;
}

AST* ASTGenerator::get_AST() {
	parser->parse();
	if (m_stack.empty() || parser->error) return nullptr;
	AST* root = m_stack.top();
	transform_tree(root);
	m_stack.pop();
	return root;
}

string AST::get_value() {
	return value;
}


string AST::get_type() {
	return type;
}

// format s_<semantic_role>_<type>

bool ASTGenerator::perform_semantic_action(std::string action, const Token& token) {
	if (action[0] != '_') return false;

	string trash;
	string type;

	stringstream ss(action);

	getline(ss, trash, '_');
	getline(ss, type, '_');

	vector<AST*> children;

	AST* new_node = nullptr;
	if (type == "stop") {
		m_stack.push(nullptr);
	}
	else if (m_leaves.count(type)) {
		new_node = new AST(type, token.lexeme, token);
		m_stack.push(new_node);
	}
	else {
		
		unordered_map<std::string, int> current_shape = unordered_map<std::string, int>();
		int any = -1;
		if (m_trees.count(type)) {
			current_shape = m_trees.at(type);
			any = 0;
			if (current_shape.count("")) {
				any = current_shape.at("");
			}
		}

		while (top_in_shape(current_shape)) {
			children.push_back(m_stack.top());
			m_stack.pop();
		}

		while (any && m_stack.top()) {
			if(!top_in_shape(current_shape)) any--;;
			children.push_back(m_stack.top());
			m_stack.pop();
		}

		if (any && !m_stack.empty() && !m_stack.top()) m_stack.pop();

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

void ASTGenerator::add_top(vector<AST*>& children) {
	children.push_back(m_stack.top());
	m_stack.pop();
}

void ASTGenerator::transform_tree(AST* root) {
	convert_dot_lists(root);
}
void ASTGenerator::convert_dot_lists(AST* node) {
	vector<AST*> children = node->children;
	if (node->get_type() == "DotList") {
		int child_nb = children.size();
		if (child_nb == 1) {
			*node = *(children[0]);
		}
		else if (child_nb == 2) {
			node->type = "Dot";
		}
		else {
			AST* new_node = create_dot(children);
			*node = *(new_node);
		}
	}

	for (AST* child : children)
		convert_dot_lists(child);
}

AST* ASTGenerator::create_dot(vector<AST*>& children) {
	if (children.size() == 2) {
		return new AST("Dot", children);
	}
	else {
		AST* last_child = children.back();
		children.pop_back();
		AST* first_child = create_dot(children);
		return new AST("Dot", {first_child, last_child});
	}
}

