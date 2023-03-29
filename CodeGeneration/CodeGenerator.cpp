#include "CodeGenerator.h"
#include "../Semantic/ASTGenerator.h"
#include "../Utils/utils.h"

using namespace std;
typedef SymbolTableEntry::Kind EntryKind;
using namespace ASTConstants;
using namespace ast;

CodeGenerator::CodeGenerator(string filename, AST* root, SymbolTable* global_table) : moon_file("output/outcode/" + filename + ".moon"), root(root), global_table(global_table) {
	for (int i = 13; i >= 1; i--) registers.push("r" + to_string(i));
}

void CodeGenerator::generate() {
	get_memory_blocks();
	generate_code();
}

void CodeGenerator::generate_code() {
	generate_code(root, nullptr, nullptr, nullptr);
}

void CodeGenerator::get_memory_blocks() {
	for (auto& entry : global_table->entries) {
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
		EntryKind kind = child_entry->kind;

		child_entry->offset = scope_size;
		scope_size -= get_memory(child_entry);
	}
}
// write(1 + 1);

void CodeGenerator::generate_temporary_function_variables(AST* node, int& scope_size, int& temp_count, SymbolTable* func_table) {
	for (AST* child_node : node->children) generate_temporary_function_variables(child_node, scope_size, temp_count, func_table);

	if (node->type == Operation || node->type == Variable || node->type == IntNum || node->type == FloatNum) {
		int size = get_size(node->decorator.get_type());
		string name = "t" + temp_count++;
		string unique_id = '_' + name;
		EntryKind kind = EntryKind::TempVar;
		SymbolType* type = new SymbolType(node->decorator.get_type());
		SymbolTableEntry* temporary_variable_entry = new SymbolTableEntry(unique_id, name, kind, type, -1, SymbolTableEntry::Visibility::None, nullptr, node, scope_size);
		scope_size -= size;
		
		func_table->add_entry(temporary_variable_entry);
	}
}
string CodeGenerator::get_register() {
	string reg = registers.top();
	registers.pop();
	return reg;
}

void CodeGenerator::free_register(std::string reg) {
	registers.push(reg);
}
void CodeGenerator::integer_add(string destination_register, string register_operand, string integer_operand) {
	write_instruction("addi " + destination_register + "," + register_operand + "," + integer_operand);
}

void CodeGenerator::jump_and_link(string destination_register, string address) {
	write_instruction("jl " + destination_register + "," + address);
}

void CodeGenerator::generate_code(AST* node, const SymbolTableEntry* function_entry, const SymbolTable* class_table, const SymbolTable* dot_class_table) {
	const string& node_type = node->type;
	const vector<AST*>& children = node->children;
	SymbolTableEntry* node_entry = node->entry;
	if (node_type == Statement) {
		SymbolTable* function_table = function_entry->link;
		const string stack_frame_size = function_table->get_scope_size();

		AST* stat_type_node = get_type(children, StatType);
		string stat_type = stat_type_node->value;
		if (stat_type == "write") {
			string r1 = get_register();
			AST* child = children[1];

			generate_code(child, function_entry, class_table, dot_class_table);
			
			SymbolTableEntry* child_entry = child->entry;

			write_instruction("");
			load_word(child_entry, r1);
			write_instruction("addi r14,r14," + stack_frame_size);
			store_word("-8", r1);
			write_instruction("addi " + r1 + ", r0, buf");
			store_word("-12", r1);
			write_instruction("jl r15, intstr");
			store_word("-8", "r13");
			write_instruction("jl r15, putstr");
			write_instruction("subi r14,r14," + stack_frame_size);

			// New line
			integer_add("r3", "r0", "nl");
			store_word("-8", "r3");
			jump_and_link("r15", "putstr");

			free_register(r1);
		}
		else if (stat_type == "while" || stat_type == "if") {

		}
		else if (stat_type == "read") {

		}
		else if (stat_type == "return") {

		}
	}
	else if (node_type == Operation) {
		string r1 = get_register();
		string r2 = get_register();

		AST* left = children[0];
		AST* right = children[2];

		generate_code(left, function_entry, class_table, dot_class_table);
		generate_code(right, function_entry, class_table, dot_class_table);

		SymbolTableEntry* left_entry = left->entry;
		SymbolTableEntry* right_entry = right->entry;

		AST* op_type_node = get_type(children, OpType);
		string op_type = op_type_node->value;
		string op_instruction = "add";
		if (op_type == "+") {
			op_instruction = "add";
		}
		else if (op_type == "-") {
			op_instruction = "sub";
		}
		else if (op_type == "*") {
			op_instruction = "mul";
		}
		else if (op_type == "/") {
			op_instruction = "div";
		}

		write_instruction("");
		load_word(left_entry, r1);
		load_word(right_entry, r2);
		write_instruction(op_instruction + " " + r2 + "," + r1 + "," + r2);

		const string node_offset = get_offset(node_entry);
		store_word(node_offset, r2);

		free_register(r1);
		free_register(r2);
	}
	// TODO factor
	else if (node_type == IntNum) {
		write_instruction("");
		string r = get_register();
		write_instruction("addi " + r + ",r0," + node->value);

		const string node_offset = get_offset(node_entry);
		store_word(node_offset, r);
		free_register(r);
	}
	else if (node_type == FuncDef) {
		SymbolTable* table = node_entry->link;
		bool is_main = node_entry->name == "main";
		string r1 = get_register();
		if (is_main) {
			write_instruction("entry");
			write_instruction("addi r14,r0,topaddr");
		}
		else {

		}

		for (AST* child_node : node->children) generate_code(child_node, node_entry, class_table, dot_class_table);

		if (is_main) {
			write_instruction("hlt");
			write_instruction("nl", "db 13, 10, 0");
			write_instruction("buf", "res 20");
		}
		else {
			/*
			int return_offset = get_offset(table, "_return");
			
			write_instruction("lw " + r1 + "," + return);
			*/

			string jump_offset = get_offset(table, "_jump");
			write_instruction("lw r15," + jump_offset + "(r14)");
			write_instruction("jr r15");
		}
		free_register(r1);
	}
	else {
		for (AST* child_node : node->children) generate_code(child_node, function_entry, class_table, dot_class_table);
	}
}

