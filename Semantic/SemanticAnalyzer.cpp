#include "SemanticAnalyzer.h"
#include "ASTGenerator.h"
#include <algorithm>
#include <sstream>
#include "../Utils/utils.h"
#include "SymbolTable.h"
#include "../CodeGeneration/CodeGenerator.h"


using std::string;
typedef SymbolTableEntry STE;
using std::cout;
using std::endl;
using std::vector;
using std::to_string;
using std::unordered_set;
using std::unordered_map;
using std::stringstream;
typedef SymbolTableEntry::Kind EntryKind;

using namespace ast;
using namespace ASTConstants;

SymbolType stat_return(SymbolType type) {
	if (type == SymbolType::INVALID) return type;
	else return SymbolType::OK;
}

SemanticAnalyzer::SemanticAnalyzer(string filename) : filename(filename) {

}

SemanticAnalyzer::SemanticError::SemanticError(std::string text, int line_location) : text(text), line_location(line_location) {

}

void SemanticAnalyzer::add_error(std::string text, int line) {
	SemanticError error("Error: " + text, line);
	errors.push_back(error);
}

void SemanticAnalyzer::add_warning(std::string text, int line) {
	SemanticError warning("Warning: " + text, line);
	warnings.push_back(warning);
}

void SemanticAnalyzer::perform_semantic_checks(AST* root, SymbolTable* global_table) {
	check_function_names(global_table);
	check_types(root, global_table);
}

void SemanticAnalyzer::check_types(AST* root, SymbolTable* global_table) {
	vector<AST*> func_defs = get_types(root->children, FuncDef);

	for (AST* func_def : func_defs) {
		const SymbolTable* class_table = func_def->decorator.class_entry ? func_def->decorator.class_entry->link : nullptr;
		const string class_name = class_table ? class_table->name : "none";
		const SymbolType constructor_type(class_name, vector<int>());
		const SymbolTableEntry* function_entry = func_def->decorator.function_entry;

		
		if (!function_entry) {
			const AST* id_node = get_type(func_def->children, Id);
			add_error("Cannot perform a semantic analysis of " + id_node->value + " because of missing declaration(s) and/or missing definition(s).", func_def->line_start);
			continue;
		}
		AST* stat_block = get_type(func_def->children, StatBlock);
		SymbolType return_type = resolve_type(stat_block, function_entry, class_table, global_table);
		cout << function_entry->unique_id << " " << return_type << endl;
		if ((return_type != *function_entry->type) && (constructor_type != *function_entry->type) && !(return_type == SymbolType::OK && *function_entry->type == SymbolType::VOID)) {
			add_error("Missing valid return statement in function " + function_entry->unique_id, func_def->line_start);
			cout << *function_entry->type << endl;
		}
		/*
		cout << func_def->decorator.function_entry->unique_id << endl;
		for (AST* statement : stat_block->children) {
			if (statement->type != VarDecl) {
				cout << resolve_type(statement, function_entry, class_table, global_table) << endl;
			}
		}
		*/
	}
}

bool SemanticAnalyzer::same_type(SymbolType t1, SymbolType t2) {
	return t1 == t2 && t1 != SymbolType::INVALID;
}

SymbolType SemanticAnalyzer::resolve_type(AST* node, const SymbolTableEntry* function_entry, const SymbolTable* class_table, const SymbolTable* global_table) {
	return resolve_type(node, function_entry, class_table, global_table, nullptr);
}

