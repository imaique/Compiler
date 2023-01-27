#include "LexicalAnalyzer.h"

typedef std::string string;
typedef Token::Type TT;

LexicalAnalyzer::LexicalAnalyzer(string filename) {
	token_file.open(filename + ".outlextokens");
	error_file.open(filename + ".outlexerrors");
	input_file.open(filename + ".txt");

	construct_states();
}

void LexicalAnalyzer::construct_states() {
	const int INTNUM = 100;
	const int FLOATNUM = 200;
	const int ID = 4;
	const int INVALIDNUM = 404;
	const int INVALIDCHAR = 405;
	std::unordered_map<int, State*> state_map{
		{1, new CompositeState({new LetterState(2), new DigitState(5, 7)}, INVALIDCHAR)},
		{2, new CompositeState({new LetterState(3), new DigitState(3, 3)}, ID)},
		{3, new CompositeState({new LetterState(3)}, ID)},
		{5, new CompositeState({new DigitState(6, 6), new CharacterState({{'.',8}})}, INTNUM)},
		{6, new DigitState(6,6,INTNUM)},
		{7, new CharacterState({{'.', 8}}, INTNUM)},
		{8, new DigitState(9, 9, INVALIDNUM)},

		{ID, new FinalState(TT::ID)},
		{INTNUM, new FinalState(TT::IntegerNumber)},
		{FLOATNUM, new FinalState(TT::FloatNumber)},
		{INVALIDNUM, new FinalState(TT::InvalidNumber)},
	};

	State::set_state_map(state_map);
}

Token LexicalAnalyzer::get_next_token() {
	// reset the state (might be a problem with block comments?)
	this->state = State::get_state(1);

	// trim starting whitespace
	while (index < line.size() && line[index] == ' ') index++;

	int start = index;

	while (index < line.size() && !state->is_final_state) {
		char c = line[index];
		Response response = state->get_next_state(c);
		if (response.consume) index++;
		state = response.nextState;
	}

	if (!state->is_final_state) {
		Response response = state->get_next_state('\n');
		state = response.nextState;
	}

	FinalState* const final_state = dynamic_cast<FinalState*>(state);
	TT token_type = final_state->get_token_type();
	const std::string lexeme = line.substr(start, index - start);
	const int location = line_number;

	if (token_type == TT::ID && Token::is_reserved_word(lexeme)) 
		token_type = Token::get_reserved_type(lexeme);

	return Token(lexeme, location, token_type);
}
bool LexicalAnalyzer::has_next_token() {
	if (index < line.size() && line[index] == '\n') index++;
	if (index < line.size()) return true;
	if (std::getline(input_file, line)) {
		index = 0;
		line_number++;
		return true;
	}
	return false;
}