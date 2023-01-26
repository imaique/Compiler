#include "LexicalAnalyzer.h"

typedef std::string string;

void LexicalAnalyzer::valid_state(int state) {
	this->state = state;
	index++;
}

/*
const std::unordered_map<char, MapOrToken> LexicalAnalyzer::valid_delimiters = 
{'=','<', '>',};
*/

bool LexicalAnalyzer::is_delimiter(char c) {
	return valid_delimiters.find(c) != valid_delimiters.end();
}
Token LexicalAnalyzer::get_next_token() {
	// trim start whitespace
	while (index < line.size() && line[index] == ' ') index++;

	int start = index;

	while (index < line.size() && !is_delimiter(line[index])) index++;

	// currently a delimiter, operator, punctuation etc..
	if (index == start) {

	}
	// id, integer, float or reserved word
	else {
		string lexeme = line.substr(start, index - start);

		// has to be integer or float
		if (isdigit(lexeme[0])) {

		}
		// has to be id or reserved word
		
		else {

		}
	}

	/*
	while (true) {
		char c = line[index];
		switch (state) {
		case 1:
			while (index < line.size() && c == ' ') index++;
			if (isalpha(c)) {
				valid_state(2);
			}
			else if (isdigit(c)) {
				valid_state(4);
			}
			else {
				std::unordered_map<char, int>  map { {'/', 4}};
				
			}
			break;
		}
	}
	*/
	
}
bool LexicalAnalyzer::has_next_token() {
	if (line[index] == '\n') index++;
	if (index < line.size()) return true;
	if (std::getline(file, line)) {
		index = 0;
		return true;
	}
	return false;
}