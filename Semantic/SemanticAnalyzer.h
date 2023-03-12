#pragma once
#include <string>
#include "AST.h"
#include "SymbolTable.h"

class SemanticAnalyzer {
private:
	std::string filename;
	SymbolTable* construct_symbol_tables(AST* node);

	//SymbolTableEntry* generate_entry(AST* node, SymbolTableEntry::Kind kind);
	SymbolTableEntry* generate_class_entry(AST* class_node);
	SymbolTableEntry* generate_function_entry(AST* function_node);
	SymbolTableEntry* generate_variable_entry(AST* var_node, SymbolTableEntry::Kind kind);
	std::vector<int> get_dimensions(const std::vector <AST*> dimension_nodes);

	std::vector<AST*>  get_types(const std::vector <AST*>& list, std::string type);
	AST* get_type(const std::vector <AST*>& list, std::string type);

public:

	SemanticAnalyzer(std::string filename);
	bool analyze();

};