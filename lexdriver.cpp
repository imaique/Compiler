#include <iostream>
#include <string>
#include <filesystem>

#include "LexicalAnalyzer/LexicalAnalyzer.h"


int main() {
	std::vector<std::string> test_files{ "lexpositivegrading" , "lexnegativegrading", "example-polynomial", "example-bubblesort", "edge-cases"};

	for (auto filename : test_files) {
        std::string source_file = "input/" + filename + ".src";
        if(std::filesystem::exists(source_file)) {
            LexicalAnalyzer positive = LexicalAnalyzer(filename);
            while (positive.has_next_token()) {
                Token token = positive.get_next_token();
            }
        } else {
            std::cout << source_file << " doesn't exist. Please make sure the file is in the input folder. Skipping file." << std::endl;
        }
	}
}
