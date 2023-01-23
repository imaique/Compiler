#pragma once

class State {
	static State* initialState;
	bool is_final_state;

	State(bool);

	virtual State* get_next_state(char);
	virtual bool consume_char(char);
};

class LetterState: virtual public State {
	State* letter_transition;
public:
	State* get_next_state(char);
	bool consume_char(char);
};