#pragma once
#include "Token.h"
#include <iostream>
#include <fstream>
#include <unordered_set>

class MapAndToken {

};

class LexicalAnalyzer {
	static const std::unordered_set<char> valid_delimiters;

	int state = 1;
	int index;
	std::string line = "";
	std::ifstream file;
	void valid_state(int);
	bool is_delimiter(char c);
public:
	LexicalAnalyzer(std::string);

	Token get_next_token();
	bool has_next_token();
};