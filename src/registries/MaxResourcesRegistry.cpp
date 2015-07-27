#include "MaxResourcesRegistry.h"
#include "ResourceRegistry.h"

const char* MR_FIELD_NAMES[] =  {"building","level","resource","amount"};

const char** MaxResourcesRegistry::get_field_names() const {		
	return MR_FIELD_NAMES;
}

int MaxResourcesRegistry::get_field_num() const {
	return 4;
}
// ------------------------------------------------------
// ResourceRegistry
// ------------------------------------------------------
bool MaxResourcesRegistry::load_entry(const RegistryReader& reader,int index,MaxResourceDefinition* def) {
	Sign s = reader.get_sign(index,"building");
	def->building_id = _building_registry->getIndex(s);
	if ( def->building_id == -1 ) {
		printf("ERROR: Invalid building %s at line %d\n",s.c_str(),reader.get_line_nr(index));
		return false;
	}
	else {
		def->level = reader.get_int(index,"level");
		Sign res = reader.get_sign(index,"resource");
		def->resource_id = _resource_registry->getIndex(res);
		if ( def->resource_id != -1 ) {
			def->amount = reader.get_int(index,"amount");
			return true;
		}
		else {
			printf("ERROR: Invalid resource definition %s at line%d\n",res.c_str(),reader.get_line_nr(index));
			return false;
		}
	}
}

void MaxResourcesRegistry::add(int building_id, int level, Resources* res) {
	for (size_t i = 0; i < _items.size(); ++i) {
		MaxResourceDefinition& def = _items[i];
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
