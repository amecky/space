#include "TaskRegistry.h"

// ------------------------------------------------------
// Task registry
// ------------------------------------------------------
const char* TR_FIELD_NAMES[] = {"island","job_id","type","building","level","amount","text"};

const char** TaskRegistry::get_field_names() const {		
	return TR_FIELD_NAMES;
}

int TaskRegistry::get_field_num() const {
	return 7;
}
// ------------------------------------------------------
// load
// ------------------------------------------------------
bool TaskRegistry::load_entry(const RegistryReader& reader,int index,Task* t) {
	bool valid = true;
	t->island = reader.get_int(index,"island");
	t->job_id = reader.get_int(index,"job_id");
	char type = reader.get_char(index,"type");
	Sign bs = reader.get_sign(index,"building");
	t->building_id = _building_registry->getIndex(bs);
	if (t->building_id == -1) {
		printf("ERROR: invalid building type at %d\n", reader.get_line_nr(index));
		valid = false;
	}
	t->level = reader.get_int(index,"level");
	t->amount = reader.get_int(index,"amout");			
	reader.get_string(index,"text",t->text);
	return valid;
}
