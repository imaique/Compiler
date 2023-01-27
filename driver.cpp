#include <iostream>
#include <string>

#include "LexicalAnalyzer/LexicalAnalyzer.h"

int main() {
	
	LexicalAnalyzer LA = LexicalAnalyzer("file.txt");
	std::cout << LA.has_next_token() << std::endl;
	Token token = LA.get_next_token();
	std::cout << token;
	std::cin.get();
	
}