SymbolType SemanticAnalyzer::resolve_type(AST* node, const SymbolTableEntry* function_entry, const SymbolTable* class_table, const SymbolTable* global_table, const SymbolTable* dot_class_table) {
	// why is function_entry sometimes nullptr
	SymbolTable* function_table = function_entry->link;

	vector<AST*> children = node->children;
	string node_type = node->type;
	const int line_start = node->line_start;

	if (node_type == VarDecl) {
		AST* id_node = get_type(node->children, Id);
		std::string variable_name = id_node->value;

		AST* type_node = get_type(node->children, Type);
		std::string type_name = type_node->value;

		SymbolTableEntry* variable_entry = function_table->get_entry(variable_name);

		if (!variable_entry) {
			add_error("Cannot declare variable of an non-existent class \"" + type_name + "\".", node->line_start);
			return SymbolType::INVALID;
		}

		const AST* dim_list = get_type(children, DimList);

		const AST* param_list = get_type(children, AParamList);

		vector<int> dimensions;

		if (dim_list) {
			
			for (AST* dim : dim_list->children) {
				SymbolType dim_type = resolve_type(dim, function_entry, class_table, global_table);
				if (dim_type != SymbolType::INTEGER) {
					add_error("Indices must be of integer type.", node->line_start);
					return SymbolType::INVALID;
				}
				AST* int_node = get_type(dim->children, IntNum);
				dimensions.push_back(int_node ? stoi(int_node->value) : -1);
			}
		}
		else if (param_list) {
			vector<SymbolType> parameters;
			for (AST* param : param_list->children) {
				SymbolType param_type = resolve_type(param, function_entry, class_table, global_table);
				parameters.push_back(param_type);
			}

			const string signature = FunctionSymbolType::get_signature(parameters);

			const string unique_id = type_name + signature;

			SymbolTableEntry* variable_class_entry = global_table->get_entry(type_name);
			SymbolTableEntry* constructor_entry = variable_class_entry->link->get_entry(unique_id);

			if (!constructor_entry) {
				add_error("There doesn't exist a constructor with signature " + signature + " for class " + type_name + ".", line_start);
				return SymbolType::INVALID;
			}
		}
		node->decorator.set_type(SymbolType(type_name, dimensions));

		return SymbolType::OK;
	}
	else if (node_type == Variable) {
		AST* id_node = get_type(node->children, Id);
		std::string variable_name = id_node->value;

		if (variable_name == "self") {
			add_error("Incorrect use of self.", line_start);
			return SymbolType::INVALID;
		}

		SymbolTableEntry* variable_entry = nullptr;

		if (dot_class_table) {
			variable_entry = dot_class_table->get_entry(variable_name);
		}
		else {
			variable_entry = function_table->get_entry(variable_name);

			if (!variable_entry && class_table) variable_entry = class_table->get_entry(variable_name);
		}

		if (variable_entry) {

			const AST* dim_list = get_type(children, DimList);

			vector<int> dimensions;
			for (AST* dim : dim_list->children) {
				SymbolType dim_type = resolve_type(dim, function_entry, class_table, global_table);
				//cout << "wtf " << dim_type;
				if (dim_type != SymbolType::INTEGER) {
					add_error("Indices must be of integer type.", node->line_start);
					return SymbolType::INVALID;
				}
				dimensions.push_back(-1);
			}
			

			SymbolType variable_type(*variable_entry->type);

			vector<int> variable_dimensions = variable_type.dimensions;

			for (int dimension : dimensions) {
				if (variable_dimensions.empty()) {
					stringstream ss;
					ss << "Variable " + variable_name + " of type " << variable_type << " does not support a " << dimensions.size() << "-dimensonal access.";
					add_error(ss.str(), node->line_start);
					return SymbolType::INVALID;
				}
				variable_dimensions.pop_back();
			}

			node->decorator.set_type(SymbolType(variable_type.type_id, variable_dimensions));
			return node->decorator.get_type();
		}
		else {
			add_error("Reference to undeclared variable " + variable_name + ".", node->line_start);
			return SymbolType::INVALID;
		}
	}
	else if (node_type == FuncCall) {

		AST* id = get_type(children, Id);

		string func_id = id->value;

		AST* param_list = get_type(children, AParamList);

		vector<SymbolType> parameters;
		for (AST* param : param_list->children) {
			SymbolType param_type = resolve_type(param, function_entry, class_table, global_table);
			parameters.push_back(param_type);
		}

		const string signature = FunctionSymbolType::get_signature(parameters);

		const string unique_id = func_id + signature;

		SymbolTableEntry* function_entry = nullptr;

		if (dot_class_table) {
			function_entry = dot_class_table->get_entry(unique_id);
		}
		else {
			if(class_table) function_entry = class_table->get_entry(unique_id);
			if (!function_entry) function_entry = global_table->get_entry(unique_id);
		}

		if (function_entry) {
			node->decorator.set_type(*function_entry->type);
			return *function_entry->type;
		}
		else {
			add_error("Reference to undeclared or undefined function " + unique_id + ".", node->line_start);
			return SymbolType::INVALID;
		}
	}
	else if (node_type == IntNum || node_type == FloatNum) {
		SymbolType type = node_type == IntNum ? SymbolType::INTEGER : SymbolType::FLOAT;
		node->decorator.set_type(type);
		return type;
	}
	else if (node_type == Factor) {
		SymbolType type = resolve_type(children[1], function_entry, class_table, global_table);
		node->decorator.set_type(type);
		return type;
	}
	else if (node_type == Operation) {
		SymbolType t1 = resolve_type(children[0], function_entry, class_table, global_table);
		SymbolType t2 = resolve_type(children[2], function_entry, class_table, global_table);

		if (t1 != SymbolType::FLOAT && t1 != SymbolType::INTEGER) {
			add_error("Cannot perform binary operations on types that aren't numbers.", node->line_start);
			return SymbolType::INVALID;
		}

		if (!same_type(t1, t2)) {
			add_error("Types are not the same on both sides of the operation.", node->line_start);
			return SymbolType::INVALID;
		}
		node->decorator.set_type(t1);
		return t1;
	}
	else if (node_type == AssignStat) {
		SymbolType t1 = resolve_type(children[0], function_entry, class_table, global_table);
		SymbolType t2 = resolve_type(children[1], function_entry, class_table, global_table);

		if (t1.dimensions.size()) {
			add_error("Cannot assign a value to an array.", node->line_start);
			return SymbolType::INVALID;
		}

		if (!same_type(t1, t2)) {
			add_error("Types are not the same on both sides of the assignment.", node->line_start);
			return SymbolType::INVALID;
		}
		return SymbolType::OK;

	} else if (node_type == Statement) {
		AST* stat_type_node = get_type(children, StatType);
		string stat_type = stat_type_node->value;

		if (stat_type == "while" || stat_type == "if") {
			AST* condition_node = get_type(children, Condition);
			resolve_type(condition_node, function_entry, class_table, global_table);

			vector<AST*> stat_blocks = get_types(children, StatBlock);

			SymbolType* return_type = new SymbolType(SymbolType::OK);

			for (AST* stat_block : stat_blocks) {
				SymbolType current_type = resolve_type(stat_block, function_entry, class_table, global_table);
				if (*return_type != SymbolType::INVALID && current_type != SymbolType::OK) {
					if (current_type != *return_type) {
						if (*return_type != SymbolType::OK) {
							add_error("Inconsistent return types in statement block.", node->line_start);
							return SymbolType::INVALID;
						}
						return_type = new SymbolType(current_type);
					}
				}
			}
			
			return *return_type;
		}
		else if (stat_type == "write" || stat_type == "read") {
			SymbolType type = resolve_type(children[1], function_entry, class_table, global_table);
			return stat_return(type);
		}
		else if (stat_type == "return") {
			SymbolType return_type = resolve_type(children[1], function_entry, class_table, global_table);
			if (return_type != *function_entry->type) {
				stringstream ss;
				ss << "Type mismatch. Returns " << return_type << ". Expected " << *function_entry->type << ".";
				add_error(ss.str(), node->line_start);
				return SymbolType::INVALID;
			}
			return return_type;
		}
	} else if (node_type == StatBlock) {
		SymbolType* return_type = new SymbolType(SymbolType::OK);

		for (AST* statement : node->children) {
			SymbolType current_type = resolve_type(statement, function_entry, class_table, global_table);
			if (*return_type != SymbolType::INVALID && current_type != SymbolType::OK) {
				if (current_type != *return_type) {
					if (*return_type != SymbolType::OK) {
						//add_error("Inconsistent return types in statement block.", node->line_start);
						return SymbolType::INVALID;
					}
					return_type = new SymbolType(current_type);
				}
			}
		}

		return *return_type;
	}
	// Forwarders
	else if (node_type == Dim) {
	if (children.size()) return resolve_type(node->children[0], function_entry, class_table, global_table, dot_class_table);
	else return SymbolType::INTEGER;
}
	else if (node_type == FuncCallStat || node_type == Condition) {
		SymbolType type = resolve_type(node->children[0], function_entry, class_table, global_table, dot_class_table);
		return stat_return(type);
	}
	else if (node_type == Dot) {
		AST* left_node = node->children[0];
		AST* next_node = node->children[1];

		if (function_table != class_table) {
			AST* id = get_type(left_node->children, Id);
			if (id->value == "self") {
				SymbolType self_type(*function_entry->scope, vector<int>());
				left_node->decorator.set_type(self_type);
				node->decorator.set_type(self_type);
				//left_node->type = *function_entry->scope;
				SymbolType type = resolve_type(next_node, function_entry, class_table, global_table, class_table);
				return type;
			}
		}

		SymbolType start_type = resolve_type(left_node, function_entry, class_table, global_table, dot_class_table);

		if (start_type.dimensions.size()) {
			stringstream ss;
			ss << start_type;
			add_error("An array does not have any members. Calling member on " + ss.str() + ".", line_start);
			return SymbolType::INVALID;
		}

		SymbolTableEntry* class_entry = global_table->get_entry(start_type.type_id);

		if (!class_entry) {
			stringstream ss;
			ss << start_type;
			add_error("Cannot call member on \"" + ss.str() + "\" which is either not a class or an undeclared class.", line_start);
			return SymbolType::INVALID;
		}

		SymbolTable* current_table = class_entry->link;

		/*
		while (next_node->type == Dot) {
			left_node = next_node->children[0];

			SymbolType left_type = resolve_type(left_node, current_table, current_table, global_table);

			if (left_type.dimensions.size()) {
				stringstream ss;
				ss << left_type;
				add_error("An array does not have any members. Calling member on " + ss.str() + ".", left_node->line_start);
				return SymbolType::INVALID;
			}

			class_entry = global_table->get_entry(start_type.type_id);
			current_table = class_entry->link;

			next_node = next_node->children[1];
		}
		*/
		node->decorator.set_type(start_type);
		return resolve_type(next_node, function_entry, class_table, global_table, current_table);
	}

	// forwarders
	
	return SymbolType::INVALID;
}

