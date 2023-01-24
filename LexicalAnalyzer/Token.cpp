#include "Token.h"

const std::unordered_map<std::string, Token::Type> Token::reserved_word_to_token_type = {
		{"private", Type::Private},
};