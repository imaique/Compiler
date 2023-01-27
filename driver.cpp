#include <iostream>
#include <string>

#include "LexicalAnalyzer/LexicalAnalyzer.h"

int main() {
	State* state = new FinalState(Token::Type::AND);
	std::cout << state->is_final_state;
	
	LexicalAnalyzer LA = LexicalAnalyzer("lexpositivegrading");
	while (LA.has_next_token()) {
		Token token = LA.get_next_token();
		std::cout << token;
	}
	std::cin.get();
	
}