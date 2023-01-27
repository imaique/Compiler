#include <iostream>
#include <string>

#include "LexicalAnalyzer/LexicalAnalyzer.h"


int main() {
	std::vector<std::string> test_files{ "lexpositivegrading" , "lexnegativegrading", "example-polynomial", "example-bubblesort"};

	for (auto filename : test_files) {
		LexicalAnalyzer positive = LexicalAnalyzer(filename);
		while (positive.has_next_token()) {
			Token token = positive.get_next_token();
		}
	}
}