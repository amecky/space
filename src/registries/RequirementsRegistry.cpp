#include "RequirementsRegistry.h"

const char* REQR_FIELD_NAMES[] =   {"building","level","required_building","required_level","required_amount"};

const char** RequirementsRegistry::get_field_names() const {		
	return REQR_FIELD_NAMES;
}

int RequirementsRegistry::get_field_num() const {
	return 5;
}
// ------------------------------------------------------
// ResourceRegistry
// ------------------------------------------------------
bool RequirementsRegistry::load_entry(const RegistryReader& reader,int index,BuildRequirement* def) {
	Sign s = reader.get_sign(index,"building");
	def->building_type = _building_registry->getIndex(s);
	if ( def->building_type != -1 ) {
		def->level = reader.get_int(index,"level");
		Sign req = reader.get_sign(index,"required_building");
		def->required_building = _building_registry->getIndex(req);
		if ( def->required_building != -1 ) {
			def->required_level = reader.get_int(index,"required_level");
			def->required_count = reader.get_int(index,"required_amount");
		}
		else {
			printf("ERROR: invalid required building %s at line %d\n",s.c_str(),reader.get_line_nr(index));
		}
	}
	else {
		printf("ERROR: invalid building %s at line %d\n",s.c_str(),reader.get_line_nr(index));
	}
	return true;
}

const bool RequirementsRegistry::contains(int building_id,int level) const {
	for (size_t i = 0; i < _items.size(); ++i) {
		if (_items[i].building_type == building_id && _items[i].level == level) {
			return true;
		}
	}
	return false;
}

bool RequirementsRegistry::getRequirement(int building_id, int level, BuildRequirement* requirement) {
	for (size_t i = 0; i < _items.size(); ++i) {
		if (_items[i].building_type == building_id && _items[i].level == level) {
			requirement->building_type = building_id;
			requirement->level = level;
			requirement->required_building = _items[i].required_building;
			requirement->required_count = _items[i].required_count;
			requirement->required_level = _items[i].required_level;
			return true;
		}
	}
	return false;
}