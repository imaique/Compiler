#include "SemanticAnalyzer.h"
#include <iostream>
#include <string>
#include <vector>
#include <sstream>

using std::string;
using std::getline;
using std::stringstream;
using std::vector;

AST::AST(std::string type, std::string value) : is_leaf(true), type(type), value(value) {

}

AST::AST(std::string type, std::vector<AST*> children) : is_leaf(false), type(type), children(children) {

}

SemanticAnalyzer::SemanticAnalyzer(std::string filename) {
	parser = new Parser(filename, this);
}

AST* SemanticAnalyzer::get_AST() {
	parser->parse();
	if (m_stack.empty()) return nullptr;
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
	if (action[1] != '_') return false;

	string trash;
	string leaf;
	string type;
	

	stringstream ss(action);

	getline(ss, trash, '_');
	getline(ss, leaf, '_');
	getline(ss, type, '_');

	AST* new_node = nullptr;

	if (leaf == "stop") {
		// push nullptr automatically from above
	}
	else if (leaf == "leaf") {
		new_node = new AST(type, token.lexeme);
	}
	else {
		vector<AST*> children;
		while (m_stack.top() != nullptr) {
			children.push_back(m_stack.top());
			m_stack.pop();
		}
		m_stack.pop();
		new_node = new AST(type, children);
	}

	m_stack.push(new_node);
}

