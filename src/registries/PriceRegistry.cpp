#include "PriceRegistry.h"
#include <string.h>
#include <fstream>
#include <string>
#include <vector>
#include "..\utils\files.h"
#include "RegistryReader.h"
#include "ResourceRegistry.h"
#include "..\utils\utils.h"
#include "..\utils\Log.h"
#include "..\Tiles.h"

// ------------------------------------------------------
// constructor
// ------------------------------------------------------
PriceRegistry::PriceRegistry(ResourceRegistry* res_reg,BuildingRegistry* bld_reg) 
	: _resource_registry(res_reg) , _building_registry(bld_reg) {

	_definitions[0] = PriceDefinition(PT_WORK,'W',"Work");
	_definitions[1] = PriceDefinition(PT_UPGRADE,'U',"Update");
	_definitions[2] = PriceDefinition(PT_DELETE,'D',"Delete");
	_definitions[3] = PriceDefinition(PT_BUILD,'B',"Build");
	_definitions[4] = PriceDefinition(PT_PERMANENT,'P',"Permanent");
	_definitions[5] = PriceDefinition(PT_REGULAR,'R',"Regular");
	_definitions[6] = PriceDefinition(PT_UNKNOWN,'-',"Unknown");
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

const char* PriceRegistry::translateWorkType(WorkType work_type) {
	for ( int i = 0; i < PT_ITEM_COUNT; ++i ) {
		if ( _definitions[i].type == work_type ) {
			return _definitions[i].name;
		}
	}
	return _definitions[PT_ITEM_COUNT-1].name;
}

WorkType PriceRegistry::findBySign(char s) {
	for ( int i = 0; i < PT_ITEM_COUNT; ++i ) {
		if ( _definitions[i].sign == s ) {
			return _definitions[i].type;
		}
	}
	return PT_UNKNOWN;
}

bool PriceRegistry::load_entry(const RegistryReader& reader,int index,RegistryEntry* entry) {
	bool valid = true;
	Sign s = reader.get_sign(index,"building");
	entry->building_type = _building_registry->getIndex(s);
	if (entry->building_type == -1) {
		LOGEC("PriceRegistry") << "Invalid building type at line " << reader.get_line_nr(index);
		valid = false;
	}
	entry->level = reader.get_int(index,"level");
	entry->work_type = PT_UNKNOWN;
	char pt = reader.get_char(index,"work");
	entry->work_type = findBySign(pt);	
	if ( entry->work_type == PT_UNKNOWN ) {
		LOGEC("PriceRegistry") << "Invalid work " << pt << " at line " << reader.get_line_nr(index);
		valid = false;
	}
	char flag = reader.get_char(index,"stage");
	entry->stage = reg::translate_stage(flag);	
	entry->duration = reader.get_int(index,"duration");
	Sign res = reader.get_sign(index,"resource");
	entry->resource_id = _resource_registry->getIndex(res);
	if (entry->resource_id == -1) {
		LOGEC("PriceRegistry") << "Invalid resource type at line " << reader.get_line_nr(index);
		valid = false;
	}
	entry->amount = reader.get_int(index,"amount");
	return valid;
}

// ------------------------------------------------------
// get index
// ------------------------------------------------------
int PriceRegistry::getIndex(WorkType work_type, int stage, int building_type, int level) {
	//printf("get index %s %d %d\n",reg::translate_work(price_type),stage,building_type,level);
	for ( size_t i = 0; i < _items.size(); ++i ) {
		RegistryEntry& entry = _items[i];
		if (entry.work_type == work_type && entry.level == level && entry.building_type == building_type && entry.stage == stage) {
			return i;
		}
	}
	return -1;
}

// ------------------------------------------------------
// get costs
// ------------------------------------------------------
bool PriceRegistry::get(WorkType work_type,int stage,int building_type,int level,Resources* resources) {
	bool found = false;
	//printf("get index %s %s %d %d\n",reg::translate_work(price_type),reg::translate_stage(stage),building_type,level);
	for ( size_t i = 0; i < _items.size(); ++i ) {
		RegistryEntry& entry = _items[i];
		if (entry.work_type == work_type && entry.level == level && entry.building_type == building_type && entry.stage == stage) {
			resources->set(entry.resource_id,entry.amount);
			found = true;
		}
		
	}
	return found;
}

bool PriceRegistry::get(WorkType work_type,int stage,const Tile& tile,Resources* resources) {
	return get(work_type,stage,tile.building_id,tile.level,resources);
}

// ------------------------------------------------------
// get duration
// ------------------------------------------------------
int PriceRegistry::getDuration(WorkType work_type, int building_type, int level) {
	int idx = getIndex(work_type,00,building_type,level);
	if ( idx != -1 ) {
		return _items[idx].duration;
	}
	else {
		LOGEC("PriceRegistry") << "Unable to determine duration";
	}
	return -1;
}
