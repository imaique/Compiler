#pragma once
#include <string>
#include <unordered_map>
#include <fstream>
#include <vector>

class SymbolTable;

class SymbolType {
public:
	static const SymbolType INVALID;
	static const SymbolType OK;
	static const SymbolType INTEGER;
	static const SymbolType FLOAT;
	static const SymbolType VOID;

	const std::string type_id;
	const std::vector<int> dimensions;
	SymbolType(std::string type_id, std::vector<int> dimensions);

	bool operator==(const SymbolType& other) const;
	bool operator!=(const SymbolType& other) const;

	friend std::ostream& operator<<(std::ostream& os, const SymbolType& type);

	virtual ~SymbolType() = default;
};

class FunctionSymbolType : public SymbolType {
	
public:
	static std::string get_signature(std::vector<SymbolType> param_types);
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
		Inherit,
		TempVar
	};
	int get_scope_size() const;
	int scope_size = -1;
	int offset = -1;
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
	AST* node;

	//SymbolTableEntry(std::string name, Kind kind, std::string type);
	SymbolTableEntry(std::string unique_id, std::string name, Kind kind, SymbolType* type, int line_location, Visibility visibility, SymbolTable* link, AST* node);

	
};

class SymbolTable {
	
	
	void get_spaces(int index, std::vector<int>& widths);
	
	void print_table(std::ostream& os, int index, const std::vector<int>& widths);
	std::vector<SymbolTableEntry*> get_sorted_entries();
public:
	void print_table(std::ostream& os);
	std::unordered_map<std::string, SymbolTableEntry*> entries;
	SymbolTableEntry* get_entry(std::string name) const;
	SymbolTableEntry* add_entry_if_new(SymbolTableEntry* entry);
	void add_entry(SymbolTableEntry* entry);
	int scope_size = -1;
	const std::string name;
	SymbolTable(std::string name);

	friend std::ostream& operator<<(std::ostream& os, SymbolTable& table);
};





class SymbolTableClassEntry : public SymbolTableEntry {
public:
	std::unordered_map <std::string, SymbolTableEntry*> inherited_members;
	SymbolTableClassEntry(std::string unique_id, std::string name, Kind kind, SymbolType* type, int line_location, Visibility visibility, SymbolTable* link, AST* node);
};
