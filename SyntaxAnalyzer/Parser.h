#pragma once
#include "../LexicalAnalyzer/Token.h"
#include "../LexicalAnalyzer/LexicalAnalyzer.h"
#include <stack>

class ASTGenerator;

using std::string;
class Parser {
private:
	static std::unordered_map<std::string, std::unordered_map<std::string, std::string>> initialize_parsing_map();
	LexicalAnalyzer la;
	
	std::ofstream derivation_file;
	std::ofstream error_file;
	string left_side = "";
	ASTGenerator* semantic_analyzer;
	void print_current_grammar();
public:
	bool error;
	static const std::unordered_map<std::string, std::unordered_map<std::string, std::string>> parsing_map;
	static const std::unordered_map<std::string, std::unordered_set<std::string>> first_set_map;
	static const std::unordered_map<std::string, std::unordered_set<std::string>> follow_set_map;
	static const std::unordered_set<std::string> nullable_set;
	Parser(std::string filename);
	Parser(std::string filename, ASTGenerator* s_a);
	std::stack<std::string> stack;
	bool parse();
	void pop();
	std::string top();
	bool in_follow(string, Token::Type);
	bool in_first(string, Token::Type);
	bool is_nullable(string);
	void skip_errors(Token&);
	void inverse_push(std::string);
	void print_stack();
	void print_errors(const Token&);
	Token get_next_token();
};