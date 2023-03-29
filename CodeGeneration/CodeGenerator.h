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
	std::unordered_map<std::string, int> known_sizes{ {"integer", 4},{"float", 8}, {"void", 0} };
	std::unordered_map<SymbolTableEntry*, std::string> func_entry_to_short_name;
	std::string get_register();
	void free_register(std::string reg);
public:
	CodeGenerator(std::string filename, AST* root, SymbolTable* global_table);
	void generate();
	void get_memory_blocks();
	int get_memory(std::string type_id);
	void get_children_sizes(std::unordered_map <std::string, SymbolTableEntry*> entries, int& scope_size);
	int get_memory(SymbolTableEntry* entry);
	void generate_temporary_function_variables(AST* node, int& scope_size, int& temp_count, SymbolTable* func_table);
	void generate_code();
	void generate_code(AST* node, const SymbolTableEntry* function_entry, const SymbolTable* class_table, const SymbolTable* dot_class_table);
	std::string get_offset(SymbolTable* table, std::string unique_id);
	std::string get_offset(SymbolTableEntry* entry);
	int get_size(SymbolType type);
	void load_word(SymbolTableEntry* entry, std::string destination_register);
	void store_word(std::string destination_offset, std::string origin_register);
	void write_instruction(std::string instruction);
	void write_instruction(std::string start, std::string instruction);
	void integer_add(std::string destination_register, std::string register_operand, std::string integer_operand);
	void jump_and_link(std::string destination_register, std::string address);
};