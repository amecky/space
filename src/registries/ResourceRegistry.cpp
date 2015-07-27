#include "ResourceRegistry.h"

const char* RR_FIELD_NAMES[] =  {"sign","global","name"};

const char** ResourceRegistry::get_field_names() const {		
	return RR_FIELD_NAMES;
}

int ResourceRegistry::get_field_num() const {
	return 3;
}
// ------------------------------------------------------
// ResourceRegistry
// ------------------------------------------------------
bool ResourceRegistry::load_entry(const RegistryReader& reader,int index,ResourceDefinition* def) {
	def->id = index;
	def->sign = reader.get_sign(index,"sign");
	def->global = reader.get_bool(index,"global"); 			
	reader.get_string(index,"name",def->name);
	return true;
}

const bool ResourceRegistry::isGlobal(int id) const {
	return _items[id].global;
}

const char* ResourceRegistry::getName(int id) const {
	if ( id >= 0 && id < _items.size()) {
		return _items[id].name;
	}
	return "UNKNOWN";
}

const int ResourceRegistry::getIndex(const Sign& c) const {
	for ( size_t i = 0; i < _items.size(); ++i ) {
		if ( _items[i].sign.compare(c) ) {
			return i;
		}
	}
	return -1;
}
