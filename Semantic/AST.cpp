#include "AST.h"

using std::string;

string AST::get_value() {
	return value;
}


string AST::get_type() {
	return type;
}

AST::AST(std::string type, std::string value, Token t) : is_leaf(true), type(type), value(value), line_start(-1) {
	token = new Token(t);
}

AST::AST(std::string type, std::vector<AST*> children) : is_leaf(false), type(type), children(children), token(nullptr), line_start(-1) {

}