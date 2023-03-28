#pragma once
#include <stack>
#include <string>
#include "../Semantic/AST.h"
class CodeGenerator {
private:
	std::stack<std::string> registers;
	std::ofstream moon_file;
	AST* const root;
	SymbolTable* const global_table;
	std::unordered_map<std::string, int> known_sizes{ {"integer", 4},{"float", 8} };
	std::string get_register();
	void give_register(std::string reg);
public:
	CodeGenerator(std::string filename, AST* root, SymbolTable* global_table);
	void generate();
	void get_memory_blocks();
	int get_memory(std::string type_id);
	void get_children_sizes(std::unordered_map <std::string, SymbolTableEntry*> entries, int& scope_size);
	int get_memory(SymbolTableEntry* entry);
	void generate_temporary_function_variables(AST* node, int& scope_size, int& temp_count, SymbolTable* func_table);
	void generate_code(AST* node, const SymbolTableEntry* function_entry, const SymbolTable* class_table, const SymbolTable* dot_class_table);
	int get_size(SymbolType type);
};