#include "SymbolTable.h"
#include <sstream>
#include <iostream>
#include <algorithm>

using std::string;
typedef SymbolTableEntry STE;
using std::vector;
using std::endl;
using std::stringstream;

const std::unordered_map<STE::Kind, std::string> SymbolTableEntry::kind_strings{
		{Kind::Class, "class"},
		{Kind::FuncDecl, "function"},
		{Kind::FuncDef, "function"},
		{Kind::Parameter, "param"},
		{Kind::Inherit, "inherit"},
		{Kind::Data, "data"},
		{Kind::Localvar, "local"},
};

const std::unordered_map<STE::Visibility, std::string> SymbolTableEntry::visibility_strings{
		{Visibility::Public, "public"},
		{Visibility::Private, "private"},
};


// SymbolType

const SymbolType SymbolType::INVALID = SymbolType("#", vector<int>());

const SymbolType SymbolType::INTEGER = SymbolType("integer", vector<int>());

const SymbolType SymbolType::FLOAT = SymbolType("float", vector<int>());

const SymbolType SymbolType::OK = SymbolType("^", vector<int>());

const SymbolType SymbolType::VOID = SymbolType("void", vector<int>());


std::ostream& operator<<(std::ostream& os, const SymbolType& type) {
	if (type == SymbolType::INVALID) {
		os << "INVALID TYPE";
	}
	else if (type == SymbolType::OK) os << "OK";
	else os << type.type_id;
	for (int dimension : type.dimensions) {
		os << "[" << (dimension >= 0 ? std::to_string(dimension) : "") << "]";
	}
	return os;
}

std::ostream& operator<<(std::ostream& os, const FunctionSymbolType& type) {
	os << type.type_id;
	for (int dimension : type.dimensions) {
		os << "[" << dimension << "]";
	}

	os << " : ";

	bool first = true;

	for (SymbolType st : type.param_types) {
		if (!first) {
			os << ", ";
		}
		else {
			first = false;
		}
		os << st;
	}
	return os;
}

// SymbolTable

SymbolType::SymbolType(std::string type_id, std::vector<int> dimensions) : type_id(type_id), dimensions(dimensions) {

}

bool SymbolType::operator==(const SymbolType& other) const
{
	return type_id == other.type_id && dimensions.size() == other.dimensions.size();
}

bool SymbolType::operator!=(const SymbolType& other) const
{
	return !(*this == other);
}


SymbolTable::SymbolTable(std::string name) : name(name) {

}

void SymbolTable::add_entry(SymbolTableEntry* entry) {
	entries.insert({entry->unique_id, entry});
}

std::string FunctionSymbolType::get_signature(std::vector<SymbolType> param_types) {
	std::stringstream ss;
	ss << '(';
	bool first = true;
	for (SymbolType param_type : param_types) {
		if (!first) {
			ss << ", ";
		}
		else {
			first = false;
		}
		ss << param_type;
	}
	ss << ')';
	return ss.str();
}

FunctionSymbolType::FunctionSymbolType(std::string return_type, std::vector<SymbolType> param_types) 
	: SymbolType(return_type, vector<int>()), param_types(param_types), signature(get_signature(param_types)) {

}

void try_push(int& index, vector<int>& widths, int width) {
	if (widths.size() == index) widths.push_back(width);
	else widths[index] = std::max(widths[index], width);
	index++;
}

void SymbolTable::get_spaces(int index, vector<int>& widths) {
	vector<SymbolTableEntry*> sorted_entries = get_sorted_entries();
	try_push(index, widths, ("table: " + name).size());
	for (SymbolTableEntry* entry : sorted_entries) {
		int local_index = index;

		int first_width = SymbolTableEntry::kind_strings.at(entry->kind).size();
		try_push(local_index, widths, first_width);

		int second_width = entry->name.size();
		try_push(local_index, widths, second_width);

		SymbolType* type = entry->type;
		if (type) {
			std::stringstream type_str;
			type_str << type;
			try_push(local_index, widths, type_str.str().size());
		}
		else {
			try_push(local_index, widths, 0);
		}

		STE::Visibility visibility = entry->visibility;

		if (visibility != STE::Visibility::None) {
			try_push(local_index, widths, STE::visibility_strings.at(visibility).size());
		}
		else {
			try_push(local_index, widths, 0);
		}

		if (entry->link) entry->link->get_spaces(index + 1, widths);
	}
}

