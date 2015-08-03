#include "BuildingRegistry.h"
// ------------------------------------------------------
// BuildingRegistry
// ------------------------------------------------------

// ------------------------------------------------------
// validate
// ------------------------------------------------------
bool BuildingRegistry::validate_data() {
	int doublet = 0;
	for ( size_t i = 0; i < _items.size(); ++i ) {
		Sign s = _items[i].sign;
		for ( size_t j = 0; j < _items.size(); ++j ) {
			if ( i != j ) {
				if ( s.compare(_items[j].sign)) {
					++doublet;
					LOGEC("BuildingRegistry") << "Found doublet at " << j << " and " << i << " : "  << s.c_str();
				}
			}		
		}
	}
	if ( doublet > 0 ) {
		LOGEC("BuildingRegistry") << "ERROR: found " << doublet << " doublets in building definitions";
	}
	return doublet == 0;
}

const char* BR_FIELD_NAMES[] = {"sign","size_x","size_y","permanent","regular","max_amount","destructable","name"};

const char** BuildingRegistry::get_field_names() const {		
	return BR_FIELD_NAMES;
}

int BuildingRegistry::get_field_num() const {
	return 8;
}
// ------------------------------------------------------
// load entry
// ------------------------------------------------------
bool BuildingRegistry::load_entry(const RegistryReader& reader,int index,BuildingDefinition* def) {
	def->id = index;
	def->sign = reader.get_sign(index,"sign");
	def->size_x = reader.get_int(index,"size_x"); 
	def->size_y = reader.get_int(index,"size_y");
	def->permanent = reader.get_bool(index,"permanent");
	def->regular = reader.get_bool(index,"regular");
	def->max_count = reader.get_int(index,"max_count");
	def->destructable = reader.get_bool(index,"destructable");
	reader.get_string(index,"name",def->name);
	return true;
}

const char* BuildingRegistry::getSign(int id) const {
	int idx = getIndex(id);
	if (idx != -1) {
		return _items[idx].sign.c_str();
	}
	return "--";
}

const char* BuildingRegistry::getName(int id) const {
	int idx = getIndex(id);
	if (idx != -1) {
		return _items[idx].name;
	}
	return "UNKNOWN";
}

const bool BuildingRegistry::runs_permanent(int id) const {
	int idx = getIndex(id);
	if (idx != -1) {
		return _items[idx].permanent;
	}
	return false;
}

const int BuildingRegistry::getIndex(const Sign& s) const {
	for (size_t i = 0; i < _items.size(); ++i) {
		if (_items[i].sign.compare(s) ) {
			return i;
		}
	}
	return -1;
}

const int BuildingRegistry::getIndex(const char* c) const {
	for (size_t i = 0; i < _items.size(); ++i) {
		if (_items[i].sign.compare(c) ) {
			return i;
		}
	}
	return -1;
}

const int BuildingRegistry::getIndex(int id) const {
	for (size_t i = 0; i < _items.size(); ++i) {
		if (_items[i].id == id) {
			return i;
		}
	}
	return -1;
}

bool BuildingRegistry::getDefinition(const Sign& s, BuildingDefinition* definition) {
	int id = getIndex(s);
	return getDefinition(id,definition);
}

bool BuildingRegistry::getDefinition(int id, BuildingDefinition* definition) {
	int idx = getIndex(id);
	if ( idx != -1 ) {
		const BuildingDefinition& def = _items[idx];
		definition->id = def.id;
		definition->sign = def.sign;
		definition->permanent = def.permanent;
		definition->regular = def.permanent;
		definition->max_count = def.max_count;
		definition->destructable = def.destructable;
		for ( int i = 0; i < 32; ++i ) {
			definition->name[i] = def.name[i];
		}
		definition->size_x = def.size_x;
		definition->size_y = def.size_y;
		return true;
	}
	return false;
}

void BuildingRegistry::show() {
	for (size_t i = 0; i < _items.size(); ++i) {
		printf("%d %s %s\n",_items[i].id,_items[i].sign.c_str(),_items[i].name);
	}
}

const bool BuildingRegistry::isDestructable(int id) const {
	return _items[id].destructable;
}

