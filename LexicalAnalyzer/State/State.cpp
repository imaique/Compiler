#include "State.h"

typedef Response r;

Response::Response(bool consume, State* nextState) : consume(consume), nextState(nextState) {};

/* LetterState */

LetterState::LetterState(State* letter_transition) : letter_transition(letter_transition) {};

Response LetterState::get_next_state(char c) {
	if (isalpha(c)) return r{ true, letter_transition };
	else return r{ false, unsupported_character_transition };;
}

/* DigitState */

DigitState::DigitState(State* digit_transition) : digit_transition(digit_transition) {};

Response DigitState::get_next_state(char c) {
	if (isdigit(c)) return r{ true, digit_transition };
	else return r{ false, unsupported_character_transition };;
}

/* CharacterState */

Response CharacterState::get_next_state(char c) { 
	if (valid_transitions.find(c) != valid_transitions.end()) return r{ true, valid_transitions.at(c) };
	else return r{ false, unsupported_character_transition };
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


/* FinalState */

Response FinalState::get_next_state(char c) { return r{ false, initial_state }; }

Token::Type FinalState::get_token_type() { return token_type; }