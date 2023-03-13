#include "SymbolTable.h"
#include <sstream>
#include <iostream>
#include <algorithm>

using std::string;
typedef SymbolTableEntry STE;
using std::vector;
using std::endl;

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

std::ostream& operator<<(std::ostream& os, const SymbolType& type) {
	os << type.type_id;
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

vector<vector<int>> SymbolTable::get_spaces() const {
	vector<vector<int>> widths;
	for (const auto& entry : entries) {

	}
	return widths;
}

void SymbolTable::print_table(std::ostream& os, std::string prefix) const {

	static constexpr auto comp = [](SymbolTableEntry* e1, SymbolTableEntry* e2) { return e1->line_location < e2->line_location; };

	os << prefix << "table: " << name << endl;
	prefix += '\t';
	vector<SymbolTableEntry*> sorted_entries;
	for (const auto& pair : entries) {
		SymbolTableEntry* entry = pair.second;
		sorted_entries.push_back(entry);
	}
	std::sort(sorted_entries.begin(), sorted_entries.end(), comp);

	for (SymbolTableEntry* entry : sorted_entries) {
		std::string kind_string = SymbolTableEntry::kind_strings.at(entry->kind);
		os << prefix << kind_string << "\t| " << entry->name;

		SymbolType* type = entry->type;
		STE::Visibility visibility = entry->visibility;

		if (type) {
			os << "\t| " << *type;
		}
		if (visibility != STE::Visibility::None) {
			os << "\t| " << STE::visibility_strings.at(visibility);
		}
		os << endl;

		if (entry->link) entry->link->print_table(os, prefix + '\t');
	}
}

std::ostream& operator<<(std::ostream& os, const SymbolTable& table) {
	//vector<vector<int>> spaces = table.get_spaces();
	table.print_table(os, "");
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

SymbolTableEntry* SymbolTable::get_entry(std::string unique_id) {
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