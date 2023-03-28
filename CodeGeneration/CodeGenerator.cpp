#include "CodeGenerator.h"

using namespace std;
typedef SymbolTableEntry::Kind EntryKind;
using namespace ASTConstants;
using namespace ast;

CodeGenerator::CodeGenerator(string filename, AST* root, SymbolTable* global_table) : moon_file("output/outcode/" + filename + ".moon"), root(root), global_table(global_table) {

}

void CodeGenerator::generate() {
	get_memory_blocks();
}

void CodeGenerator::get_memory_blocks() {
	for (auto entry : global_table->entries) {
		get_memory(entry.second);
	}
}
int CodeGenerator::get_memory(string type_id) {
	
	if (!known_sizes.count(type_id)) {
		SymbolTableEntry* class_entry = global_table->entries.at(type_id);
		known_sizes.insert({ type_id, get_memory(class_entry)});
	}
	return known_sizes.at(type_id);
}

void CodeGenerator::get_children_sizes(unordered_map <std::string, SymbolTableEntry*> entries, int& scope_size) {
	for (const auto& child : entries) {
		SymbolTableEntry* child_entry = child.second;
		child_entry->offset = -1 * scope_size;
		scope_size += get_memory(child_entry);
	}
}
// write(1 + 1);

void CodeGenerator::generate_temporary_function_variables(AST* node, int& scope_size, int& temp_count, SymbolTable* func_table) {
	for (AST* child_node : node->children) generate_temporary_function_variables(child_node, scope_size, temp_count, func_table);

	if (node->type == Operation || node->type == Variable) {
		int size = get_size(node->decorator.get_type());
		string name = "t" + temp_count++;
		string unique_id = '_' + name;
		EntryKind kind = EntryKind::TempVar;
		SymbolType* type = new SymbolType(node->decorator.get_type());
		SymbolTableEntry* temporary_variable_entry = new SymbolTableEntry(unique_id, name, kind, type, -1, SymbolTableEntry::Visibility::None, nullptr, node);

		func_table->add_entry(temporary_variable_entry);
	}
}
string CodeGenerator::get_register() {
	string reg = registers.top();
	registers.pop();
	return reg;
}

void CodeGenerator::give_register(std::string reg) {
	registers.push(reg);
}

void CodeGenerator::generate_code(AST* node, const SymbolTableEntry* function_entry, const SymbolTable* class_table, const SymbolTable* dot_class_table) {
	const string& node_type = node->type;
	const vector<AST*>& children = node->children;
	if (node_type == Statement) {
		AST* stat_type_node = get_type(children, StatType);
		string stat_type = stat_type_node->value;
		if (stat_type == "write") {
			string r1 = get_register();
			
			string r2 = get_register();

			AST* child = children[1];
			//SymbolTableEntry* child_entry = child->entry;
			//moon_file << "lw r1," << child_entry->offset << "(r14)" << endl;
			generate_code(child, function_entry, class_table, dot_class_table);
			const int stack_frame_size = function_entry->get_scope_size();
			moon_file << "addi r14,r14," << stack_frame_size << endl;
			moon_file << "sw -8(r14),r1" << endl;
			moon_file << "addi r1,r0, buf" << endl;
			moon_file << "sw -12(r14),r1" << endl;
			moon_file << "jl r15, intstr" << endl;
			moon_file << "sw -8(r14),r13" << endl;
			moon_file << "jl r15, putstr" << endl;
			moon_file << "addi r14,r14," << stack_frame_size << endl;
		}
		else if (stat_type == "while" || stat_type == "if") {

		}
		else if (stat_type == "read") {

		}
		else if (stat_type == "return") {

		}
	}
}

int CodeGenerator::get_memory(SymbolTableEntry* entry) {
	if (entry->scope_size >= 0) return entry->scope_size;
	switch (entry->kind) {
	case EntryKind::Data:
	case EntryKind::Localvar:
	{
		int size = get_size(*entry->type);
		entry->scope_size = size;
		return size;
	}
	case EntryKind::Class:
	{
		int scope_size = 0;
		SymbolTableClassEntry* class_entry = static_cast<SymbolTableClassEntry*>(entry);

		get_children_sizes(class_entry->link->entries, scope_size);
		get_children_sizes(class_entry->inherited_members, scope_size);

		entry->scope_size = scope_size;
		return scope_size;
	}
	case EntryKind::FuncDef: 
	{
		int scope_size = 0;
		get_children_sizes(entry->link->entries, scope_size);
		int temp_count = 1;
		generate_temporary_function_variables(entry->node, scope_size, temp_count, entry->link);

		entry->scope_size = scope_size;
		return scope_size;
	}
	default:
		return 0;
	}
}

int CodeGenerator::get_size(SymbolType type) {
	int type_size = get_memory(type.type_id);
	for (int dim : type.dimensions) type_size *= dim;
	return type_size;
}