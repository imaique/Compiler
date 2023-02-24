#include "Token.h"
#include <string>
#include <iostream>

typedef Token::Type T;

Token::Token(const Token& t) : line_location(t.line_location), lexeme(t.lexeme), token_type(t.token_type), is_error(t.is_error), index_start(t.index_start) {};

Token::Token(std::string lexeme, int line_location, Type token_type, int index_start)
	: lexeme(lexeme), line_location(line_location), token_type(token_type), is_error(is_error_token_type(token_type)), index_start(index_start) {}

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
		{T::IntegerNumber, "intnum"},
		{T::ID, "id"},
		{T::FloatNumber, "floatnum"},
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
		{T::ClosedParenthesis, "rpar"},
		{T::OpenCurlyBracket, "lcurbr"},
		{T::ClosedCurlyBracket, "rcurbr"},
		{T::OpenSquareBracket, "lsqbr"},
		{T::ClosedSquareBracket, "rsqbr"},
		{T::SemiColon, "semi"},
		{T::Comma, "comma"},
		{T::Point, "dot"},
		{T::Colon, "colon"},
		{T::ReturnType, "arrow"},
		{T::ScopeOperator, "sr"},
		{T::OR, "or"},
		{T::AND, "and"},
		{T::NOT, "not"},
		{T::Integer, "integer"},
		{T::Float, "float"},
		{T::Void, "void"},
		{T::Class, "class"},
		//{T::Self, "self"},
		{T::Isa, "isa"},
		{T::While, "while"},
		{T::If, "if"},
		{T::Then, "then"},
		{T::Else, "else"},
		{T::Read, "read"},
		{T::Write, "write"},
		{T::Return, "return"},
		{T::LocalVar, "localvar"},
		{T::Constructor, "constructorkeyword"},
		{T::Attribute, "attribute"},
		{T::Function, "function"},
		{T::Public, "public"},
		{T::Private, "private"},
		{T::LineComment, "inlinecmt"},
		{T::BlockComment, "blockcmt"},
		{T::UnclosedBlockComment, "lblockcmt"},
		{T::InvalidCharacter, "invalidchar"},
		{T::OR, "or"},
		{T::AND, "and"},
		{T::NOT, "not"},
		{T::Integer, "integer"},
		{T::Float, "float"},
		{T::Void, "void"},
		{T::Class, "class"},
		//{T::Self, "self"},
		{T::Isa, "isa"},
		{T::While, "while"},
		{T::If, "if"},
		{T::Then, "then"},
		{T::Else, "else"},
		{T::Read, "read"},
		{T::Write, "write"},
		{T::Return, "return"},
		{T::LocalVar, "localvar"},
		{T::Constructor, "constructorkeyword"},
		{T::Attribute, "attribute"},
		{T::Function, "function"},
		{T::Public, "public"},
		{T::Private, "private"},
		{T::EndOfFile, "eof"},
};

const std::unordered_map<std::string, Token::Type> Token::string_to_token_type = {
		{"intnum", T::IntegerNumber},
		{"id", T::ID},
		{"floatnum", T::FloatNumber},
		{"invalidnum", T::InvalidNumber},
		{"eq", T::Equal},
		{"neq", T::NotEqual},
		{"gt", T::GreaterThan},
		{"lt", T::LessThan},
		{"geq", T::GreaterOrEqualThan},
		{"leq", T::LessOrEqualThan},
		{"plus", T::Plus},
		{"minus", T::Minus},
		{"mult", T::Multiply},
		{"div", T::Divide},
		{"assign", T::Assign},
		{"lpar", T::OpenParenthesis},
		{"rpar", T::ClosedParenthesis},
		{"lcurbr", T::OpenCurlyBracket},
		{"rcurbr", T::ClosedCurlyBracket},
		{"lsqbr", T::OpenSquareBracket},
		{"rsqbr", T::ClosedSquareBracket},
		{"semi", T::SemiColon},
		{"comma", T::Comma},
		{"dot", T::Point},
		{"colon", T::Colon},
		{"arrow", T::ReturnType},
		{"sr", T::ScopeOperator},
		{"or", T::OR},
		{"and", T::AND},
		{"not", T::NOT},
		{"integer", T::Integer},
		{"float", T::Float},
		{"void", T::Void},
		{"class", T::Class},
		//{"self", T::Self},
		{"isa", T::Isa},
		{"while", T::While},
		{"if", T::If},
		{"then", T::Then},
		{"else", T::Else},
		{"read", T::Read},
		{"write", T::Write},
		{"return", T::Return},
		{"localvar", T::LocalVar},
		{"constructorkeyword", T::Constructor},
		{"attribute", T::Attribute},
		{"function", T::Function},
		{"public", T::Public},
		{"private", T::Private},
		{"inlinecmt", T::LineComment},
		{"blockcmt", T::BlockComment},
		{"lblockcmt", T::UnclosedBlockComment},
		{"invalidchar", T::InvalidCharacter},
		{"or", T::OR},
		{"and", T::AND},
		{"not", T::NOT},
		{"integer", T::Integer},
		{"float", T::Float},
		{"void", T::Void},
		{"class", T::Class},
		//{"self", T::Self},
		{"isa", T::Isa},
		{"while", T::While},
		{"if", T::If},
		{"then", T::Then},
		{"else", T::Else},
		{"read", T::Read},
		{"write", T::Write},
		{"return", T::Return},
		{"localvar", T::LocalVar},
		{"constructorkeyword", T::Constructor},
		{"attribute", T::Attribute},
		{"function", T::Function},
		{"public", T::Public},
		{"private", T::Private},
		{"eof", T::EndOfFile},
};

const std::unordered_map<std::string, Token::Type> Token::reserved_word_to_token_type = {
		{"or", T::OR},
		{"and", T::AND},
		{"not", T::NOT},
		{"integer", T::Integer},
		{"float", T::Float},
		{"void", T::Void},
		{"class", T::Class},
		//{"self", T::Self},
		{"isa", T::Isa},
		{"while", T::While},
		{"if", T::If},
		{"then", T::Then},
		{"else", T::Else},
		{"read", T::Read},
		{"write", T::Write},
		{"return", T::Return},
		{"localvar", T::LocalVar},
		{"constructor", T::Constructor},
		{"attribute", T::Attribute},
		{"function", T::Function},
		{"public", T::Public},
		{"private", T::Private},
};

const bool Token::is_token_type(std::string s) {
	return string_to_token_type.find(s) != string_to_token_type.end();
}

std::ostream& operator<<(std::ostream& os, const Token& t) {
	os << "[" << Token::get_string(t.token_type) << ", " << t.lexeme << ", " << t.line_location << "]";
	return os;
}