void SemanticAnalyzer::check_function_names(SymbolTable* global_table) {
	vector<SymbolTable*> tables_to_check;
	tables_to_check.push_back(global_table);

	for (const auto& pair : global_table->entries) {
		SymbolTableEntry* entry = pair.second;
		if (entry->kind == EntryKind::Class) {
			tables_to_check.push_back(entry->link);
		}
	}

	for (SymbolTable* table : tables_to_check) {
		unordered_map<string, vector<string>> functions;

		const string scope = (table->name == "global" ? "" : table->name + "::");

		for (const auto& pair : table->entries) {
			SymbolTableEntry* entry = pair.second;
			string function_name = entry->name;
			if (entry->kind == EntryKind::FuncDef) {
				
				string function_id = entry->unique_id;
				if (!functions.count(function_name)) functions.insert({ function_name, vector<string>() });

				vector<string>& func_id_list = functions.at(function_name);
				func_id_list.push_back(function_id);
			}
			else if (entry->kind == EntryKind::FuncDecl) {
				add_error("Function " + scope + function_name +  " is declared but never defined", entry->line_location);
			}
		}

		for (const auto& pair : functions) {
			const vector<string>& func_id_list = pair.second;
			const string function_name = pair.first;

			if (func_id_list.size() > 1) {
				
				std::stringstream ss;
				bool first = true;
				for (string function_id : func_id_list) {
					if (first) first = false;
					else ss << ", ";
					ss << function_id;
				}

				if (function_name == "main" && table->name == "global") {
					add_error("Multiple main functions with different signatures: " + ss.str() + ".", -1);
				}
				else {
					add_warning("Function " + scope + function_name + " overloaded: " + ss.str() + ".", -1);
				}
			}
		}
	}



}

