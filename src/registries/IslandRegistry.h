#pragma once
#include "BaseRegistry.h"

// ------------------------------------------------------
// Island registry
// ------------------------------------------------------
struct AreaDefinition {

	int island;
	int area;
	int size_x;
	int size_y;
	int start_x;
	int start_y;
	bool locked;
	int resource_id;
	int amount;
	char file[256];

};

class IslandRegistry : public BaseRegistry<AreaDefinition> {

public:
	IslandRegistry() {}
	~IslandRegistry() {}
	const char** get_field_names() const;
	int get_field_num() const;
	bool load_entry(const RegistryReader& reader,int index,AreaDefinition* t);	
};


