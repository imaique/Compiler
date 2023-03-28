#pragma once

#include "../Semantic/AST.h"

namespace dot_writer {
    void traverse_ast(AST* node, int& unique, std::ofstream& dot_file);
    void write_ast(AST* root, std::string filename);
}

namespace ast {
    std::vector<AST*> get_types(const std::vector <AST*>& list, std::string type);
    AST* get_type(const std::vector <AST*>& list, std::string type);
}
