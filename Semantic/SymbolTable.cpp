#include "SymbolTable.h"

using std::string;

SymbolTableEntry::SymbolTableEntry(std::string name, Kind kind, std::string type) :
	name(name), kind(kind), type(type), link(nullptr), nextEntry(nullptr)
{

}

SymbolTableEntry::SymbolTableEntry(std::string name, Kind kind, std::string type, SymbolTable* link) :
	name(name), kind(kind), type(type), link(link), nextEntry(nullptr)
{

}