int SemanticAnalyzer::migrate_line_locations(AST* node) {
	if (node->token) {
		node->line_start = node->token->line_location;
		return node->line_start;
	}
	else {
		int line = -1;
		for (AST* child : node->children) {
			int child_line = migrate_line_locations(child);
			if (child_line != -1 && (line == -1 || child_line < line)) {
				line = child_line;	
			}
		}
		node->line_start = line;
		return line;
	}
}

bool SemanticAnalyzer::analyze() {
	ASTGenerator ast_generator(filename);
	AST* root = ast_generator.get_AST();
	dot_writer::write_ast(root, filename);

	// Invalid syntax
	if (!root) return false;

	SymbolTable* global_table = construct_symbol_tables(root);
	std::ofstream file("output/outsemantic/" + filename + ".outsymboltables");
	//file << *global_table;

	migrate_line_locations(root);
	perform_semantic_checks(root, global_table);

	print_errors();

	
	CodeGenerator code_generator(filename, root, global_table);
	if (!errors.size()) code_generator.generate();
	else code_generator.write_comment("semantic errors");
	
	
	file << *global_table;
	return true;
}

bool operator<(const SemanticAnalyzer::SemanticError& e1, const SemanticAnalyzer::SemanticError& e2)
{
	return e1.line_location < e2.line_location;
}

