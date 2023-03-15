#include "Decorator.h"

Decorator::Decorator() : type_resolved(false), class_entry(nullptr), function_entry(nullptr) {

}

SymbolType Decorator::get_type() {
	return *type;
}
void Decorator::set_type(SymbolType type) {
	type_resolved = true;
	this->type = new SymbolType(type);
}