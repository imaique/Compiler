#pragma once
#include "Token.h"
#include "State/State.h"
#include <iostream>
#include <fstream>
#include <unordered_set>

class MapAndToken {

};

class LexicalAnalyzer {
	void construct_states();

	State* state;
	int index;
	int line_number;
	std::string line;
	std::ofstream token_file;
	std::ofstream error_file;
	std::ifstream input_file;
	void valid_state(int);
	void increment_line();
	void increment_index();
	void skip_empty_lines();
public:
	LexicalAnalyzer(std::string);

	Token get_next_token();
	bool has_next_token();
};