void SemanticAnalyzer::print_errors() {
	std::ofstream file("output/outsemantic/" + filename + ".outsemanticerrors");
	vector<SemanticError> errors_warnings;
	errors_warnings.insert(errors_warnings.end(), errors.begin(), errors.end());
	errors_warnings.insert(errors_warnings.end(), warnings.begin(), warnings.end());
	std::sort(errors_warnings.begin(), errors_warnings.end());

	for (SemanticError error : errors_warnings) {
		file << error.text;
		if(error.line_location >= 0) file << " (line " << error.line_location << ")";
		file << endl;
	}
}

SymbolTableClassEntry* SemanticAnalyzer::generate_class_entry(AST* class_node) {
	const vector<AST*>& children = class_node->children;

	const AST* id = get_type(children, Id);

	SymbolTable* class_table = new SymbolTable(id->value);

	const Token id_token = *(id->token);

	const int line_location = id_token.line_location;

	const string name = id->value;

	//const AST* isa = get_type(children, ISA);

	const vector<AST*> variables = get_types(children, VarDecl);

	for (AST* variable : variables) {
		SymbolTableEntry* entry = generate_variable_entry(variable, STE::Kind::Data);

		SymbolTableEntry* duplicate_entry = class_table->add_entry_if_new(entry);

		if (entry != duplicate_entry) {
			add_error("There already exists a variable of the name " + entry->name + " in class " + name + " at line " + to_string(duplicate_entry->line_location) + ".", line_location);
		}
	}

	const vector<AST*> functions = get_types(children, FuncDecl);

	for (AST* func_decl : functions) {
		SymbolTableEntry* entry = generate_function_entry(func_decl, STE::Kind::FuncDecl, name);

		SymbolTableEntry* duplicate_entry = class_table->add_entry_if_new(entry);

		if (entry != duplicate_entry) {
			add_error("There already exists a method of the name " + entry->unique_id + " in class " + name + " at line " + to_string(duplicate_entry->line_location) + ".", line_location);
		}
	}

	const AST* isa_node = get_type(children, ISA);

	if (isa_node) {
		for (AST* parent : isa_node->children) {
			SymbolTableEntry* entry = generate_inherit_entry(parent);

			SymbolTableEntry* duplicate_entry = class_table->add_entry_if_new(entry);

			if (entry != duplicate_entry) {
				add_error("Inheriting from the same class " + entry->name + " more than once.", line_location);
			}
		}
	}


	return new SymbolTableClassEntry(name, name, STE::Kind::Class, nullptr, line_location, STE::Visibility::None, class_table, class_node);
}

