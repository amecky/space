#include "PriceRegistry.h"
#include <string.h>
#include <fstream>
#include <string>
#include <vector>
#include "files.h"
#include "CSVFile.h"

// ------------------------------------------------------
// ResourceRegistry
// ------------------------------------------------------
void ResourceRegistry::load() {
	// MO,Y,Money
	CSVFile file;
	if ( file.load("resource_definitions.txt","data") ) {
		for ( size_t i = 0; i < file.size(); ++i ) {
			const TextLine& l = file.get(i);
			ResourceDefinition def;
			def.id = i;
			def.sign = l.get_sign(0);
			def.global = l.get_bool(1); 			
			l.get_string(2,def.name);
			_definitions.push_back(def);
		}
	}	
	printf("resource defintions: %d\n", _definitions.size());
}

const bool ResourceRegistry::isGlobal(int id) const {
	return _definitions[id].global;
}

const char* ResourceRegistry::getName(int id) const {
	if ( id >= 0 && id < _definitions.size()) {
		return _definitions[id].name;
	}
	return "UNKNOWN";
}

const int ResourceRegistry::getIndex(const Sign& c) const {
	for ( size_t i = 0; i < _definitions.size(); ++i ) {
		if ( _definitions[i].sign.compare(c) ) {
			return i;
		}
	}
	return -1;
}

// ------------------------------------------------------
// BuildingRegistry
// ------------------------------------------------------

// ------------------------------------------------------
// load
// ------------------------------------------------------
void BuildingRegistry::load() {
	CSVFile file;
	// HB,2,2,N,Y,1,S,HomeBase
	if ( file.load("buildings.txt","data") ) {
		for ( size_t i = 0; i < file.size(); ++i ) {
			const TextLine& l = file.get(i);
			BuildingDefinition def;
			def.id = i;
			def.sign = l.get_sign(0);
			def.size_x = l.get_int(1); 
			def.size_y = l.get_int(2);
			def.permanent = l.get_bool(3);
			def.regular = l.get_bool(4);
			def.max_count = l.get_int(5);
			def.destructable = l.get_bool(6);
			l.get_string(7,def.name);
			_definitions.push_back(def);
		}
	}	
	// verify data
	int doublet = 0;
	for ( size_t i = 0; i < _definitions.size(); ++i ) {
		Sign s = _definitions[i].sign;
		for ( size_t j = 0; j < _definitions.size(); ++j ) {
			if ( i != j ) {
				if ( s.compare(_definitions[j].sign)) {
					++doublet;
					printf("Found doublet at %d and %d : %s\n",j,i,s.c_str());
				}
			}		
		}
	}
	if ( doublet > 0 ) {
		printf("ERROR: found %d doublets in building definitions\n",doublet);
	}
	printf("building defintions: %d\n", _definitions.size());
}

const char* BuildingRegistry::getSign(int id) const {
	int idx = getIndex(id);
	if (idx != -1) {
		return _definitions[idx].sign.c_str();
	}
	return "--";
}

const char* BuildingRegistry::getName(int id) const {
	int idx = getIndex(id);
	if (idx != -1) {
		return _definitions[idx].name;
	}
	return "UNKNOWN";
}

const bool BuildingRegistry::runs_permanent(int id) const {
	int idx = getIndex(id);
	if (idx != -1) {
		return _definitions[idx].permanent;
	}
	return false;
}

const int BuildingRegistry::getIndex(const Sign& s) const {
	for (size_t i = 0; i < _definitions.size(); ++i) {
		if (_definitions[i].sign.compare(s) ) {
			return i;
		}
	}
	return -1;
}

const int BuildingRegistry::getIndex(const char* c) const {
	for (size_t i = 0; i < _definitions.size(); ++i) {
		if (_definitions[i].sign.compare(c) ) {
			return i;
		}
	}
	return -1;
}

