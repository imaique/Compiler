#include <iostream>
#include <string>
#include <filesystem>
#include "Utils/utils.h"
#include "Semantic/ASTGenerator.h"

int main() {
    std::vector<std::string> test_files{ "syntax-func", "example-polynomial", "example-bubblesort", "full-coverage"};

    for (auto filename : test_files) {
        std::string source_file = "input/" + filename + ".src";
        if (std::filesystem::exists(source_file)) {
            ASTGenerator sa = ASTGenerator(filename);
            AST* root = sa.get_AST();
            dot_writer::write_ast(root, filename);
        }
        else {
            std::cout << source_file << " doesn't exist. Please make sure the file is in the input folder. Skipping file." << std::endl;
        }
    }
}

