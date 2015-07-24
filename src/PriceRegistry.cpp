#include "PriceRegistry.h"
#include <string.h>
#include <fstream>
#include <string>
#include <vector>
#include "files.h"
#include "CSVFile.h"
#include "RegistryReader.h"

// ------------------------------------------------------
// ResourceRegistry
// ------------------------------------------------------
void ResourceRegistry::load() {
	const char* names[] = {"sign","global","name"};
	RegistryReader r(names,3);
	if ( r.load("resource_definitions.txt","data") ) {
		for ( int i = 0; i < r.size(); ++i ) {
			ResourceDefinition def;
			def.id = i;
			def.sign = r.get_sign(i,"sign");
			def.global = r.get_bool(i,"global"); 			
			r.get_string(i,"name",def.name);
			_items.push_back(def);
		}
	}	
	printf("resource defintions: %d\n", _items.size());
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

// ------------------------------------------------------
// Task registry
// ------------------------------------------------------

// ------------------------------------------------------
// load
// ------------------------------------------------------
void TaskRegistry::load() {
	// island : 0 , job_id : 1 , type : B , building : HT , level : 3 , amount : 2 , text : "Build 2 Huts"
	const char* names[] = {"island","job_id","type","building","level","amount","text"};
	RegistryReader r(names,7);
	if ( r.load("tasks.txt","data") ) {
		for ( int i = 0; i < r.size(); ++i ) {
			bool valid = true;
			Task t;
			t.island = r.get_int(i,"island");
			t.job_id = r.get_int(i,"job_id");
			char type = r.get_char(i,"type");
			Sign bs = r.get_sign(i,"building");
			t.building_id = _building_registry->getIndex(bs);
			if (t.building_id == -1) {
				printf("ERROR: invalid building type at %d\n", i);
				valid = false;
			}
			t.level = r.get_int(i,"level");
			t.amount = r.get_int(i,"amout");			
			r.get_string(i,"text",t.text);
			if ( valid ) {
				_items.push_back(t);
			}
		}
	}	
	printf("tasks: %d\n", _items.size());
}

// ------------------------------------------------------
// Island registry
// ------------------------------------------------------

// ------------------------------------------------------
// load
// ------------------------------------------------------
void IslandRegistry::load() {
	// island : 0 , area : 0 , size_x : 32 , size_y : 32 , start_x :  0 , start_y :  0 , costs : MO , amount : 7000 , locked : N , file : "in_0_ar_0"
	const char* names[] = {"island","area","size_x","size_y","start_x","start_y","costs","amount","locked","file"};
	RegistryReader r(names,10);
	if ( r.load("islands.txt","data") ) {
		for ( int i = 0; i < r.size(); ++i ) {
			AreaDefinition def;
			def.island = r.get_int(i,"island");
			def.area = r.get_int(i,"area");
			def.size_x = r.get_int(i,"size_x");
			def.size_y = r.get_int(i,"size_y");
			def.start_x = r.get_int(i,"start_x");
			def.start_y = r.get_int(i,"start_y");
			def.locked = r.get_bool(i,"locked");
			r.get_string(i,"file",def.file);
			_items.push_back(def);
		}
	}
	printf("areas: %d\n", _items.size());
}


// ------------------------------------------------------
// BuildingRegistry
// ------------------------------------------------------

// ------------------------------------------------------
// load
// ------------------------------------------------------
void BuildingRegistry::load() {
	const char* names[] = {"sign","size_x","size_y","permanent","regular","max_amount","destructable","name"};
	RegistryReader r(names,8);
	if ( r.load("buildings.txt","data") ) {
		for ( int i = 0; i < r.size(); ++i ) {
			BuildingDefinition def;
			def.id = i;
			def.sign = r.get_sign(i,"sign");
			def.size_x = r.get_int(i,"size_x"); 
			def.size_y = r.get_int(i,"size_y");
			def.permanent = r.get_bool(i,"permanent");
			def.regular = r.get_bool(i,"regular");
			def.max_count = r.get_int(i,"max_count");
			def.destructable = r.get_bool(i,"destructable");
			r.get_string(i,"name",def.name);
			_items.push_back(def);
		}
	}
	// verify data
	int doublet = 0;
	for ( size_t i = 0; i < _items.size(); ++i ) {
		Sign s = _items[i].sign;
		for ( size_t j = 0; j < _items.size(); ++j ) {
			if ( i != j ) {
				if ( s.compare(_items[j].sign)) {
					++doublet;
					printf("Found doublet at %d and %d : %s\n",j,i,s.c_str());
				}
			}		
		}
	}
	if ( doublet > 0 ) {
		printf("ERROR: found %d doublets in building definitions\n",doublet);
	}
	printf("building defintions: %d\n", _items.size());
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

void PriceRegistry::load() {
	char buffer[256];
	for ( size_t i = 0; i < _building_registry->size(); ++i ) {
		const BuildingDefinition& def = _building_registry->get(i);
		sprintf(buffer,"%s.txt",def.sign.c_str());
		load(buffer);
	}
}
// ------------------------------------------------------
// load registry
// ------------------------------------------------------
void PriceRegistry::load(const char* fileName) {	
	const char* names[] = {"building","level","work","stage","duration","resource","amount"};
	RegistryReader r(names,7);
	if ( r.load(fileName,"data") ) {
		printf("loading: %s\n",fileName);
		for ( int i = 0; i < r.size(); ++i ) {
			bool valid = true;
			RegistryEntry entry;
			Sign s = r.get_sign(i,"building");
			entry.building_type = _building_registry->getIndex(s);
			if (entry.building_type == -1) {
				printf("ERROR: invalid building type at %d\n", i);
				valid = false;
			}
			entry.level = r.get_int(i,"level");
			entry.price_type = -1;
			char pt = r.get_char(i,"work");
			for (int j = 0; j < 6; ++j) {
				if (pt == PRICE_TYPES[j]) {
					entry.price_type = j;
				}
			}
			if ( entry.price_type == -1 ) {
				printf("ERROR: invalid work %c\n",pt);
				valid = false;
			}
			char flag = r.get_char(i,"stage");
			if ( flag == 'S' ) {
				entry.stage = 0;
			}
			else if (flag == 'F') {
				entry.stage = 1;
			}
			else {
				entry.stage = 2;
			}
			entry.duration = r.get_int(i,"duration");
			Sign res = r.get_sign(i,"resource");
			entry.resource_id = _resource_registry->getIndex(res);
			if (entry.resource_id == -1) {
				printf("ERROR: invalid resource type at %d\n", i);
				valid = false;
			}
			entry.amount = r.get_int(i,"amount");
			if ( valid ) {
				_entries.push_back(entry);
			}
		}
	}
}

// ------------------------------------------------------
// load registry
// ------------------------------------------------------
void PriceRegistry::load_requirements() {
	const char* names[] = {"building","level","required_building","required_level","required_amount"};
	RegistryReader r(names,5);
	if ( r.load("requirements.txt","data") ) {
		for ( int i = 0; i < r.size(); ++i ) {
			BuildRequirement br;
			Sign s = r.get_sign(i,"building");
			br.building_type = _building_registry->getIndex(s);
			if ( br.building_type != -1 ) {
				br.level = r.get_int(i,"level");
				Sign req = r.get_sign(i,"required_building");
				br.required_building = _building_registry->getIndex(req);
				if ( br.required_building != -1 ) {
					br.required_level = r.get_int(i,"required_level");
					br.required_count = r.get_int(i,"required_amount");
					_requirements.push_back(br);   
				}
				else {
					printf("ERROR: invalid required building %s at line %d\n",s.c_str(),r.get_line_nr(i));
			}
			}
			else {
				printf("ERROR: invalid building %s at line %d\n",s.c_str(),r.get_line_nr(i));
			}
		}
	}	
	printf("requirements: %d\n",_requirements.size());
}

// ------------------------------------------------------
// load max resources
// ------------------------------------------------------
void PriceRegistry::load_max_resources() {
	// building : HB , level : 1 , resource : WD , amount : 500 
	const char* names[] = {"building","level","resource","amount"};
	RegistryReader r(names,4);
	if ( r.load("max_resources.txt","data") ) {
		for ( int i = 0; i < r.size(); ++i ) {
			MaxResourceDefinition def;
			// HB,1,WD,500 
			Sign s = r.get_sign(i,"building");
			def.building_id = _building_registry->getIndex(s);
			if ( def.building_id == -1 ) {
				printf("ERROR: Invalid building %s\n",s.c_str());
			}
			else {
				def.level = r.get_int(i,"level");
				Sign res = r.get_sign(i,"resource");
				def.resource_id = _resource_registry->getIndex(res);
				if ( def.resource_id != -1 ) {
					def.amount = r.get_int(i,"amount");
					_max_resources.push_back(def);
				}
				else {
					printf("ERROR: Invalid resource definition %s\n",res.c_str());
				}
			}
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
	//printf("get index %d %d %d\n",price_type,stage,building_type,level);
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