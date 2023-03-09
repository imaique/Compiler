#include "SemanticAnalyzer.h"
#include "ASTGenerator.h"


using std::string;

SemanticAnalyzer::SemanticAnalyzer(string filename) : filename(filename) {

}

bool SemanticAnalyzer::analyze() {
	ASTGenerator ast_generator(this->filename);
	AST* root = ast_generator.get_AST();

	// Invalid syntax
	if (!root) return false;

	construct_symbol_tables(root);
}

void SemanticAnalyzer::construct_symbol_tables(AST* node) {

}