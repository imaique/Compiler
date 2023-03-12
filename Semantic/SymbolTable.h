#pragma once
#include <string>
#include <unordered_map>
#include <fstream>

class SymbolTable;

class SymbolType {

public:
	const std::string type_id;
	const std::vector<int> dimensions;
	SymbolType(std::string type_id, std::vector<int> dimensions);

	friend std::ostream& operator<<(std::ostream& os, const SymbolType& type);

	virtual ~SymbolType() = default;
};

class FunctionSymbolType : public SymbolType {
	std::string get_signature(std::vector<SymbolType> param_types);
public:
	std::vector<SymbolType> param_types;
	const std::string signature;

	FunctionSymbolType(std::string return_type, std::vector<SymbolType> param_types);

	friend std::ostream& operator<<(std::ostream& os, const FunctionSymbolType& type);
	
	~FunctionSymbolType() = default;
	//std::string get_signature();
};


class SymbolTableEntry {
public:
	enum class Kind {
		FuncDecl,
		FuncDef,
		Class,
		Parameter,
		Data,
		Localvar,
		Inherit
	};
	static const std::unordered_map<Kind, std::string> kind_strings;
	enum class Visibility {
		None,
		Public,
		Private
	};
	static const std::unordered_map<Visibility, std::string> visibility_strings;
	std::string unique_id;
	std::string name;
	Kind kind;
	SymbolType* type;
	Visibility visibility;
	int line_location;
	SymbolTable* link;
	// In case of name collision
	SymbolTableEntry* nextEntry;

	//SymbolTableEntry(std::string name, Kind kind, std::string type);
	SymbolTableEntry(std::string unique_id, std::string name, Kind kind, SymbolType* type, int line_location, Visibility visibility, SymbolTable* link);

};

class SymbolTable {
	
	std::unordered_map<std::string, SymbolTableEntry*> entries;
	std::vector<std::vector<int>> get_spaces() const;
	void print_table(std::ostream& os, std::string prefix) const;

public:
	SymbolTableEntry* get_entry(std::string name);
	SymbolTableEntry* add_entry_if_new(SymbolTableEntry* entry);
	void add_entry(SymbolTableEntry* entry);

	const std::string name;
	SymbolTable(std::string name);

	friend std::ostream& operator<<(std::ostream& os, const SymbolTable& table);
};





class SymbolTableClassEntry : SymbolTableEntry {
	std::unordered_map <std::string, std::string> inherited_members;
};