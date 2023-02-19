#include "Token.h"
#include <string>
#include <iostream>

typedef Token::Type T;

Token::Token(std::string lexeme, int line_location, Type token_type) 
	: lexeme(lexeme), line_location(line_location), token_type(token_type), is_error(is_error_token_type(token_type)) {}

std::string Token::get_string(Type type) { return token_type_to_string.at(type); }

T Token::get_reserved_type(std::string lexeme) {
	return reserved_word_to_token_type.at(lexeme);
}

bool Token::is_reserved_word(std::string lexeme) {
	return reserved_word_to_token_type.find(lexeme) != reserved_word_to_token_type.end();
}

std::string Token::get_error_string(Type type) { return error_type_to_string.at(type); }

bool Token::is_error_token_type(T type) {
	return error_type_to_string.find(type) != error_type_to_string.end();
}

const std::unordered_map<Token::Type, std::string> Token::error_type_to_string = {
	{T::InvalidNumber, "Invalid number"},
	{T::InvalidCharacter, "Invalid character"},
	{T::UnclosedBlockComment, "Unterminated block comment"},
};

const std::unordered_map<Token::Type, std::string> Token::token_type_to_string = {
		{T::IntegerNumber, "intlit"},
		{T::ID, "id"},
		{T::FloatNumber, "floatlit"},
		{T::InvalidNumber, "invalidnum"},
		{T::Equal, "eq"},
		{T::NotEqual, "neq"},
		{T::GreaterThan, "gt"},
		{T::LessThan, "lt"},
		{T::GreaterOrEqualThan, "geq"},
		{T::LessOrEqualThan, "leq"},
		{T::Plus, "plus"},
		{T::Minus, "minus"},
		{T::Multiply, "mult"},
		{T::Divide, "div"},
		{T::Assign, "assign"},
		{T::OpenParenthesis, "lpar"},
		{T::ClosedParenthesis, "lpar"},
		{T::OpenCurlyBracket, "lcubr"},
		{T::ClosedCurlyBracket, "rcubr"},
		{T::OpenSquareBracket, "lsqbr"},
		{T::ClosedSquareBracket, "rsqbr"},
		{T::SemiColon, "semi"},
		{T::Comma, "comma"},
		{T::Point, "dot"},
		{T::Colon, "colon"},
		{T::ReturnType, "returntype"},
		{T::ScopeOperator, "scopeop"},
		{T::OR, "or"},
		{T::AND, "and"},
		{T::NOT, "not"},
		{T::Integer, "integer"},
		{T::Float, "float"},
		{T::Void, "void"},
		{T::Class, "class"},
		{T::Self, "self"},
		{T::Isa, "isa"},
		{T::While, "while"},
		{T::If, "if"},
		{T::Then, "then"},
		{T::Else, "else"},
		{T::Read, "read"},
		{T::Write, "write"},
		{T::Return, "return"},
		{T::LocalVar, "localvar"},
		{T::Constructor, "constructor"},
		{T::Attribute, "attribute"},
		{T::Function, "function"},
		{T::Public, "public"},
		{T::Private, "private"},
		{T::LineComment, "inlinecmt"},
		{T::BlockComment, "blockcmt"},
		{T::UnclosedBlockComment, "lblockcmt"},
		{T::InvalidCharacter, "invalidchar"},
		{Type::OR, "or"},
		{Type::AND, "and"},
		{Type::NOT, "not"},
		{Type::Integer, "integer"},
		{Type::Float, "float"},
		{Type::Void, "void"},
		{Type::Class, "class"},
		{Type::Self, "self"},
		{Type::Isa, "isa"},
		{Type::While, "while"},
		{Type::If, "if"},
		{Type::Then, "then"},
		{Type::Else, "else"},
		{Type::Read, "read"},
		{Type::Write, "write"},
		{Type::Return, "return"},
		{Type::LocalVar, "localvar"},
		{Type::Constructor, "constructor"},
		{Type::Attribute, "attribute"},
		{Type::Function, "function"},
		{Type::Public, "public"},
		{Type::Private, "private"},
		{T::EndOfFile, "$"},
};

const std::unordered_map<std::string, Token::Type> Token::reserved_word_to_token_type = {
		{"or", Type::OR},
		{"and", Type::AND},
		{"not", Type::NOT},
		{"integer", Type::Integer},
		{"float", Type::Float},
		{"void", Type::Void},
		{"class", Type::Class},
		{"self", Type::Self},
		{"isa", Type::Isa},
		{"while", Type::While},
		{"if", Type::If},
		{"then", Type::Then},
		{"else", Type::Else},
		{"read", Type::Read},
		{"write", Type::Write},
		{"return", Type::Return},
		{"localvar", Type::LocalVar},
		{"constructor", Type::Constructor},
		{"attribute", Type::Attribute},
		{"function", Type::Function},
		{"public", Type::Public},
		{"private", Type::Private},
};

std::ostream& operator<<(std::ostream& os, const Token& t) {
	os << "[" << Token::get_string(t.token_type) << ", " << t.lexeme << ", " << t.line_location << "]";
	return os;
}