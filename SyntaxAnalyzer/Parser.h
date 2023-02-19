#pragma once
#include "../LexicalAnalyzer/Token.h"
#include "../LexicalAnalyzer/LexicalAnalyzer.h"
class Parser {
private:
	static const std::unordered_map<std::string, std::unordered_map<std::string, std::string>> parsing_map;
	static std::unordered_map<std::string, std::unordered_map<std::string, std::string>> initialize_parsing_map();
	LexicalAnalyzer const la;
	
public:
	Parser(std::string filename);
	void parse();
};