std::string CodeGenerator::get_offset(SymbolTableEntry* entry) {
	return to_string(entry->offset);
}

std::string CodeGenerator::get_offset(SymbolTable* table, std::string unique_id) {
	SymbolTableEntry* entry = table->get_entry(unique_id);
	return to_string(entry->offset);
}
void CodeGenerator::store_word(string destination_offset, std::string origin_register) {
	write_instruction("sw " + destination_offset + +"(r14)," + origin_register);
}

void CodeGenerator::load_word(SymbolTableEntry* entry, string destination_register) {
	write_instruction("lw " + destination_register + "," + to_string(entry->offset) + "(r14)");
}

void CodeGenerator::write_instruction(std::string start, std::string instruction) {
	moon_file << start << '\t' << instruction << endl;
}

void CodeGenerator::write_instruction(std::string instruction) {
	write_instruction("", instruction);
}

int CodeGenerator::get_memory(SymbolTableEntry* entry) {
	if (entry->m_size <= 0) return entry->m_size;
	SymbolTable* current_table = entry->link;
	switch (entry->kind) {
	case EntryKind::Data:
	case EntryKind::Localvar:
	{
		int size = get_size(*entry->type);
		entry->m_size = size;
		return size;
	}
	case EntryKind::Class:
	{
		int scope_size = 0;
		SymbolTableClassEntry* class_entry = static_cast<SymbolTableClassEntry*>(entry);

		// this computes the scope size of methods as well :)
		get_children_sizes(current_table->entries, scope_size);
		get_children_sizes(class_entry->inherited_members, scope_size);

		entry->m_size = scope_size;
		current_table->scope_size = scope_size;
		return scope_size;
	}
	case EntryKind::FuncDef: 
	{
		bool is_main = entry->name == "main";

		int scope_size = is_main ? -4 : 0;	
		get_children_sizes(current_table->entries, scope_size);
		int temp_count = 1;
		generate_temporary_function_variables(entry->node, scope_size, temp_count, current_table);

		if (!is_main) {
			SymbolTableEntry* jump_entry = new SymbolTableEntry("_jump", "jump", EntryKind::Jump, new SymbolType("integer", vector<int>()), -1, SymbolTableEntry::Visibility::None, nullptr, nullptr, scope_size);
			scope_size -= 4;

			SymbolType return_type = SymbolType(*entry->type);
			SymbolTableEntry* return_entry = new SymbolTableEntry("_return", "return", EntryKind::Jump, new SymbolType(return_type), -1, SymbolTableEntry::Visibility::None, nullptr, nullptr);

			scope_size -= get_size(return_type);

			// TODO: also pass class ref

			current_table->add_entry(jump_entry);
			current_table->add_entry(return_entry);
		}

		entry->m_size = 0;
		current_table->scope_size = scope_size;
		return 0;
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