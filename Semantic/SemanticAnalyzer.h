#pragma once
#include <string>
#include "AST.h"
#include "SymbolTable.h"
#include <unordered_set>
#include "../Compiler/CompilerError.h"

class SemanticAnalyzer {
private:	
	std::vector<CompilerError> errors;
	std::vector<CompilerError> warnings;
	void add_error(std::string text, int line);
	void add_warning(std::string text, int line);
	void print_errors();
	bool same_type(SymbolType t1, SymbolType t2);
	std::string filename;
	SymbolTable* construct_symbol_tables(AST* node);

	//SymbolTableEntry* generate_entry(AST* node, SymbolTableEntry::Kind kind);
	void check_function_names(SymbolTable* global_table);
	void check_types(AST* root, SymbolTable* global_table);
	int migrate_line_locations(AST* node);
	SymbolTableClassEntry* generate_class_entry(AST* class_node);
	SymbolTableEntry* generate_function_entry(AST* function_node, SymbolTableEntry::Kind func_kind, std::string parent_class);
	SymbolTableEntry* generate_variable_entry(AST* var_node, SymbolTableEntry::Kind kind);
	SymbolTableEntry* generate_inherit_entry(AST* id_node);
	std::vector<int> get_dimensions(const std::vector <AST*> dimension_nodes);
	void perform_semantic_checks(AST* root, SymbolTable* global_table);
	void pull_members(SymbolTableClassEntry* class_entry, SymbolTableClassEntry* parent_entry);
	void traverse_parents(SymbolTableClassEntry* class_entry, std::unordered_set<SymbolTableClassEntry*>& augmented_classes, std::unordered_set<SymbolTableClassEntry*>& current_path, const SymbolTable* global_table);
	SymbolType resolve_type(AST* node, const SymbolTableEntry* function_entry, const SymbolTable* class_table, const SymbolTable* global_table);
	SymbolType resolve_type(AST* node, const SymbolTableEntry* function_entry, const SymbolTable* class_table, const SymbolTable* global_table, const SymbolTable* dot_class_table);

	SymbolTableEntry::Visibility get_visibility(const AST* visibility_node);
public:

	SemanticAnalyzer(std::string filename);
	bool analyze();

};
