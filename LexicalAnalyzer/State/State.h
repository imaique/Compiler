#pragma once
#include <iostream>
#include <unordered_map>
#include "../Token.h"

struct Response {
public:
	Response(bool, State*);
	const bool consume;
	const State* nextState;
};

class State {
protected:
	static State* initial_state;
	static State* error_state;
	State* unsupported_character_transition;
	const bool is_final_state = false;

	virtual Response get_next_state(char) = 0;
};

class LetterState: virtual public State {
	State* letter_transition;
public:
	LetterState(State*);
	Response get_next_state(char);
};

class DigitState : virtual public State {
	State* digit_transition;
public:
	DigitState(State*);
	Response get_next_state(char);
};

class FinalState : virtual public State {
	const bool is_final_state = true;
	const Token::Type token_type;
public:
	Response get_next_state(char);
	Token::Type get_token_type();
};

class BlockCommentState : virtual public State {
	State* after_comment_transition;
	int counter = 1;
	char previous_character = 'a';
public:
	Response get_next_state(char);
};

class CharacterState : virtual public State {
	const std::unordered_map<char, State*> valid_transitions;
public:
	Response get_next_state(char);
};