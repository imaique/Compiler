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
	static const std::unordered_map<int, State*> get_state_map();
	static const std::unordered_map<int, State*> state_map;
protected:
	int unsupported_transition;
	bool consume_on_unsupported_transition;
	State();
	State(bool is_final_state);
	State(int unsupported_transition, bool consume_on_unsupported_transition);
public:
	static State* get_state(int);
	
	const bool is_final_state;
	virtual Response get_next_state(char) = 0;
	Response unsupported_response();
};

class LetterState: virtual public State {
	int letter_transition;
public:
	LetterState(int letter_transition);
	LetterState(int letter_transition, int unsupported_transition);
	LetterState(int letter_transition, int unsupported_transition, bool consume_on_unsupported_transition);
	Response get_next_state(char);
};

class DigitState : virtual public State {
	int digit_transition;
	int zero_transition;
public:
	DigitState(int digit_transition, int zero_transition);
	DigitState(int digit_transition, int zero_transition, int unsupported_transition);
	Response get_next_state(char);
};

class FinalState : virtual public State {
	const Token::Type token_type;
public:
	FinalState(Token::Type token_type);
	Response get_next_state(char);
	Token::Type get_token_type();
};

class BlockCommentState : virtual public State {
	int after_comment_transition;
	int counter = 1;
	char previous_character = 'a';
public:
	BlockCommentState(int after_comment_transition);
	Response get_next_state(char);
};

class CompositeState : virtual public State {
	std::vector<State*> states;
public:
	CompositeState(std::vector<State*> states, int unsupported_transition);
	CompositeState(std::vector<State*> states, int unsupported_transition, bool consume_on_unsupported_transition);
	Response get_next_state(char);
};

class CharacterState : virtual public State {
	const std::unordered_map<char, int> valid_transitions;
public:
	CharacterState(std::unordered_map<char, int> valid_transitions);
	CharacterState(std::unordered_map<char, int> valid_transitions, int unsupported_transition);
	CharacterState(std::unordered_map<char, int> valid_transitions, int unsupported_transition, bool consume_on_unsupported_transition);
	Response get_next_state(char);
};