SymbolTableEntry* SemanticAnalyzer::generate_inherit_entry(AST* id_node) {
	const string name = id_node->value;
	const int line_location = id_node->token->line_location;
	return new SymbolTableEntry(name + ' ', name, EntryKind::Inherit, nullptr, line_location, STE::Visibility::None, nullptr, id_node);
}

SymbolTableEntry* SemanticAnalyzer::generate_function_entry(AST* func_node, STE::Kind func_kind, string parent_class) {
	if (func_kind != STE::Kind::FuncDecl && func_kind != STE::Kind::FuncDef) return nullptr;

	const vector<AST*>& children = func_node->children;

	const AST* id = get_type(children, Id);

	const Token id_token = *(id->token);

	const int line_location = id_token.line_location;

	string name = parent_class;


	std::optional<string> scope = {};

	if (func_kind == STE::Kind::FuncDef) {
		AST* scope_node = get_type(children, Scope);
		AST* scope_id_node = get_type(scope_node->children, Id);
		if (scope_id_node && scope_id_node->value.size()) scope = { scope_id_node->value };
	}
	else {
		scope = { parent_class };
	}
	

	const AST* type_node = get_type(children, Type);

	string type_id = parent_class;

	if (type_node && type_node->is_leaf) {
		type_id = type_node->value;
		name = id->value;
	}

	SymbolTable* func_table = new FunctionSymbolTable(name);

	const AST* param_list = get_type(children, ParamList);

	vector<SymbolType> parameters;

	for (AST* param : param_list->children) {
		SymbolTableEntry* entry = generate_variable_entry(param, STE::Kind::Parameter);

		SymbolTableEntry* duplicate_entry = func_table->add_entry_if_new(entry);

		if (entry != duplicate_entry) {
			add_error("There already exists a parameter of the name " + entry->name + " at line " + to_string(duplicate_entry->line_location) + ".", line_location);
		}
		else {
			parameters.push_back(*entry->type);
		}
	}


	FunctionSymbolType* type = new FunctionSymbolType(type_id, parameters);

	if (func_kind == STE::Kind::FuncDef) {
		const AST* stat_block = get_type(children, StatBlock);
		const vector<AST*> localvars = get_types(stat_block->children, VarDecl);
		for (AST* localvar : localvars) {
			SymbolTableEntry* entry = generate_variable_entry(localvar, STE::Kind::Localvar);

			SymbolTableEntry* duplicate_entry = func_table->add_entry_if_new(entry);

			if (entry != duplicate_entry) {
				add_error("There already exists a variable or parameter of the name " + entry->name + " at line " + to_string(duplicate_entry->line_location) + ".", entry->line_location);
			}
		}
	}

	string unique_id = name + type->signature;
	
	const AST* visibility_node = get_type(children, Visibility);

	STE::Visibility visibility = get_visibility(visibility_node);

	return new SymbolTableEntry(unique_id, name, func_kind, type, line_location, visibility, func_table, func_node, scope);
}

