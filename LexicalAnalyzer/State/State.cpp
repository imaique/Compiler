#include "State.h"

typedef Response r;

/* State */
State* State::get_state(int state) {
	return state_map[state];
}

Response State::unsupported_response() {
	return r{ consume_on_unsupported_transition, unsupported_transition };
}

void State::set_state_map(std::unordered_map<int, State*> state_map) {
	State::state_map = state_map;
}

std::unordered_map<int, State*> State::state_map;

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