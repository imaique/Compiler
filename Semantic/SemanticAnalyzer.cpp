#include "SemanticAnalyzer.h"


using std::string;

SemanticAnalyzer::SemanticAnalyzer(string filename) : filename(filename) {

}

bool SemanticAnalyzer::analyze() {
	ASTGenerator ast_generator(this->filename);
	AST* ast = ast_generator.get_AST();

	// Invalid syntax
	if (!ast) return false;

	construct_symbol_tables(ast);
}

