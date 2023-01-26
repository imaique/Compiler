#pragma once
#include <iostream>
#include <unordered_map>
#include "../Token.h"

class State;

struct Response {
public:
	Response(bool, int);
	Response(bool, State*);
	const bool consume;
	State* const nextState;
};

class State {
	static std::unordered_map<int, State*> state_map;
protected:
	int unsupported_transition;
	State();
	State(int unsupported_transition);
public:
	static State* get_state(int);
	//static void set_state_map(std::unordered_map<int, State*>);
	
	const bool is_final_state = false;
	virtual Response get_next_state(char) = 0;
};

class LetterState: virtual public State {
	int letter_transition;
public:
	LetterState(int);
	Response get_next_state(char);
};

class DigitState : virtual public State {
	int digit_transition;
	int zero_transition;
public:
	DigitState(int digit_transition, int zero_transition, int unsupported_transition);
	Response get_next_state(char);
};

class FinalState : virtual public State {
	const Token::Type token_type;
public:
	const bool is_final_state = true;
	FinalState(Token::Type token_type);
	Response get_next_state(char);
	Token::Type get_token_type();
};

class BlockCommentState : virtual public State {
	int after_comment_transition;
	int counter = 1;
	char previous_character = 'a';
public:
	Response get_next_state(char);
};

class CompositeState : virtual public State {
	std::vector<State> states;
public:
	CompositeState(int unsupported_transition, std::vector<State> states);
	Response get_next_state(char);
};

class CharacterState : virtual public State {
	const std::unordered_map<char, int> valid_transitions;
public:
	Response get_next_state(char);
};