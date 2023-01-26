#include "State.h"

typedef Response r;

/* State */
State* State::get_state(int state) {
	return state_map[state];
}

std::unordered_map<int, State*> State::state_map = {};

State::State() : unsupported_transition(-1) {};

State::State(int unsupported_transition) : unsupported_transition(unsupported_transition), is_final_state(false) {};

Response::Response(bool consume, int nextState) : consume(consume), nextState(State::get_state(nextState)) {};

Response::Response(bool consume, State* nextState) : consume(consume), nextState(nextState) {};

/* LetterState */

LetterState::LetterState(int letter_transition) : letter_transition(letter_transition) {};

Response LetterState::get_next_state(char c) {
	if (isalpha(c)) return r{ true, letter_transition };
	else return r{ false, unsupported_transition };;
}

/* DigitState */

DigitState::DigitState(int digit_transition, int zero_transition, int unsupported_transition) 
	: digit_transition(digit_transition), zero_transition(zero_transition), State(unsupported_transition) {};

Response DigitState::get_next_state(char c) {
	if (isdigit(c)) return r{ true, digit_transition };
	else return r{ false, unsupported_transition };;
}

/* CharacterState */

Response CharacterState::get_next_state(char c) { 
	if (valid_transitions.find(c) != valid_transitions.end()) return r{ true, valid_transitions.at(c) };
	else return r{ false, unsupported_transition };
}

/* BlockCommentState */

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

CompositeState::CompositeState(int unsupported_transition, std::vector<State> states) 
	: states(states), State(unsupported_transition) {};

Response CompositeState::get_next_state(char c) {
	for (State& state : states) {
		// if the child state wants to perform a character transition, return it
		Response response = state.get_next_state(c);
		if (response.consume) return response;
	}
	return r{ false, unsupported_transition };
}


/* FinalState */
FinalState::FinalState(Token::Type token_type) : token_type(token_type) {};

// make sure all good
Response FinalState::get_next_state(char c) { return r{ false, -1 }; }

Token::Type FinalState::get_token_type() { return token_type; }