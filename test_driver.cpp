#include <iostream>
#include <string>

#include "LexicalAnalyzer/LexicalAnalyzer.h"


int main() {
	LexicalAnalyzer positive = LexicalAnalyzer("edge-cases");
	while (positive.has_next_token()) {
		Token token = positive.get_next_token();
	}
	
}