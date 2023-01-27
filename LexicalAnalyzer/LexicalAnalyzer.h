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
	int index = 0;
	int line_number = 0;
	std::string line = "";
	std::ofstream token_file;
	std::ofstream error_file;
	std::ifstream input_file;
	void valid_state(int);
public:
	LexicalAnalyzer(std::string);

	Token get_next_token();
	bool has_next_token();
};