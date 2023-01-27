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
	last_token_line = -1;
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
	const int LESSTHAN = 400;
	const int COLON = 500;
	const int ASSIGN = 600;
	const int GREATERTHAN = 700;



	std::unordered_map<int, State*> state_map{
		{1, new CompositeState({new LetterState(3), new DigitState(5, 7), new CharacterState(
			{
				{'/', 16},
				{'.', 22},
				{'+', 23},
				{'-', 24},
				{';', 43},
				{'[', 25},
				{']', 26},
				{'{', 27},
				{'}', 28},
				{'<', 29},
				{':', 32},
				{'(', 34},
				{')', 35},
				{'*', 36},
				{'=', 37},
				{'>', 40},
				{',', 42},
			}
		)}
			, INVALIDCHAR, true)},
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
		{22, new FinalState(TT::Point)},
		{23, new FinalState(TT::Plus)},
		{24, new FinalState(TT::Minus)},
		{43, new FinalState(TT::SemiColon)},
		{25, new FinalState(TT::OpenSquareBracket)},
		{26, new FinalState(TT::ClosedSquareBracket)},
		{27, new FinalState(TT::OpenCurlyBracket)},
		{28, new FinalState(TT::ClosedCurlyBracket)},
		{29, new CharacterState({{'>', 30},{'=', 31}}, LESSTHAN)},
		{30, new FinalState(TT::NotEqual)},
		{31, new FinalState(TT::LessOrEqualThan)},
		{32, new CharacterState({{':', 33}}, COLON)},
		{33, new FinalState(TT::ScopeOperator)},
		{34, new FinalState(TT::OpenParenthesis)},
		{35, new FinalState(TT::ClosedParenthesis)},
		{36, new FinalState(TT::Multiply)},
		{37, new CharacterState({{'>', 38},{'=', 39}}, ASSIGN)},
		{38, new FinalState(TT::ReturnType)},
		{39, new FinalState(TT::Equal)},
		{40, new CharacterState({{'=', 41}}, GREATERTHAN)},
		{41, new FinalState(TT::GreaterOrEqualThan)},
		{42, new FinalState(TT::Comma)},


		{ID, new FinalState(TT::ID)},
		{INTNUM, new FinalState(TT::IntegerNumber)},
		{FLOATNUM, new FinalState(TT::FloatNumber)},
		{INVALIDNUM, new FinalState(TT::InvalidNumber)},
		{INVALIDCHAR, new FinalState(TT::InvalidCharacter)},
		{DIVIDE, new FinalState(TT::Divide)},
		{LESSTHAN, new FinalState(TT::LessThan)},
		{ASSIGN, new FinalState(TT::Assign)},
		{GREATERTHAN, new FinalState(TT::GreaterThan)},
		{COLON, new FinalState(TT::Colon)},
	};

	State::set_state_map(state_map);
}

void LexicalAnalyzer::increment_line() {
	index = 0;
	line_number++;
	/*
	token_file << std::endl;
	error_file << std::endl;
	*/
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
			//token_stream << c;
			if(!end_of_line) index++;
		}
		state = response.nextState;
	}
	
	TT token_type;
	if (state->is_final_state) {
		FinalState* const final_state = dynamic_cast<FinalState*>(state);
		token_type = final_state->get_token_type();
	}
	// only happens when there's an open block comment
	// might need to modularize logic on introduction on other multi-line tokens
	else {
		token_type = TT::UnclosedBlockComment;
	}

	std::string lexeme = token_stream.str();

	//if it's a line comment, remove the \n
	if (token_type == TT::LineComment) lexeme = lexeme.substr(0, lexeme.size() - 2);
	else if (token_type == TT::ID && Token::is_reserved_word(lexeme)) token_type = Token::get_reserved_type(lexeme);

	Token token(lexeme, location, token_type);

	print_token(token);

	

	return token;
}

void LexicalAnalyzer::print_token(Token token) {
	const int location = token.line_location;
	if (last_token_line != location) {
		if (last_token_line > 0) token_file << std::endl;
		last_token_line = location;
	}
	else token_file << " ";
	token_file << token;

	if (token.is_error) {
		error_file << "Lexical error: " << Token::get_error_string(token.token_type) << ": ";
		error_file << "\"" << token.lexeme << "\": line " << location << std::endl;
	}
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

