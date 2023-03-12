#include "SemanticAnalyzer.h"
#include "ASTGenerator.h"



using std::string;
typedef SymbolTableEntry STE;
using std::cout;
using std::endl;
using std::vector;
typedef SymbolTableEntry::Kind EntryKind;

using namespace ASTConstants;

SemanticAnalyzer::SemanticAnalyzer(string filename) : filename(filename) {

}

bool SemanticAnalyzer::analyze() {
	ASTGenerator ast_generator(filename);
	AST* root = ast_generator.get_AST();

	// Invalid syntax
	if (!root) return false;

	SymbolTable* global_table = construct_symbol_tables(root);
	std::ofstream file("output/outsemantic/" + filename + ".outsymboltables");
	file << *global_table;
	return true;
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

SymbolTableEntry* SemanticAnalyzer::generate_class_entry(AST* class_node) {

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

		}
	}

	const vector<AST*> functions = get_types(children, FuncDecl);


	return new SymbolTableEntry(name, name, STE::Kind::Class, nullptr, line_location, STE::Visibility::None, class_table);
}

SymbolTableEntry* SemanticAnalyzer::generate_function_entry(AST* func_node) {
	const vector<AST*>& children = func_node->children;

	const AST* id = get_type(children, Id);

	const Token id_token = *(id->token);

	const int line_location = id_token.line_location;

	const string name = id->value;

	const AST* type_node = get_type(children, Type);

	const string type_id = type_node->value;

	return nullptr;
}

SymbolTableEntry* SemanticAnalyzer::generate_variable_entry(AST* var_node, EntryKind kind) {
	const vector<AST*>& children = var_node->children;

	const AST* id = get_type(children, Id);

	const Token id_token = *(id->token);

	const int line_location = id_token.line_location;

	const string name = id->value;

	const AST* type_node = get_type(children, Type);

	const string type_id = type_node->value;

	const AST* dim_list = get_type(children, DimList);

	vector<int> dimensions = get_dimensions(dim_list->children);

	SymbolType* type = new SymbolType(type_id, dimensions);

	const AST* visibility_node = get_type(children, Visibility);

	STE::Visibility visibility = STE::Visibility::None;

	if (visibility_node) {
		if (visibility_node->value == "private") {
			visibility = STE::Visibility::Private;
		}
		else {
			visibility = STE::Visibility::Public;
		}
	}

	return new SymbolTableEntry(name, name, kind, type, line_location, visibility, nullptr);
}

vector<int> SemanticAnalyzer::get_dimensions(const std::vector <AST*> dimension_nodes) {
	vector<int> dimensions;

	for (AST* dimension : dimension_nodes) 
		dimensions.push_back(stoi(dimension->value));

	return dimensions;
}


SymbolTable* SemanticAnalyzer::construct_symbol_tables(AST* node) {
	SymbolTable* global_table = new SymbolTable("global");

	const vector<AST*>& children = node->children;

	const vector<AST*> class_decls = get_types(children, Class);

	const vector<AST*> func_defs = get_types(children, FuncDef);

	// Start with classes so that you can add method definitions to the classes' symbol tables
	for (AST* class_decl : class_decls) {
		SymbolTableEntry* entry = generate_class_entry(class_decl);

		SymbolTableEntry* duplicate_entry = global_table->add_entry_if_new(entry);

		// there's already a class of this name
		if (duplicate_entry != entry) {
			cout << "The class " << entry->name << " declared at line " << entry->line_location << " has already been declared at line " << duplicate_entry->line_location << "." << endl;
		}
	}


	return global_table;


}