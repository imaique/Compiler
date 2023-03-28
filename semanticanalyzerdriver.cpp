#include "Semantic/SemanticAnalyzer.h"
#include <iostream>
#include <string>
#include <filesystem>

int main() {
    std::vector<std::string> test_files{ "polynomialsemanticerrors", "syntax-func", "example-polynomial", "example-bubblesort" , };//, "full-coverage" };

    for (auto filename : test_files) {
        std::string source_file = "input/" + filename + ".src";
        std::cout << filename << std::endl;
        if (std::filesystem::exists(source_file)) {
            SemanticAnalyzer sa = SemanticAnalyzer(filename);
            sa.analyze();
        }
        else {
            std::cout << source_file << " doesn't exist. Please make sure the file is in the input folder. Skipping file." << std::endl;
        }
    }
}