STE::Visibility SemanticAnalyzer::get_visibility(const AST* visibility_node) {
	STE::Visibility visibility = STE::Visibility::None;

	if (visibility_node) {
		if (visibility_node->value == "private") {
			visibility = STE::Visibility::Private;
		}
		else {
			visibility = STE::Visibility::Public;
		}
	}

	return visibility;
}

SymbolTableEntry* SemanticAnalyzer::generate_variable_entry(AST* var_node, STE::Kind kind) {
	const vector<AST*>& children = var_node->children;

	const AST* id = get_type(children, Id);

	const Token id_token = *(id->token);

	const int line_location = id_token.line_location;

	const string name = id->value;

	const AST* type_node = get_type(children, Type);

	const string type_id = type_node->value;

	const AST* dim_list = get_type(children, DimList);

	vector<int> dimensions;
	if(dim_list) dimensions = get_dimensions(dim_list->children);

	SymbolType* type = new SymbolType(type_id, dimensions);

	const AST* visibility_node = get_type(children, Visibility);

	STE::Visibility visibility = get_visibility(visibility_node);

	return new SymbolTableEntry(name, name, kind, type, line_location, visibility, nullptr, var_node);
}

vector<int> SemanticAnalyzer::get_dimensions(const std::vector <AST*> dimension_nodes) {
	vector<int> dimensions;

	for (AST* dimension : dimension_nodes) {
		
		int value = -1;
		if (dimension->children.size() == 1) value = stoi(dimension->children[0]->value);
		dimensions.push_back(value);
	}
		

	return dimensions;
}

SymbolTable* SemanticAnalyzer::construct_symbol_tables(AST* root) {
	SymbolTable* global_table = new SymbolTable("global");

	const vector<AST*>& children = root->children;

	const vector<AST*> class_decls = get_types(children, Class);

	const vector<AST*> func_defs = get_types(children, FuncDef);

	vector<SymbolTableClassEntry*> class_entries;

	// Start with classes so that you can add method definitions to the classes' symbol tables
	for (AST* class_decl : class_decls) {
		SymbolTableClassEntry* entry = generate_class_entry(class_decl);

		SymbolTableEntry* duplicate_entry = global_table->add_entry_if_new(entry);

		// there's already a class of this name
		if (duplicate_entry != entry) {
			stringstream ss;
			ss << "The class " << entry->name << " has already been declared at line " << duplicate_entry->line_location << ".";
			add_error(ss.str(), entry->line_location);
		}
		else {
			class_entries.push_back(entry);
			class_decl->decorator.class_entry = entry;
		}
	}

	bool main_func = false;

	for (AST* func_def : func_defs) {
		AST* scope = get_type(func_def->children, Scope);
		AST* scope_id = get_type(scope->children, Id);

		if (scope_id) {
			
			string class_name = scope_id->value;


			SymbolTableEntry* class_entry = global_table->get_entry(class_name);
			if (class_entry) {
				SymbolTableEntry* entry = generate_function_entry(func_def, STE::Kind::FuncDef, class_name);

				SymbolTable* class_symbol_table = class_entry->link;

				// if nullptr, no DECLARATION
				// else 
					// if funcdecl, then override
					// else already defined
				SymbolTableEntry* current_entry = class_symbol_table->get_entry(entry->unique_id);

				if (current_entry) {
					if (current_entry->kind == STE::Kind::FuncDecl) {
						// replace funcdecl
						class_symbol_table->entries.insert({entry->unique_id, entry});
						entry->visibility = current_entry->visibility;

						current_entry->kind = entry->kind;
						current_entry->line_location = entry->line_location;
						current_entry->node = entry->node;
						current_entry->node->entry = entry;

						delete current_entry->link;

						current_entry->link = entry->link;
						func_def->decorator.function_entry = current_entry;
						func_def->decorator.class_entry = class_entry;
					}
					else {
						add_error("The function " + entry->unique_id + " is already defined in class " + class_name + ".", entry->line_location);
					}
				}
				else {
					add_error("No function declaration for " + entry->unique_id + " in class " + class_name + ".", entry->line_location);
				}
			}
			else {
				add_error("The class " + class_name + " does not exist.", scope_id->token->line_location);
			}
		}
		else {
			SymbolTableEntry* entry = generate_function_entry(func_def, STE::Kind::FuncDef, "error");
			if (entry->name == "main") main_func = true;

			SymbolTableEntry* duplicate_entry = global_table->add_entry_if_new(entry);

			if (entry != duplicate_entry) {
				add_error("There already exists a function " + entry->unique_id + " at line " + to_string(duplicate_entry->line_location) + ".", entry->line_location);
			}
			else {
				func_def->decorator.function_entry = entry;
			}
		}
	}

	if (!main_func) add_error("Missing main function.", -1);

	unordered_set<SymbolTableClassEntry*> augmented_classes;
	for (SymbolTableClassEntry* class_entry : class_entries) {
		unordered_set<SymbolTableClassEntry*> current_path;
		traverse_parents(class_entry, augmented_classes, current_path, global_table);
	}

	return global_table;
}

