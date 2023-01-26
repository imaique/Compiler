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
	std::string line = "";
	std::ifstream file;
	void valid_state(int);
public:
	LexicalAnalyzer(std::string);

	Token get_next_token();
	bool has_next_token();
};