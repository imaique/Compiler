#pragma once
#include <string>
#include <unordered_map>

class Token {
	std::string lexeme;
	int line_location;
public:
	
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
		InvalidCharacter,
	};

	static const std::unordered_map<std::string, Type> reserved_word_to_token_type;

	std::string token_type_to_string(Type type);
};