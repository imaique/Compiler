#pragma once
#include <string>
#include "AST.h"
#include "SymbolTable.h"
#include <unordered_set>

class SemanticAnalyzer {
private:
	class SemanticError {
	public:
		int line_location;
		std::string text;

		SemanticError(std::string text, int line_location);
	};
	
	std::vector<SemanticError> errors;
	std::vector<SemanticError> warnings;
	void add_error(std::string text, int line);
	void add_warning(std::string text, int line);
	void print_errors();
	std::string filename;
	SymbolTable* construct_symbol_tables(AST* node);

	//SymbolTableEntry* generate_entry(AST* node, SymbolTableEntry::Kind kind);
	SymbolTableClassEntry* generate_class_entry(AST* class_node);
	SymbolTableEntry* generate_function_entry(AST* function_node, SymbolTableEntry::Kind func_kind, std::string parent_class);
	SymbolTableEntry* generate_variable_entry(AST* var_node, SymbolTableEntry::Kind kind);
	SymbolTableEntry* generate_inherit_entry(AST* id_node);
	void traverse_parents(SymbolTableClassEntry* class_entry, std::unordered_set<SymbolTableClassEntry*>& augmented_classes, const SymbolTable* global_table);
	std::vector<int> get_dimensions(const std::vector <AST*> dimension_nodes);
	void perform_semantic_checks(AST* root, SymbolTable* global_table);
	void pull_members(SymbolTableClassEntry* class_entry, SymbolTableClassEntry* parent_entry);
	void traverse_parents(SymbolTableClassEntry* class_entry, std::unordered_set<SymbolTableClassEntry*>& augmented_classes, std::unordered_set<SymbolTableClassEntry*>& current_path, const SymbolTable* global_table);

	SymbolTableEntry::Visibility SemanticAnalyzer::get_visibility(const AST* visibility_node);
	std::vector<AST*>  get_types(const std::vector <AST*>& list, std::string type);
	AST* get_type(const std::vector <AST*>& list, std::string type);

	friend bool operator<(const SemanticAnalyzer::SemanticError& p1, const SemanticAnalyzer::SemanticError& p2);

public:

	SemanticAnalyzer(std::string filename);
	bool analyze();

};