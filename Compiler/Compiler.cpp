#include "Compiler.h"
#include "../Semantic/SemanticAnalyzer.h"
#include <filesystem>
#include <iostream>

using std::string;
using std::cin;
using std::cout;
using std::endl;
using std::vector;
using std::filesystem::directory_iterator;

std::vector<CompilerError> Compiler::all_errors;

void Compiler::reset_logger()
{
    all_errors.clear();
}

void Compiler::add_error(CompilerError error)
{
    all_errors.push_back(error);
}

void Compiler::print_errors(std::string filename) {
    std::ofstream file("output/outcompile/" + filename + ".outcompile");
    std::sort(all_errors.begin(), all_errors.end());

    for (CompilerError error : all_errors) {
        file << error.text;
        if (error.line_location >= 0) file << " (line " << error.line_location << ")";
        file << endl;
    }
}

void Compiler::compile(std::string filename)
{
    reset_logger();
    std::string source_file = "input/" + filename + ".src";

    std::cout << endl << "Compiling " << filename << std::endl;
    if (std::filesystem::exists(source_file)) {
        SemanticAnalyzer sa = SemanticAnalyzer(filename);
        sa.analyze();
    }
    else {
        std::cout << source_file << " doesn't exist. Please make sure the file is in the input folder. Skipping file." << std::endl;
    }
    print_errors(filename);
    cout << "Compilation end." << endl << endl;
}
