#pragma once
#include <string>
#include <unordered_map>

class Token {
public:
	enum class Type {
		IntegerNumber,
		ID,
		FloatNumber,
		InvalidNumber,
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
		OR,
		AND,
		NOT,
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
		LineComment,
		BlockComment,
		InvalidCharacter,
		UnclosedBlockComment,
		EndOfFile,
	};

	Token(std::string lexeme, int line_location, Type token_type);

	

	friend std::ostream& operator<<(std::ostream& os, const Token& t);


	static bool is_error_token_type(Type type);
	static std::string get_string(Type type);
	static std::string get_error_string(Type type);
	static Type get_reserved_type(std::string lexeme);
	static bool is_reserved_word(std::string lexeme);
	const std::string lexeme;
	const int line_location;
	const Type token_type;
	//const std::string string_type;
	const bool is_error;

private:
	static const std::unordered_map<Type, std::string> error_type_to_string;
	static const std::unordered_map<Type, std::string> token_type_to_string;
	static const std::unordered_map<std::string, Type> reserved_word_to_token_type;
	
};