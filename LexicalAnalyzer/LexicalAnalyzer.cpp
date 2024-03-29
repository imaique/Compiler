#include "LexicalAnalyzer.h"
#include <sstream>
#include "../Compiler/Compiler.h"

typedef std::string string;
typedef Token::Type TT;

LexicalAnalyzer::LexicalAnalyzer(string filename) {
	token_file.open("output/outlex/" + filename + ".outlextokens");
	error_file.open("output/outlex/" + filename + ".outlexerrors");
	input_file.open("input/" + filename + ".src");

	state = State::get_state(1);
	index = 0;
	line_number = 1;
	last_token_line = -1;
	std::getline(input_file, line);
}


void LexicalAnalyzer::increment_line() {
	index = 0;
	line_number++;
	trim();
}


Token LexicalAnalyzer::get_next_token() {
	if (!has_next_token()) return Token("", line_number, TT::EndOfFile, 0);
	this->state = State::get_state(1);
	std::stringstream token_stream;

	int index_start = index;

	const int location = line_number;

	while (!state->is_final_state) {
		bool end_of_line = index == line.size();
		char c;
		if (!end_of_line) c = line[index];
		else {
			c = '\n';
			if(!std::getline(input_file, line)) break;
			increment_line();
		}
		Response response = state->get_next_state(c);
		if (response.consume) {
			if(c == '\n') token_stream << "\\n";
			else token_stream << c;
			if(!end_of_line) index++;
		}
		state = response.nextState;
	}
	
	TT token_type;
	if (state->is_final_state) {
		FinalState* const final_state = dynamic_cast<FinalState*>(state);
		token_type = final_state->get_token_type();
	}
	// only happens when there's an open block comment
	// might need to modularize logic on introduction on other multi-line tokens
	else {
		token_type = TT::UnclosedBlockComment;
	}

	std::string lexeme = token_stream.str();

	//if it's a line comment, remove the \n
	if (token_type == TT::LineComment) lexeme = lexeme.substr(0, lexeme.size() - 2);
	else if (token_type == TT::ID && Token::is_reserved_word(lexeme)) token_type = Token::get_reserved_type(lexeme);

	Token token(lexeme, location, token_type, index_start);

	print_token(token);

	

	return token;
}

void LexicalAnalyzer::print_token(Token token) {
	const int location = token.line_location;
	if (last_token_line != location) {
		if (last_token_line > 0) token_file << std::endl;
		last_token_line = location;
	}
	else token_file << " ";
	token_file << token;

	if (token.is_error) {
		error = true;

		Compiler::add_error(CompilerError(Token::get_error_string(token.token_type), location));
		error_file << "Lexical error: " << Token::get_error_string(token.token_type) << ": ";
		error_file << "\"" << token.lexeme << "\": line " << location << std::endl;
	}
}

void LexicalAnalyzer::trim() {
	while (index < line.size() && (line[index] == ' ' || line[index] == '\t' || line[index] == '\n')) index++;
}

void LexicalAnalyzer::skip_empty_lines() {
	// trim starting whitespace
	trim();
	
	while (index >= line.size() && std::getline(input_file, line)) {
		increment_line();
	}
}
bool LexicalAnalyzer::has_next_token() {
	skip_empty_lines();
	if (index < line.size()) return true;
	else {
		if (!token_file.is_open()) return false;
		if(error) std::cout << "Lexical error(s) detected." << std::endl;
		std::cout << "Lexical Analysis complete." << std::endl;
		token_file.close();
		error_file.close();
		return false;
	}
}