const int BuildingRegistry::getIndex(int id) const {
	for (size_t i = 0; i < _definitions.size(); ++i) {
		if (_definitions[i].id == id) {
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
		const BuildingDefinition& def = _definitions[idx];
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
	for (size_t i = 0; i < _definitions.size(); ++i) {
		printf("%d %s %s\n",_definitions[i].id,_definitions[i].sign.c_str(),_definitions[i].name);
	}
}

const bool BuildingRegistry::isDestructable(int id) const {
	return _definitions[id].destructable;
}

// ------------------------------------------------------
// PriceRegistry
// ------------------------------------------------------

// ------------------------------------------------------
// constructor
// ------------------------------------------------------
PriceRegistry::PriceRegistry(ResourceRegistry* res_reg,BuildingRegistry* bld_reg) 
	: _resource_registry(res_reg) , _building_registry(bld_reg) {
}

// ------------------------------------------------------
// destructor
// ------------------------------------------------------
PriceRegistry::~PriceRegistry() {
}

// ------------------------------------------------------
// load registry
// ------------------------------------------------------
void PriceRegistry::load(const char* fileName) {
	//HT,1,B,S,60,WO,1
	CSVFile file;
	if ( file.load(fileName,"data") ) {
		for ( size_t i = 0; i < file.size(); ++i ) {
			const TextLine& l = file.get(i);
			RegistryEntry entry;
			Sign s = l.get_sign(0);
			entry.building_type = _building_registry->getIndex(s);
			entry.level = l.get_int(1);
			entry.price_type = -1;
			char pt = l.get_char(2);
			for (int i = 0; i < 6; ++i) {
				if (pt == PRICE_TYPES[i]) {
					entry.price_type = i;
				}
			}
			char flag = l.get_char(3);
			if ( flag == 'S' ) {
				entry.stage = 0;
			}
			else if (flag == 'F') {
				entry.stage = 1;
			}
			else {
				entry.stage = 2;
			}
			entry.duration = l.get_int(4);
			Sign r = l.get_sign(5);
			entry.resource_id = _resource_registry->getIndex(r);
			entry.amount = l.get_int(6);
			//printf("-> entry %d %d %d\n",entry.building_type,entry.level,entry.price_type);
			_entries.push_back(entry);
		}
	}		
	printf("entries: %d\n",_entries.size());
}

// ------------------------------------------------------
// load registry
// ------------------------------------------------------
void PriceRegistry::load_requirements() {
	// HB,2,HT,3,3
	CSVFile file;
	if ( file.load("requirements.txt","data") ) {
		for ( size_t i = 0; i < file.size(); ++i ) {
			const TextLine& l = file.get(i);
			BuildRequirement br;
			Sign s = l.get_sign(0);
			br.building_type = _building_registry->getIndex(s);
			br.level = l.get_int(1);
			Sign r = l.get_sign(2);
			br.required_building = _building_registry->getIndex(r);
			br.required_level = l.get_int(3);
			br.required_count = l.get_int(4);
			_requirements.push_back(br);      
		}
	}	
	printf("requirements: %d\n",_requirements.size());
}

// ------------------------------------------------------
// load max resources
// ------------------------------------------------------
void PriceRegistry::load_max_resources() {
	CSVFile file;
	if ( file.load("max_resources.txt","data") ) {
		for ( size_t i = 0; i < file.size(); ++i ) {
			const TextLine& l = file.get(i);
			MaxResourceDefinition def;
			// HB,1,WD,500 
			Sign s = l.get_sign(0);
			def.building_id = _building_registry->getIndex(s);
			def.level = l.get_int(1);
			Sign r = l.get_sign(2);
			def.resource_id = _resource_registry->getIndex(r);
			def.amount = l.get_int(3);
			_max_resources.push_back(def);
		}		
	}
	printf("max resource definition: %d\n", _max_resources.size());
}

// ------------------------------------------------------
// add to maximum resources
// ------------------------------------------------------
void PriceRegistry::add_max_resources(int building_id, int level, Resources* res) {
	for (size_t i = 0; i < _max_resources.size(); ++i) {
		MaxResourceDefinition& def = _max_resources[i];
		if (def.building_id == building_id && def.level == level) {
			if ( res->_values[def.resource_id] == -1 ) {
				res->_values[def.resource_id] = def.amount;
			}
			else {
				res->_values[def.resource_id] += def.amount;
			}
		}
	}
}
// ------------------------------------------------------
// get index
// ------------------------------------------------------
int PriceRegistry::getIndex(int price_type, int stage, int building_type, int level) {
	printf("get index %d %d %d\n",price_type,stage,building_type,level);
	for ( size_t i = 0; i < _entries.size(); ++i ) {
		RegistryEntry& entry = _entries[i];
		if (entry.price_type == price_type && entry.level == level && entry.building_type == building_type && entry.stage == stage) {
			return i;
		}
	}
	return -1;
}

// ------------------------------------------------------
// get costs
// ------------------------------------------------------
bool PriceRegistry::get(int price_type,int stage,int building_type,int level,Resources* resources) {
	bool found = false;
	for ( size_t i = 0; i < _entries.size(); ++i ) {
		RegistryEntry& entry = _entries[i];
		if (entry.price_type == price_type && entry.level == level && entry.building_type == building_type && entry.stage == stage) {
			resources->set(entry.resource_id,entry.amount);
			found = true;
		}
		
	}
	return found;
}

// ------------------------------------------------------
// get duration
// ------------------------------------------------------
int PriceRegistry::getDuration(int price_type, int building_type, int level) {
	int idx = getIndex(price_type,00,building_type,level);
	if ( idx != -1 ) {
		return _entries[idx].duration;
	}
	else {
		printf("Error: Unable to determine duration\n");
	}
	return -1;
}

// ------------------------------------------------------
// get requirements
// ------------------------------------------------------
bool PriceRegistry::getRequirement(int building_id, int level, BuildRequirement* requirement) {
	for (size_t i = 0; i < _requirements.size(); ++i) {
		if (_requirements[i].building_type == building_id && _requirements[i].level == level) {
			requirement->building_type = building_id;
			requirement->level = level;
			requirement->required_building = _requirements[i].required_building;
			requirement->required_count = _requirements[i].required_count;
			requirement->required_level = _requirements[i].required_level;
			return true;
		}
	}
	return false;
}