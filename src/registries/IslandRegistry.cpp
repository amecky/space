#include "IslandRegistry.h"

// ------------------------------------------------------
// Island registry
// ------------------------------------------------------
const char* IR_FIELD_NAMES[] = {"island","area","size_x","size_y","start_x","start_y","costs","amount","locked","file"};

const char** IslandRegistry::get_field_names() const {		
	return IR_FIELD_NAMES;
}

int IslandRegistry::get_field_num() const {
	return 10;
}
// ------------------------------------------------------
// load
// ------------------------------------------------------
bool IslandRegistry::load_entry(const RegistryReader& reader,int index,AreaDefinition* def) {
	def->island = reader.get_int(index,"island");
	def->area = reader.get_int(index,"area");
	def->size_x = reader.get_int(index,"size_x");
	def->size_y = reader.get_int(index,"size_y");
	def->start_x = reader.get_int(index,"start_x");
	def->start_y = reader.get_int(index,"start_y");
	def->locked = reader.get_bool(index,"locked");
	reader.get_string(index,"file",def->file);
	return true;
}