void SemanticAnalyzer::pull_members(SymbolTableClassEntry* class_entry, SymbolTableClassEntry* parent_entry) {
	SymbolTable* child_table = class_entry->link;

	for (const auto& entry_pair : parent_entry->link->entries) {
		SymbolTableEntry* parent_member = entry_pair.second;
		if (parent_member->kind == EntryKind::Data || parent_member->kind == EntryKind::FuncDef) {
			string member_unique_id = parent_member->unique_id;
			SymbolTableEntry* main_dup_entry = child_table->get_entry(member_unique_id);
			if (main_dup_entry) {
				add_warning("Member " + member_unique_id + " in " + class_entry->name + " is overriding the one defined in " + parent_entry->name + ".", main_dup_entry->line_location);
			}
			else {
				if (class_entry->inherited_members.count(member_unique_id)) {
					add_error("Ambiguity inheriting member " + member_unique_id + " in " + class_entry->name + " since it exists in two or more inherited classes.", class_entry->line_location);
				}
				else {
					class_entry->inherited_members.insert({ parent_member->unique_id, parent_member });
				}
			}
		}
	}

}

void SemanticAnalyzer::traverse_parents(SymbolTableClassEntry* class_entry, unordered_set<SymbolTableClassEntry*>& augmented_classes, unordered_set<SymbolTableClassEntry*>& current_path, const SymbolTable* global_table) {
	if (augmented_classes.count(class_entry)) return;
	if (current_path.count(class_entry)) {
		std::stringstream ss;
		bool first = true;
		for (SymbolTableClassEntry* visited_class : current_path) {
			if (first) first = false;
			else ss << ", ";
			ss << visited_class->name;
		}
		add_error("Circular inheritance found: " + ss.str(), class_entry->line_location);
		return;
	}
	current_path.insert(class_entry);
	for (const auto& entry_pair : class_entry->link->entries) {
		SymbolTableEntry* inherit_entry = entry_pair.second;
		if (inherit_entry->kind == EntryKind::Inherit) {
			SymbolTableEntry* parent_entry = global_table->get_entry(inherit_entry->name);
			if (parent_entry) {
				// could add link to inherit to class if u want
				SymbolTableClassEntry* parent_class_entry = static_cast<SymbolTableClassEntry*>(parent_entry);
				traverse_parents(parent_class_entry, augmented_classes, current_path, global_table);
				pull_members(class_entry, parent_class_entry);
			} else{
				add_error("Class " + class_entry->name + " cannot inherit from non-existent class " + inherit_entry->name + ".", class_entry->line_location);
			}
		}
	}
	current_path.erase(class_entry);
	augmented_classes.insert(class_entry);
	return;
}