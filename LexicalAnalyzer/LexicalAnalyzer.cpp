#include "LexicalAnalyzer.h"
#include <sstream>

typedef std::string string;
typedef Token::Type TT;

LexicalAnalyzer::LexicalAnalyzer(string filename) {
	token_file.open(filename + ".outlextokens");
	error_file.open(filename + ".outlexerrors");
	input_file.open(filename + ".src");

	index = 0;
	line_number = 1;
	std::getline(input_file, line);

	construct_states();
}

void LexicalAnalyzer::construct_states() {
	const int INTNUM = 100;
	const int FLOATNUM = 200;
	const int ID = 4;
	const int INVALIDNUM = 404;
	const int INVALIDCHAR = 405;
	const int DIVIDE = 300;



	std::unordered_map<int, State*> state_map{
		{1, new CompositeState({new LetterState(3), new DigitState(5, 7), new CharacterState({{'/', 16}})}, INVALIDCHAR, true)},
		{3, new CompositeState({new LetterState(3), new DigitState(3, 3), new CharacterState({{'_', 3}})}, ID)},
		{5, new CompositeState({new DigitState(5, 5), new CharacterState({{'.',8}})}, INTNUM)},
		{7, new CharacterState({{'.', 8}}, INTNUM)},
		{8, new DigitState(9, 9, INVALIDNUM)},
		{9, new CompositeState({new DigitState(9,10), new CharacterState({{'e',11}})}, FLOATNUM)},
		{10, new DigitState(9, 10, INVALIDNUM)},
		{11, new CompositeState({new DigitState(15, INVALIDNUM), new CharacterState({{'+', 12}, {'-', 12}})}, INVALIDNUM)},
		{12, new DigitState(15, INVALIDNUM)},
		{15, new DigitState(15, 15, FLOATNUM)},
		{16, new CharacterState({{'/', 17},{'*', 19}}, DIVIDE)},
		{17, new CharacterState({{'\n', 18}}, 17, true)},
		{18, new FinalState(TT::LineComment)},
		{19, new BlockCommentState(21)},
		{21, new FinalState(TT::BlockComment)},


		{ID, new FinalState(TT::ID)},
		{INTNUM, new FinalState(TT::IntegerNumber)},
		{FLOATNUM, new FinalState(TT::FloatNumber)},
		{INVALIDNUM, new FinalState(TT::InvalidNumber)},
		{INVALIDCHAR, new FinalState(TT::InvalidCharacter)},
		{DIVIDE, new FinalState(TT::Divide)},
	};

	State::set_state_map(state_map);
}

void LexicalAnalyzer::increment_line() {
	index = 0;
	line_number++;
	token_file << std::endl;
	error_file << std::endl;
}


Token LexicalAnalyzer::get_next_token() {
	has_next_token();
	this->state = State::get_state(1);
	std::stringstream token_stream;

	int start = index;
	const int location = line_number;

	while (!state->is_final_state) {
		bool end_of_line = index == line.size();
		char c;
		if (!end_of_line) c = line[index];
		else {
			c = '\n';
			if(!std::getline(input_file, line)) break;
			increment_line();
		}
		Response response = state->get_next_state(c);
		if (response.consume) {
			if(c == '\n') token_stream << "\\n";
			else token_stream << c;
			if(!end_of_line) index++;
		}
		state = response.nextState;
	}

	if (!state->is_final_state) {
		Response response = state->get_next_state('\n');
		state = response.nextState;
	}

	FinalState* const final_state = dynamic_cast<FinalState*>(state);
	TT token_type = final_state->get_token_type();
	const std::string lexeme = token_stream.str();
	

	if (token_type == TT::ID && Token::is_reserved_word(lexeme)) 
		token_type = Token::get_reserved_type(lexeme);

	Token token(lexeme, location, token_type);

	token_file << token;

	return token;
}
void LexicalAnalyzer::skip_empty_lines() {
	// trim starting whitespace
	while (index < line.size() && (line[index] == ' ' || line[index] == '\t' || line[index] == '\n')) index++;
	while (index >= line.size() && std::getline(input_file, line)) {
		increment_line();
	}
}
bool LexicalAnalyzer::has_next_token() {
	skip_empty_lines();
	if (index < line.size()) return true;
	else {
		token_file.close();
		error_file.close();
		return false;
	}
}