void SymbolTable::print_table(std::ostream& os) {
	vector<int> widths;
	get_spaces(0, widths);
	print_table(os, 0, widths);
}

void print_char(std::ostream& os, int nb, char c) {
	for (int i = 0; i < nb; i++) os << c;
}

void print_info(std::ostream& os, int index, const std::vector<int>& widths, const std::vector<string>& info, bool table) {
	char c = table ? '=' : ' ';
	for (int i = 0, j = 0; i < widths.size(); i++) {
		if (i >= index) {
			if (j < info.size()) {
				os << info[j];
				print_char(os, widths[i] - info[j].size(), c);
			}
			else {
				print_char(os, widths[i], c);
			}
			os << (table ? "===" : " | ");
			j++;
		}
		else {
			print_char(os, widths[i], ' ');
			os << " | ";
		}
		
	}
	os << endl;
}

void print_info(std::ostream& os, int index, const std::vector<int>& widths, const std::vector<string>& info) {
	print_info(os, index, widths, info, false);
}


void SymbolTable::print_table(std::ostream& os, int index, const std::vector<int>& widths) {

	vector<string> info { "table: " + name };
	print_info(os, index, widths, vector<string>(), true);
	print_info(os, index, widths, info);
	index++;
	vector<SymbolTableEntry*> sorted_entries = get_sorted_entries();

	for (SymbolTableEntry* entry : sorted_entries) {
		
		std::string kind_string = SymbolTableEntry::kind_strings.at(entry->kind);

		SymbolType* type = entry->type;
		STE::Visibility visibility = entry->visibility;

		string type_string = "";
		string visibility_string = "";

		if (type) {
			stringstream ss;
			ss << *type;
			type_string = ss.str();
		}
		if (visibility != STE::Visibility::None) {
			visibility_string = STE::visibility_strings.at(visibility);
		}

		vector<string> info{kind_string, entry->name, type_string, visibility_string };
		print_info(os, index, widths, info);

		if (entry->link) entry->link->print_table(os, index + 1, widths);
	}
	print_info(os, index - 1, widths, vector<string>(), true);
	if(index - 1) print_info(os, index, widths, vector<string>());
}

vector<SymbolTableEntry*> SymbolTable::get_sorted_entries() {
	static constexpr auto comp = [](SymbolTableEntry* e1, SymbolTableEntry* e2) { return e1->line_location < e2->line_location; };
	vector<SymbolTableEntry*> sorted_entries;
	for (const auto& pair : entries) {
		SymbolTableEntry* entry = pair.second;
		sorted_entries.push_back(entry);
	}
	std::sort(sorted_entries.begin(), sorted_entries.end(), comp);
	return sorted_entries;
}

std::ostream& operator<<(std::ostream& os, SymbolTable& table) {
	//vector<vector<int>> spaces = table.get_spaces();
	table.print_table(os);
	//os << table.name;
	return os;
}


// SymbolTableEntry

/*
SymbolTableEntry::SymbolTableEntry(std::string name, Kind kind, std::string type) :
	name(name), kind(kind), type(type), link(nullptr), nextEntry(nullptr)
{

}
*/

SymbolTableEntry::SymbolTableEntry(std::string unique_id, std::string name, Kind kind, SymbolType* type, int line_location, Visibility visibility, SymbolTable* link) :
	unique_id(unique_id), name(name), kind(kind), type(type), link(link), line_location(line_location), nextEntry(nullptr), visibility(visibility)
{

}

SymbolTableEntry* SymbolTable::get_entry(std::string unique_id) const {
	if (!entries.count(unique_id)) return nullptr;

	SymbolTableEntry* entry = entries.at(unique_id);

	return entry;
}

SymbolTableEntry* SymbolTable::add_entry_if_new(SymbolTableEntry* entry) {
	if (!entry) return entry;
	SymbolTableEntry* duplicate_entry = get_entry(entry->unique_id);

	if (duplicate_entry) return duplicate_entry;

	this->add_entry(entry);
	return entry;
}

SymbolTableClassEntry::SymbolTableClassEntry(std::string unique_id, std::string name, Kind kind, SymbolType* type, int line_location, Visibility visibility, SymbolTable* link) :
	SymbolTableEntry(unique_id, name, kind, type, line_location, visibility, link) {

}