#pragma once
#include <stack>
#include <string>
#include "../Semantic/AST.h"
#include <optional>
class CodeGenerator {
private:
	std::stack<std::string> registers;
	std::ofstream moon_file;
	AST* const root;
	SymbolTable* const global_table;
	std::unordered_map<std::string, int> known_sizes{ {"integer", 4},{"float", 8}, {"void", 0} };
	std::unordered_map<std::string, std::string> func_entry_to_short_name;
	std::string get_register();
	std::string get_short_function_name(std::string unique_id);
	void free_register(std::string reg);
	int branch_count = 1;
	int function_count = 1;
public:
	CodeGenerator(std::string filename, AST* root, SymbolTable* global_table);
	void generate();
	void get_memory_blocks();
	int get_memory(std::string type_id);
	void get_children_sizes(std::unordered_map <std::string, SymbolTableEntry*> entries, int& scope_size);
	void get_children_sizes(std::unordered_map <std::string, SymbolTableEntry*> entries, int& scope_size, SymbolType* class_type);

	int get_memory(SymbolTableEntry* entry);
	int get_memory(SymbolTableEntry* entry, SymbolType* class_type);
	void generate_temporary_function_variables(AST* node, int& scope_size, int& temp_count, SymbolTable* func_table);
	void generate_code();
	SymbolTableEntry* generate_code(AST* node, const SymbolTableEntry* function_entry, const SymbolTable* class_table, SymbolTableEntry* dot_class_table);
	std::string get_offset(SymbolTable* table, std::string unique_id);
	std::string get_offset(SymbolTableEntry* entry);
	std::string get_op_instruction(std::string op_instruop_typection);
	int get_size(SymbolType type);
	void load_word(SymbolTableEntry* entry, std::string destination_register);
	void load_value(SymbolTableEntry* entry, std::string destination_register);
	void store_into_reference(SymbolTableEntry* entry, std::string destination_register);

	void store_word(std::string destination_offset, std::string origin_register);
	void write_instruction(std::string instruction);
	void write_empty_line();
	void write_comment(std::string comment);
	void write_comment(std::string comment, int line);
	void write_instruction(std::string start, std::string instruction, std::string comment);
	void write_instruction(std::string start, std::string instruction);
	void integer_add(std::string destination_register, std::string register_operand, std::string integer_operand);
	void register_add(std::string destination_register, std::string r1, std::string r2);

	void jump_and_link(std::string destination_register, std::string address);
	void jump(std::string address);
	std::string get_branch();
};