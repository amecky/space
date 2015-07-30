#include "PriceRegistry.h"
#include <string.h>
#include <fstream>
#include <string>
#include <vector>
#include "..\utils\files.h"
#include "RegistryReader.h"
#include "ResourceRegistry.h"
#include "..\utils\utils.h"
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

const char* PR_FIELD_NAMES[] = {"building","level","work","stage","duration","resource","amount"};

const char** PriceRegistry::get_field_names() const {
	return PR_FIELD_NAMES;
}

int PriceRegistry::get_field_num() const {
	return 7;
}

bool PriceRegistry::load_entry(const RegistryReader& reader,int index,RegistryEntry* entry) {
	bool valid = true;
	Sign s = reader.get_sign(index,"building");
	entry->building_type = _building_registry->getIndex(s);
	if (entry->building_type == -1) {
		printf("ERROR: invalid building type at line %d\n", reader.get_line_nr(index));
		valid = false;
	}
	entry->level = reader.get_int(index,"level");
	entry->price_type = -1;
	char pt = reader.get_char(index,"work");
	entry->price_type = reg::translate_work(pt);	
	if ( entry->price_type == -1 ) {
		printf("ERROR: invalid work %c at line %d\n",pt,reader.get_line_nr(index));
		valid = false;
	}
	char flag = reader.get_char(index,"stage");
	entry->stage = reg::translate_stage(flag);
	/*
	if ( flag == 'S' ) {
		entry->stage = 0;
	}
	else if (flag == 'F') {
		entry->stage = 1;
	}
	else {
		entry->stage = 2;
	}
	*/
	entry->duration = reader.get_int(index,"duration");
	Sign res = reader.get_sign(index,"resource");
	entry->resource_id = _resource_registry->getIndex(res);
	if (entry->resource_id == -1) {
		printf("ERROR: invalid resource type at line %d\n", reader.get_line_nr(index));
		valid = false;
	}
	entry->amount = reader.get_int(index,"amount");
	return valid;
}

// ------------------------------------------------------
// get index
// ------------------------------------------------------
int PriceRegistry::getIndex(int price_type, int stage, int building_type, int level) {
	//printf("get index %s %d %d\n",reg::translate_work(price_type),stage,building_type,level);
	for ( size_t i = 0; i < _items.size(); ++i ) {
		RegistryEntry& entry = _items[i];
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
	//printf("get index %s %s %d %d\n",reg::translate_work(price_type),reg::translate_stage(stage),building_type,level);
	for ( size_t i = 0; i < _items.size(); ++i ) {
		RegistryEntry& entry = _items[i];
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
		return _items[idx].duration;
	}
	else {
		printf("Error: Unable to determine duration\n");
	}
	return -1;
}
