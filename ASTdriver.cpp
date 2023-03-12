#include <iostream>
#include <string>
#include <filesystem>

#include "Semantic/ASTGenerator.h"

using std::string;
using std::endl;

void traverse_ast(AST* node, int& unique, std::ofstream& dot_file) {
    int node_id = unique++;
    dot_file << node_id << "[label=\"" << node->get_type();

    if (node->is_leaf) {
        string value = node->get_value();
        if (!isalnum(value[0])) value = Token::get_string(node->token->token_type);
        dot_file << " | " << value;
    }

    dot_file << "\"];" << endl;

    if (!node->is_leaf) {
        if (node->children.size()) {
            for (AST* child_node : node->children) {
                dot_file << node_id << "->" << unique << ";";
                traverse_ast(child_node, unique, dot_file);
            }
        }
        else {
            dot_file << "none" << node_id << "[shape=point];" << endl;
            dot_file << node_id << "->none" << node_id << ";";
        }
    }
}

void write_ast(AST* root, string filename) {
    
    std::ofstream dot_file("output/outAST/" + filename + ".dot.outast");
    dot_file << "digraph AST {" << endl;
    dot_file << "node [shape=record];" << endl;
    dot_file << "node [fontname=Sans];" << "charset = \"UTF-8\"" <<  "splines = true splines = spline rankdir = LR" << endl;

    int unique = 1;

    if (root) {
        traverse_ast(root, unique, dot_file);
    }

    dot_file << "}";
}

int main() {
    std::vector<std::string> test_files{ "syntax-func", "example-polynomial", "example-bubblesort", "full-coverage"};

    for (auto filename : test_files) {
        std::string source_file = "input/" + filename + ".src";
        if (std::filesystem::exists(source_file)) {
            ASTGenerator sa = ASTGenerator(filename);
            AST* root = sa.get_AST();
            write_ast(root, filename);
        }
        else {
            std::cout << source_file << " doesn't exist. Please make sure the file is in the input folder. Skipping file." << std::endl;
        }
    }
}

