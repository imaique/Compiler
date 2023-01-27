#include "Token.h"
#include <string>
#include <iostream>

typedef Token::Type T;

Token::Token(std::string lexeme, int line_location, Type token_type) 
	: lexeme(lexeme), line_location(line_location), token_type(token_type) {}

std::string Token::get_string(Type type) { return token_type_to_string.at(type); }

const std::unordered_map<Token::Type, std::string> Token::token_type_to_string = {
		{T::IntegerNumber, "intnum"},
		{T::ID, "id"},
		{T::FloatNumber, "floatnum"},
		{T::InvalidNumber, "invalidnum"},
		{T::Equal, "eq"},
		{T::NotEqual, "noteq"},
		{T::GreaterThan, "gt"},
		{T::LessThan, "lt"},
		{T::GreaterOrEqualThan, "geq"},
		{T::LessOrEqualThan, "leq"},
		{T::Plus, "plus"},
		{T::Minus, "minus"},
		{T::Multiply, "mult"},
		{T::Divide, "div"},
		{T::Assign, "assign"},
		{T::OpenParenthesis, "openpar"},
		{T::ClosedParenthesis, "closepar"},
		{T::OpenCurlyBracket, "opencubr"},
		/*
		{T::ClosedCurlyBracket,
		{T::OpenSquareBracket,
		{T::ClosedSquareBracket,
		{T::SemiColon,
		{T::Comma,
		{T::Point,
		{T::Colon,
		{T::ReturnType,
		{T::ScopeOperator,
		{T::OR,
		{T::AND,
		{T::NOT,
		{T::Integer,
		{T::Float,
		{T::Void,
		{T::Class,
		{T::Self,
		{T::Isa,
		{T::While,
		{T::If,
		{T::Then,
		{T::Else,
		{T::Read,
		{T::Write,
		{T::Return,
		{T::LocalVar,
		{T::Constructor,
		{T::Attribute,
		{T::Function,
		{T::Public,
		{T::Private,
		{T::InvalidCharacter,
		*/
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