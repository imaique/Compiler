#pragma once
#include <string>
#include <unordered_map>

class SymbolTable {
	
	std::unordered_map<std::string, SymbolTableEntry*> entries;

public:
	SymbolTableEntry* get_entry(std::string name, SymbolTableEntry::Kind kind);
	bool add_entry(SymbolTableEntry* entry);

	const std::string name;
};



class SymbolTableEntry {
public:
	enum class Kind {
		Function,
		Class,
		Parameter,
		Variable,
	};
	std::string name;
	Kind kind;
	std::string type;
	SymbolTable* link;
	// In case of name collision
	SymbolTableEntry* nextEntry;

	SymbolTableEntry(std::string name, Kind kind, std::string type);
	SymbolTableEntry(std::string name, Kind kind, std::string type, SymbolTable* link);
};