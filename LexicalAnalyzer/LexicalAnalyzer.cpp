#include "LexicalAnalyzer.h"

typedef std::string string;
typedef Token::Type TT;

LexicalAnalyzer::LexicalAnalyzer(string s) {
	file.open(s);

	construct_states();
}

void LexicalAnalyzer::construct_states() {
	std::unordered_map<int, State*> state_map{
		{6, new DigitState(6,6,7)},
		{7, new FinalState(TT::IntegerNumber)}
	};

}

Token LexicalAnalyzer::get_next_token() {
	// trim start whitespace
	while (index < line.size() && line[index] == ' ') index++;

	int start = index;

	while (!state->is_final_state) {
		char c = line[index];
		Response response = state->get_next_state(c);
		if (response.consume) index++;
		state = response.nextState;
	}

	FinalState* final_state = dynamic_cast<FinalState*>(state);

	return Token();
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