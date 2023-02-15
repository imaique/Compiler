#include "State.h"

typedef Response r;
typedef Token::Type TT;

/* State */
State* State::get_state(int state) {
	return state_map.at(state);
}

Response State::unsupported_response() {
	return r{ consume_on_unsupported_transition, unsupported_transition };
}

const std::unordered_map<int, State*> State::get_state_map() {
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

	const std::unordered_map<int, State*> state_map{
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
		{11, new CompositeState({new DigitState(15, 150), new CharacterState({{'+', 12}, {'-', 12}})}, INVALIDNUM)},
		{12, new DigitState(15, 150, INVALIDNUM)},
		{15, new DigitState(15, 15, FLOATNUM)},
		{150, new FinalState(TT::FloatNumber)},
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

		{-1, nullptr},
	};

	return state_map;
}

const std::unordered_map<int, State*> State::state_map = get_state_map();

State::State(bool is_final_state) : unsupported_transition(-1), consume_on_unsupported_transition(false), is_final_state(is_final_state) {};

State::State() : State(false) {};


State::State(int unsupported_transition, bool consume_on_unsupported_transition)
	: unsupported_transition(unsupported_transition), is_final_state(false), 
	consume_on_unsupported_transition(consume_on_unsupported_transition) {};

/* Response */

Response::Response(bool consume, int nextState) : consume(consume), nextState(State::get_state(nextState)) {};

Response::Response(bool consume, State* nextState) : consume(consume), nextState(nextState) {};

/* LetterState */

LetterState::LetterState(int letter_transition) : LetterState(letter_transition, -1) {};

LetterState::LetterState(int letter_transition, int unsupported_transition) 
	: letter_transition(letter_transition), State(unsupported_transition, false) {};

LetterState::LetterState(int letter_transition, int unsupported_transition, bool consume_on_unsupported_transition)
	: letter_transition(letter_transition), State(unsupported_transition, consume_on_unsupported_transition) {};

Response LetterState::get_next_state(char c) {
	if (isalpha(c)) return r{ true, letter_transition };
	else return unsupported_response();
}

/* DigitState */

DigitState::DigitState(int digit_transition, int zero_transition)
	: DigitState(digit_transition, zero_transition, -1) {};

DigitState::DigitState(int digit_transition, int zero_transition, int unsupported_transition) 
	: digit_transition(digit_transition), zero_transition(zero_transition), State(unsupported_transition, false) {};

Response DigitState::get_next_state(char c) {
	if (isdigit(c)) return r{ true, c == '0' ? zero_transition : digit_transition};
	else return unsupported_response();
}

/* CharacterState */
CharacterState::CharacterState(std::unordered_map<char, int> valid_transitions, int unsupported_transition, bool consume_on_unsupported_transition)
	: valid_transitions(valid_transitions), State(unsupported_transition, consume_on_unsupported_transition) {
}

CharacterState::CharacterState(std::unordered_map<char, int> valid_transitions, int unsupported_transition) 
	: valid_transitions(valid_transitions), State(unsupported_transition, false) {
}

CharacterState::CharacterState(std::unordered_map<char, int> valid_transitions)
	: CharacterState(valid_transitions, -1) {
}

Response CharacterState::get_next_state(char c) { 
	if (valid_transitions.find(c) != valid_transitions.end()) return r{ true, valid_transitions.at(c) };
	else return unsupported_response();
}

/* BlockCommentState */

BlockCommentState::BlockCommentState(int after_comment_transition) : after_comment_transition(after_comment_transition) {
}

Response BlockCommentState::get_next_state(char c) { 
	if (previous_character == '/' && c == '*') counter++;
	if (previous_character == '*' && c == '/') counter--;

	if (counter == 0) {
		// reset counter for next time(s)
		counter = 1;
		previous_character = 'a';
		return r{ true, after_comment_transition };
	}
	else {
		previous_character = c;
		return r{ true, this };
	}
}

/* CompositeState */

CompositeState::CompositeState(std::vector<State*> states, int unsupported_transition, bool consume_on_unsupported_transition)
	: states(states), State(unsupported_transition, consume_on_unsupported_transition) {};

CompositeState::CompositeState(std::vector<State*> states, int unsupported_transition)
	: states(states), State(unsupported_transition, false) {};

Response CompositeState::get_next_state(char c) {
	for (State* state : states) {
		// if the child state wants to perform a character transition, return it
		Response response = state->get_next_state(c);
		if (response.consume) return response;
	}
	return unsupported_response();
}


/* FinalState */
FinalState::FinalState(Token::Type token_type) : token_type(token_type), State(true) {};

// make sure all good
Response FinalState::get_next_state(char c) { return r{ false, -1 }; }

Token::Type FinalState::get_token_type() { return token_type; }