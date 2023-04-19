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
	moon_file.close();

	std::cout << "Code generation complete." << std::endl;
	return;
}

void CodeGenerator::generate_code() {
	generate_code(root, nullptr, nullptr, nullptr);

	write_instruction("ent", "db \"Enter input: \", 0");
	write_instruction("nl", "db 13, 10, 0");
	write_instruction("buf", "res 20");
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
	get_children_sizes(entries, scope_size, nullptr);
}


void CodeGenerator::get_children_sizes(unordered_map <std::string, SymbolTableEntry*> entries, int& scope_size, SymbolType* class_type) {
	for (const auto& child : entries) {
		SymbolTableEntry* child_entry = child.second;
		EntryKind kind = child_entry->kind;

		child_entry->offset = scope_size;
		scope_size -= abs(get_memory(child_entry, class_type));
	}
}

void CodeGenerator::generate_temporary_function_variables(AST* node, int& scope_size, int& temp_count, SymbolTable* func_table) {
	for (AST* child_node : node->children) generate_temporary_function_variables(child_node, scope_size, temp_count, func_table);
	string node_type = node->type;

	if (node_type == Operation || node_type == Variable || node_type == IntNum || node_type == FloatNum || node_type == FuncCall || node_type == Dot || node_type == Factor) {
		int size = 4;
		if(node_type == FuncCall) size = get_size(node->decorator.get_type());
		string name = "t" + to_string(temp_count++);
		string unique_id = '_' + name;
		EntryKind kind = EntryKind::TempVar;
		SymbolType* type = new SymbolType(node->decorator.get_type());
		SymbolTableEntry* temporary_variable_entry = new SymbolTableEntry(unique_id, name, kind, type, -1, SymbolTableEntry::Visibility::None, nullptr, node, scope_size);
		scope_size -= size;
		
		func_table->add_entry(temporary_variable_entry);
	}
	else if (node->type == Condition) {
		node->entry = node->children[0]->entry;
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

void CodeGenerator::register_add(string destination_register, string r1, string r2) {
	write_instruction("add " + destination_register + "," + r1 + "," + r2);
}

void CodeGenerator::jump_and_link(string destination_register, string address) {
	write_instruction("jl " + destination_register + "," + address);
}

string CodeGenerator::get_branch() {
	return "b" + to_string(branch_count++);
}

SymbolTableEntry* CodeGenerator::generate_code(AST* node, const SymbolTableEntry* function_scope, const SymbolTable* class_table, SymbolTableEntry* dot_entry) {
	const string& node_type = node->type;
	const vector<AST*>& children = node->children;
	SymbolTableEntry* node_entry = node->entry;
	if (class_table && !function_scope) return nullptr;
	if (node_type == Statement) {
		// needs to be in here because function_scope might be null at root
		SymbolTable* function_table = function_scope->link;
		
		const string stack_frame_size = function_table->get_scope_size();

		AST* stat_type_node = get_type(children, StatType);
		string stat_type = stat_type_node->value;
		if (stat_type == "write") {
			
			AST* child = children[1];

			
			
			SymbolTableEntry* child_entry = generate_code(child, function_scope, class_table, dot_entry);

			string r1 = get_register();
			write_empty_line();
			write_comment("write statement");
			load_value(child_entry, r1);
			write_instruction("addi r14,r14," + stack_frame_size);
			store_word("-8", r1);
			write_instruction("addi " + r1 + ", r0, buf");
			store_word("-12", r1);
			write_instruction("jl r15, intstr");
			store_word("-8", "r13");
			write_instruction("jl r15, putstr");

			// New line
			write_empty_line();
			write_comment("new line");
			integer_add(r1, "r0", "nl");
			store_word("-8", r1);
			jump_and_link("r15", "putstr");
			write_instruction("subi r14,r14," + stack_frame_size);

			free_register(r1);
		}
		else if (stat_type == "if") {
			
			AST* condition_node = get_type(children, Condition);
			vector<AST*> stat_blocks = get_types(children, StatBlock);
			AST* then_block = stat_blocks[0];
			AST* else_block = stat_blocks[1];
			SymbolTableEntry* condition_entry = generate_code(condition_node, function_scope, class_table, dot_entry);

			string r = get_register();

			string else_branch = get_branch();
			string end_branch = get_branch();

			write_comment("if statement");
			load_value(condition_entry, r);
			write_instruction("bz " + r + "," + else_branch);
			generate_code(then_block, function_scope, class_table, dot_entry);
			jump(end_branch);
			write_instruction(else_branch, "");
			generate_code(else_block, function_scope, class_table, dot_entry);
			write_instruction(end_branch, "");
			write_empty_line();

			free_register(r);
		}
		else if (stat_type == "while") {
			AST* condition_node = get_type(children, Condition);
			AST* stat_block = get_type(children, StatBlock);

			string go_branch = get_branch();
			string end_branch = get_branch();
			write_comment("while loop");
			write_instruction(go_branch, "");

			SymbolTableEntry* condition_entry = generate_code(condition_node, function_scope, class_table, dot_entry);
			string r = get_register();

			load_value(condition_entry, r);
			write_instruction("bz " + r + "," + end_branch);
			generate_code(stat_block, function_scope, class_table, dot_entry);
			jump(go_branch);
			write_instruction(end_branch, "");
		
			free_register(r);
		}
		else if (stat_type == "read") {
			const string r = get_register();

			SymbolTableEntry* variable_entry = generate_code(children[1], function_scope, class_table, dot_entry);

			write_comment("read statement");
			write_instruction("addi r14,r14," + stack_frame_size);

			integer_add(r, "r0", "ent");
			store_word("-8", r);
			jump_and_link("r15", "putstr");

			integer_add(r, "r0", "buf");
			store_word("-8", r);
			jump_and_link("r15", "getstr");
			jump_and_link("r15", "strint");

			write_instruction("subi r14,r14," + stack_frame_size);

			load_word(variable_entry, r);
			write_instruction("sw 0(" + r + "),r13");

			free_register(r);
		}
		else if (stat_type == "return") {
			SymbolTableEntry* return_value_entry = generate_code(children[1], function_scope, class_table, dot_entry);
			SymbolTableEntry* return_entry = function_table->get_entry("_return");
			string r = get_register();

			write_empty_line();
			write_comment("store return value into return register");
			if (return_value_entry->is_reference) {
				load_word(return_value_entry, r);
				store_word(return_entry->get_offset(), r);
			}
			else {
				integer_add(r, "r0", return_value_entry->get_offset());
				register_add(r, r, "r14");
				store_word(return_entry->get_offset(), r);
			}


			//return_entry->is_reference = return_value_entry->is_reference;

			free_register(r);
		}
	}
	else if (node_type == Condition) {
		return generate_code(children[0], function_scope, class_table, dot_entry);
	}
	// always returns the ABSOLUTE address of the variable its referring to
	else if (node_type == Variable) {
		SymbolTable* function_table = function_scope->link;
		AST* id_node = get_type(node->children, Id);
		std::string variable_name = id_node->value;

		if (variable_name == "self") {
			SymbolTableEntry* self = function_table->get_entry("_self");
			self->is_reference = true;
			return self;
		}

		write_comment(variable_name + " to " + node_entry->name);

		SymbolTableEntry* variable_entry = nullptr;

		string r = get_register();

		if (dot_entry) {
			SymbolTableEntry* dot_class_entry = global_table->get_entry(dot_entry->type->type_id);
			SymbolTable* dot_class_table = dot_class_entry->link;
			SymbolTableEntry* data_member_entry = dot_class_table->get_entry(variable_name);

			
			load_word(dot_entry, r);
			integer_add(r, r, data_member_entry->get_offset());
			variable_entry = data_member_entry;
		}
		else {
			variable_entry = function_table->get_entry(variable_name);

			if (variable_entry) {
				if (variable_entry && variable_entry->kind == EntryKind::Parameter && !variable_entry->type->is_basic_type()) {
					load_word(variable_entry, r);
				}
				else {
					integer_add(r, "r0", variable_entry->get_offset());
					register_add(r, r, "r14");
				}
			}
			else {
				SymbolTableEntry* object_entry = function_table->get_entry("_self");
				SymbolTable* object_class_table = object_entry->link;
				variable_entry = object_class_table->get_entry(variable_name);

				load_word(object_entry, r);
				//register_add(r, r, "r14");
				integer_add(r, r, variable_entry->get_offset());
			}
		}

		const AST* dim_list = get_type(children, DimList);

		SymbolType variable_type(variable_entry->type ? *variable_entry->type : variable_entry->node->decorator.get_type());

		vector<int> variable_dimensions = variable_type.dimensions;

		const vector<AST*>& dimensions = dim_list->children;

		
		string multipler_reg = get_register();
		string offset_reg = get_register();
		integer_add(offset_reg, "r0", "0");
		integer_add(multipler_reg, "r0", "-4");

		for (int i = variable_dimensions.size() - 1; i >= 0; i--) {

			if (dimensions.size() > i) {
				SymbolTableEntry* dimension_entry = generate_code(dimensions[i], function_scope, class_table, nullptr);
				string local_reg = get_register();

				load_value(dimension_entry, local_reg);
				write_instruction("mul " + local_reg + "," + local_reg + "," + multipler_reg);
				register_add(offset_reg, offset_reg, local_reg);

				free_register(local_reg);
			}
			write_instruction("muli " + multipler_reg + "," + multipler_reg + "," + to_string(variable_dimensions[i]));
		}

		register_add(r, r, offset_reg);
		store_word(node_entry->get_offset(), r);
		node_entry->is_reference = true;

		free_register(r);
		free_register(offset_reg);
		free_register(multipler_reg);
		return node_entry;
	}

	else if (node_type == FuncCall) {

		AST* id = get_type(children, Id);

		string func_id = id->value;

		AST* param_list = get_type(children, AParamList);

		vector<SymbolType> argument_types;
		vector<SymbolTableEntry*> argument_entries;
		for (AST* param : param_list->children) {
			SymbolTableEntry* argument_entry = generate_code(param, function_scope, class_table, dot_entry);
			argument_entries.push_back(argument_entry);

			SymbolType param_type = param->decorator.get_type();
			argument_types.push_back(param_type);
		}

		const string signature = FunctionSymbolType::get_signature(argument_types);

		const string unique_id = func_id + signature;

		SymbolTableEntry* function_entry = nullptr;

		SymbolTableEntry* self_argument = nullptr;

		if (dot_entry) {
			SymbolType dot_type = *dot_entry->type;

			SymbolTableEntry* class_entry = global_table->get_entry(dot_type.type_id);

			function_entry = class_entry->link->get_entry(unique_id);

			self_argument = dot_entry;
			// Maybe pushback dot_entry??
			//argument_entries.push_back(dot_entry);
		}
		else {
			SymbolTableEntry* object_entry = function_scope->link->get_entry("_self");


			if (object_entry) {
				SymbolTable* object_class_table = object_entry->link;
				function_entry = object_class_table->get_entry(unique_id);
				self_argument = object_entry;
			}
			
			if(!function_entry) function_entry = global_table->get_entry(unique_id);
		}

		SymbolTable* calling_function_table = function_scope->link;

		const string stack_frame_size = calling_function_table->get_scope_size();
		const int scope_size = calling_function_table->scope_size;

		std::string short_name = get_short_function_name(function_entry->unique_id);

		FunctionSymbolTable* jumping_function_table = static_cast<FunctionSymbolTable*>(function_entry->link);

		const vector<SymbolTableEntry*>& parameters = jumping_function_table->parameters;

		const string argument_reg = get_register();

		for (int i = 0; i < argument_entries.size(); i++) {
			SymbolTableEntry* argument = argument_entries[i];
			SymbolTableEntry* parameter = parameters[i];

			if (argument->type->is_basic_type()) load_value(argument, argument_reg);
			else load_word(argument, argument_reg);
			const int parameter_offset = parameter->offset + scope_size;
			store_word(to_string(parameter_offset), argument_reg);
		}

		SymbolTableEntry* self_entry = jumping_function_table->get_entry("_self");

		if (self_entry) {
			load_word(self_argument, argument_reg);
			const int parameter_offset = self_entry->offset + scope_size;
			store_word(to_string(parameter_offset), argument_reg);
		}

		free_register(argument_reg);

		const string return_register = get_register();
		
		integer_add("r14", "r14", stack_frame_size);
		write_instruction("jl r15," + short_name);

		SymbolTableEntry* return_entry = jumping_function_table->get_entry("_return");
		load_word(return_entry, return_register);

		write_instruction("subi r14,r14," + stack_frame_size);

		copy_values(node_entry, return_register, return_entry);

		free_register(return_register);

		return node_entry;
	}
	else if (node_type == VarDecl && function_scope) {
		AST* argument_list_node = get_type(children, AParamList);
		if (!argument_list_node) return nullptr;
		//AST* type_node = get_type(children, Type);
		AST* id_node = get_type(children, Id);

		const string var_id = id_node->value;
		SymbolTableEntry* self_argument = function_scope->link->get_entry(var_id);

		SymbolTable* calling_function_table = function_scope->link;
		const string class_name = node_entry->type->type_id;
		SymbolTable* class_table = global_table->get_entry(class_name)->link;

		vector<SymbolType> argument_types;
		vector<SymbolTableEntry*> argument_entries;
		for (AST* param : argument_list_node->children) {
			SymbolTableEntry* argument_entry = generate_code(param, function_scope, class_table, dot_entry);
			argument_entries.push_back(argument_entry);

			SymbolType param_type = param->decorator.get_type();
			argument_types.push_back(param_type);
		}

		const string signature = FunctionSymbolType::get_signature(argument_types);

		const string unique_id = class_name + signature;

		SymbolTableEntry* function_entry = class_table->get_entry(unique_id);

		const string stack_frame_size = calling_function_table->get_scope_size();
		const int scope_size = calling_function_table->scope_size;

		std::string short_name = get_short_function_name(function_entry->unique_id);

		FunctionSymbolTable* jumping_function_table = static_cast<FunctionSymbolTable*>(function_entry->link);

		const vector<SymbolTableEntry*>& parameters = jumping_function_table->parameters;

		const string argument_reg = get_register();

		for (int i = 0; i < argument_entries.size(); i++) {
			SymbolTableEntry* argument = argument_entries[i];
			SymbolTableEntry* parameter = parameters[i];

			if (argument->type->is_basic_type()) load_value(argument, argument_reg);
			else load_word(argument, argument_reg);
			const int parameter_offset = parameter->offset + scope_size;
			store_word(to_string(parameter_offset), argument_reg);
		}

		SymbolTableEntry* self_entry = jumping_function_table->get_entry("_self");

		if (self_entry) {
			integer_add(argument_reg, "r0", self_argument->get_offset());
			register_add(argument_reg, argument_reg, "r14");
			const int parameter_offset = self_entry->offset + scope_size;
			store_word(to_string(parameter_offset), argument_reg);
		}

		free_register(argument_reg);

		integer_add("r14", "r14", stack_frame_size);
		write_instruction("jl r15," + short_name);

		write_instruction("subi r14,r14," + stack_frame_size);
	}
	else if (node_type == Dot) {
		AST* left = children[0];
		AST* right = children[1];

		SymbolTableEntry* left_entry = generate_code(left, function_scope, class_table, dot_entry);
		SymbolTableEntry* dot_entry = left_entry;
		if (!dot_entry->is_reference) {
			const string r = get_register();
			integer_add(r, "r0", left_entry->get_offset());
			register_add(r, r, "r14");
			store_word(node_entry->get_offset(), r);
			dot_entry = node_entry;
		}
		SymbolTableEntry* right_entry = generate_code(right, function_scope, class_table, dot_entry);
		return right_entry;
	}
	else if (node_type == Dim) {
		return generate_code(children[0], function_scope, class_table, nullptr);
	}
	else if (node_type == AssignStat) {
		AST* left = children[0];
		AST* right = children[1];

		SymbolTableEntry* right_entry = generate_code(right, function_scope, class_table, dot_entry);
		SymbolTableEntry* left_entry = generate_code(left, function_scope, class_table, dot_entry);

		string r = get_register();

		write_empty_line();
		write_comment("Assignment", left->line_start);
		load_value(right->entry, r);
		store_into_reference(left_entry, r);
		write_empty_line();
		free_register(r);

	}
	else if (node_type == Factor) {
		string r = get_register();

		SymbolTableEntry* number_entry = generate_code(children[1], function_scope, class_table, dot_entry);
		load_value(number_entry, r);
		AST* sign_node = get_type(children, Sign);
		string sign = sign_node->value;
		if (sign == "-") {
			write_instruction("sub " + r + ",r0," + r);
		}
		else if (sign == "not") {
			write_instruction("not " + r + "," + r);
		}

		store_word(node_entry->get_offset(), r);
		node_entry->is_reference = false;
		free_register(r);
		return node_entry;
	}
	else if (node_type == Operation) {
		string r1 = get_register();
		string r2 = get_register();

		AST* left = children[0];
		AST* right = children[2];

		SymbolTableEntry* left_entry = generate_code(left, function_scope, class_table, dot_entry);
		SymbolTableEntry* right_entry = generate_code(right, function_scope, class_table, dot_entry);

		AST* op_type_node = get_type(children, OpType);
		string op_type = op_type_node->value;
		string op_instruction = get_op_instruction(op_type);


		write_instruction("");
		load_value(left_entry, r1);
		load_value(right_entry, r2);
		write_instruction(op_instruction + " " + r2 + "," + r1 + "," + r2);

		const string node_offset = get_offset(node_entry);
		store_word(node_offset, r2);

		free_register(r1);
		free_register(r2);
		node->entry->is_reference = false;
		return node_entry;
	}
	else if (node_type == IntNum) {
		write_instruction("");
		string r = get_register();
		write_instruction("addi " + r + ",r0," + node->value);

		const string node_offset = get_offset(node_entry);
		store_word(node_offset, r);
		free_register(r);
		node->entry->is_reference = false;
		return node_entry;
	}
	else if (node_type == Class) {
		SymbolTable* node_table = node_entry->link;
		for (AST* child_node : node->children) generate_code(child_node, nullptr, node_table, nullptr);
	}
	else if (node_type == FuncDef) {
		write_comment("Function definition of " + node->entry->unique_id);
		SymbolTable* func_table = node_entry->link;
		bool is_main = node_entry->name == "main" && !class_table;
		string r1 = get_register();
		if (is_main) {
			write_instruction("entry");
			write_instruction("addi r14,r0,topaddr");
		}
		else {
			string short_name = get_short_function_name(node_entry->unique_id);
			SymbolTableEntry* jump_entry = func_table->get_entry("_jump");

			write_instruction(short_name, "sw " + jump_entry->get_offset()+ "(r14),r15");
		}

		for (AST* child_node : node->children) {
			generate_code(child_node, node_entry, class_table, nullptr);
		}

		if (is_main) {
			write_instruction("hlt");
		}
		else {
			/*
			int return_offset = get_offset(table, "_return");
			
			write_instruction("lw " + r1 + "," + return);
			*/

			string jump_offset = get_offset(func_table, "_jump");
			write_instruction("lw r15," + jump_offset + "(r14)");
			write_instruction("jr r15");
		}
		free_register(r1);
	}
	else {
		for (AST* child_node : node->children) generate_code(child_node, function_scope, class_table, dot_entry);
	}

	return nullptr;
}

std::string CodeGenerator::get_short_function_name(string unique_id) {
	if (!func_entry_to_short_name.count(unique_id)) {
		func_entry_to_short_name.insert({ unique_id, "f" + to_string(function_count++)});
	}
	return func_entry_to_short_name.at(unique_id);
}



std::string CodeGenerator::get_op_instruction(std::string op_type) {
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
	else if (op_type == "and") {
		op_instruction = "and";
	}
	else if (op_type == "or") {
		op_instruction = "or";
	}
	// TODO ???
	else if (op_type == "not") {
		op_instruction = "not";
	}
	else if (op_type == "==") {
		op_instruction = "ceq";
	}
	else if (op_type == "<>") {
		op_instruction = "cne";
	}
	else if (op_type == "<") {
		op_instruction = "clt";
	}
	else if (op_type == "<=") {
		op_instruction = "cle";
	}
	else if (op_type == ">") {
		op_instruction = "cgt";
	}
	else if (op_type == ">=") {
		op_instruction = "cge";
	}
	return op_instruction;
}

std::string CodeGenerator::get_offset(SymbolTableEntry* entry) {
	return to_string(entry->offset);
}

std::string CodeGenerator::get_offset(SymbolTable* table, std::string unique_id) {
	SymbolTableEntry* entry = table->get_entry(unique_id);
	return to_string(entry->offset);
}
void CodeGenerator::jump(std::string address) {
	write_instruction("j " + address);
}

void CodeGenerator::copy_values(SymbolTableEntry* destination_entry, std::string origin_register, SymbolTableEntry* origin_reference_entry) {
	/*
	if (!origin_reference_entry->is_reference) {
		store_word(destination_entry->get_offset(), origin_register);
		return;
	}
	*/

	int current_word = 0;

	const int return_size = abs(get_size(*origin_reference_entry->type));

	const string destination_register = get_register();
	integer_add(destination_register, "r0", destination_entry->get_offset());
	register_add(destination_register, destination_register, "r14");
	//load_word(destination_entry, destination_register);

	const string value_register = get_register();

	while (return_size > current_word)
	{
		write_instruction("lw " + value_register + ",0(" + origin_register + ")");
		write_instruction("sw 0(" + destination_register + ")," + value_register);
		integer_add(destination_register, destination_register, "-4");
		integer_add(origin_register, origin_register, "-4");
		current_word += 4;
	}

	free_register(value_register);
	free_register(destination_register);
}

void CodeGenerator::store_word(string destination_offset, std::string origin_register) {
	write_instruction("sw " + destination_offset + +"(r14)," + origin_register);
}

void CodeGenerator::load_word(SymbolTableEntry* entry, string destination_register) {
	write_instruction("lw " + destination_register + "," + to_string(entry->offset) + "(r14)");
}

void CodeGenerator::store_into_reference(SymbolTableEntry* entry_with_reference, string origin_register) {
	string ref_register = get_register();

	load_word(entry_with_reference, ref_register);
	write_instruction("sw 0(" + ref_register + ")," + origin_register);

	free_register(ref_register);
}

void CodeGenerator::load_value(SymbolTableEntry* entry, string destination_register) {
	load_word(entry, destination_register);
	if (entry->is_reference) {
		write_instruction("lw " + destination_register + "," + "0(" + destination_register + ")");
	}
}

void CodeGenerator::write_comment(string comment) {
	moon_file << "% " << comment << endl;
}

void CodeGenerator::write_comment(string comment, int line) {
	write_comment(comment + " (line " + to_string(line) + ")");
}

void CodeGenerator::write_instruction(std::string start, std::string instruction, std::string comment) {
	moon_file << start << '\t' << instruction;
	if(comment.size()) moon_file << "\t\t\t" << "% " + comment;
	moon_file << endl;
}
void CodeGenerator::write_instruction(std::string start, std::string instruction) {
	write_instruction(start, instruction, "");
}

void CodeGenerator::write_instruction(std::string instruction) {
	write_instruction("", instruction);
}
void CodeGenerator::write_empty_line() {
	write_instruction("");
}

int CodeGenerator::get_memory(SymbolTableEntry* entry) {
	return get_memory(entry, nullptr);
}

int CodeGenerator::get_memory(SymbolTableEntry* entry, SymbolType* class_type) {
	if (entry->m_size <= 0) return entry->m_size;
	SymbolTable* current_table = entry->link;
	switch (entry->kind) {
	case EntryKind::Parameter:
	{
		entry->m_size = 4;
		return 4;
	}
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
		get_children_sizes(current_table->entries, scope_size, class_entry->type);
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
		AST* stat_block_node = get_type(entry->node->children, StatBlock);

		generate_temporary_function_variables(stat_block_node, scope_size, temp_count, current_table);

		if (!is_main) {
			SymbolTableEntry* jump_entry = new SymbolTableEntry("_jump", "jump", EntryKind::Jump, new SymbolType("integer", vector<int>()), -1, SymbolTableEntry::Visibility::None, nullptr, nullptr, scope_size);
			scope_size -= 4;

			SymbolType return_type = SymbolType(*entry->type);
			SymbolTableEntry* return_entry = new SymbolTableEntry("_return", "return", EntryKind::Jump, new SymbolType(return_type), -1, SymbolTableEntry::Visibility::None, nullptr, nullptr, scope_size);

			scope_size -= 4;//get_size(return_type);

			if (entry->scope) {

				SymbolTableEntry* class_entry = global_table->get_entry(*entry->scope);
				SymbolTable* class_table = class_entry->link;
				SymbolTableEntry* self_entry = new SymbolTableEntry("_self", "self", EntryKind::TempVar, new SymbolType(class_entry->unique_id, vector<int>()), -1, SymbolTableEntry::Visibility::None, class_table, nullptr, scope_size);
				scope_size -= 4;
				current_table->add_entry(self_entry);
			}

			
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
	return abs(type_size);
}