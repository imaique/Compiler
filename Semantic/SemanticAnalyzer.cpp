#include "SemanticAnalyzer.h"
#include "ASTGenerator.h"
#include <algorithm>
#include <sstream>


using std::string;
typedef SymbolTableEntry STE;
using std::cout;
using std::endl;
using std::vector;
using std::to_string;
using std::unordered_set;
typedef SymbolTableEntry::Kind EntryKind;

using namespace ASTConstants;

SemanticAnalyzer::SemanticAnalyzer(string filename) : filename(filename) {

}

SemanticAnalyzer::SemanticError::SemanticError(std::string text, int line_location) : text(text), line_location(line_location) {

}

void SemanticAnalyzer::add_error(std::string text, int line) {
	SemanticError error(text, line);
	errors.push_back(error);
}

void SemanticAnalyzer::add_warning(std::string text, int line) {
	SemanticError warning(text, line);
	warnings.push_back(warning);
}

void SemanticAnalyzer::perform_semantic_checks(AST* root, SymbolTable* global_table) {

}

bool SemanticAnalyzer::analyze() {
	ASTGenerator ast_generator(filename);
	AST* root = ast_generator.get_AST();

	// Invalid syntax
	if (!root) return false;

	SymbolTable* global_table = construct_symbol_tables(root);
	std::ofstream file("output/outsemantic/" + filename + ".outsymboltables");
	file << *global_table;

	perform_semantic_checks(root, global_table);

	print_errors();
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

	for (SemanticError error : errors_warnings) file << error.text << " (line " << error.line_location  << ")" << endl;
}


vector<AST*> SemanticAnalyzer::get_types(const vector <AST*>& list, std::string type) {
	vector<AST*> type_list;
	for (AST* el : list) if (el->get_type() == type) type_list.push_back(el);
	return type_list;
}

AST* SemanticAnalyzer::get_type(const vector <AST*>& list, std::string type) {
	for (AST* el : list) if (el->get_type() == type) return el;
	return nullptr;
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


	return new SymbolTableClassEntry(name, name, STE::Kind::Class, nullptr, line_location, STE::Visibility::None, class_table);
}

SymbolTableEntry* SemanticAnalyzer::generate_inherit_entry(AST* id_node) {
	const string name = id_node->value;
	const int line_location = id_node->token->line_location;
	return new SymbolTableEntry(name + ' ', name, EntryKind::Inherit, nullptr, line_location, STE::Visibility::None, nullptr);

}

SymbolTableEntry* SemanticAnalyzer::generate_function_entry(AST* func_node, STE::Kind func_kind, string parent_class) {
	if (func_kind != STE::Kind::FuncDecl && func_kind != STE::Kind::FuncDef) return nullptr;

	const vector<AST*>& children = func_node->children;

	const AST* id = get_type(children, Id);

	const Token id_token = *(id->token);

	const int line_location = id_token.line_location;

	const string name = id->value;

	SymbolTable* func_table = new SymbolTable(name);

	const AST* type_node = get_type(children, Type);

	const string type_id = type_node ? type_node->value : parent_class;

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
				add_error("There already exists a variable or parameter of the name " + entry->name + " at line " + to_string(duplicate_entry->line_location) + ".", line_location);
			}
		}
	}

	string unique_id = name + type->signature;

	
	const AST* visibility_node = get_type(children, Visibility);

	STE::Visibility visibility = get_visibility(visibility_node);

	return new SymbolTableEntry(unique_id, name, func_kind, type, line_location, visibility, func_table);
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

SymbolTableEntry* SemanticAnalyzer::generate_variable_entry(AST* var_node, EntryKind kind) {
	if (kind == EntryKind::Localvar) {
		cout << "ayo";
	}
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

	return new SymbolTableEntry(name, name, kind, type, line_location, visibility, nullptr);
}

vector<int> SemanticAnalyzer::get_dimensions(const std::vector <AST*> dimension_nodes) {
	vector<int> dimensions;

	for (AST* dimension : dimension_nodes) {
		int value = -1;
		if (dimension->value.size()) value = stoi(dimension->value);
		dimensions.push_back(value);
	}
		

	return dimensions;
}


SymbolTable* SemanticAnalyzer::construct_symbol_tables(AST* node) {
	SymbolTable* global_table = new SymbolTable("global");

	const vector<AST*>& children = node->children;

	const vector<AST*> class_decls = get_types(children, Class);

	const vector<AST*> func_defs = get_types(children, FuncDef);

	vector<SymbolTableClassEntry*> class_entries;

	// Start with classes so that you can add method definitions to the classes' symbol tables
	for (AST* class_decl : class_decls) {
		SymbolTableClassEntry* entry = generate_class_entry(class_decl);

		SymbolTableEntry* duplicate_entry = global_table->add_entry_if_new(entry);

		// there's already a class of this name
		if (duplicate_entry != entry) {
			cout << "The class " << entry->name << " declared at line " << entry->line_location << " has already been declared at line " << duplicate_entry->line_location << "." << endl;
		}
		else {
			class_entries.push_back(entry);
		}
	}

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
						current_entry->kind = entry->kind;
						current_entry->line_location = entry->line_location;

						delete current_entry->link;

						current_entry->link = entry->link;
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

			SymbolTableEntry* duplicate_entry = global_table->add_entry_if_new(entry);

			if (entry != duplicate_entry) {
				add_error("There already exists a function " + entry->unique_id + " at line " + to_string(duplicate_entry->line_location) + ".", entry->line_location);
			}
		}
	}

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
		if (parent_member->kind == EntryKind::Data || parent_member->kind == EntryKind::FuncDef || parent_member->kind == EntryKind::FuncDef) {
			string member_unique_id = parent_member->unique_id;
			SymbolTableEntry* main_dup_entry = child_table->get_entry(member_unique_id);
			if (main_dup_entry) {
				add_warning("Member " + member_unique_id + " in " + class_entry->name + " is overriding a member in " + parent_entry->name + ".", main_dup_entry->line_location);
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