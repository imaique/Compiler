#pragma once
#include <string>

class Token {
	std::string lexeme;
	int line_location;

	enum class Type {
		IntegerNumber,
		ID,
		FloatNumber,
		Error,
		Equal,
		NotEqual,
		GreaterThan,
		LessThan,
		GreaterOrEqualThan,
		LessOrEqualThan,
		Plus,
		Minus,
		Multiply,
		Divide,
		Assign,
		OpenParenthesis,
		ClosedParenthesis,
		OpenCurlyBracket,
		ClosedCurlyBracket,
		OpenSquareBracket,
		ClosedSquareBracket,
		SemiColon,
		Comma,
		Point,
		Colon,
		ReturnType,
		ScopeOperator,
		Integer,
		Float,
		Void,
		Class,
		Self,
		Isa,
		While,
		If,
		Then,
		Else,
		Read,
		Write,
		Return,
		LocalVar,
		Constructor,
		Attribute,
		Function,
		Public,
		Private,
	};

	std::string token_type_to_string(Type type);

	
	
};