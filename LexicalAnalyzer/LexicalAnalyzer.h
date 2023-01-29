#pragma once
#include "Token.h"
#include "State/State.h"
#include <iostream>
#include <fstream>
#include <unordered_set>

class MapAndToken {

};

class LexicalAnalyzer {
	State* state;
	int index;
	int line_number;
	std::string line;
	std::ofstream token_file;
	int last_token_line;
	std::ofstream error_file;
	std::ifstream input_file;
	void valid_state(int);
	void increment_line();
	void increment_index();
	void skip_empty_lines();
	void print_token(Token token);
public:
	LexicalAnalyzer(std::string);

	Token get_next_token();
	bool has_next_token();
};