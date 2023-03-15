#pragma once
#include "SymbolTable.h"

class Decorator {
	SymbolType* type;
public:
	Decorator::Decorator();
	bool type_resolved;
	SymbolTableEntry* class_entry;
	SymbolTableEntry* function_entry;
	void set_type(SymbolType type);
	SymbolType get_type();
};