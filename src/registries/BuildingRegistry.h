#pragma once
#include "BaseRegistry.h"
#include "..\Common.h"
// ------------------------------------------------------
// building definition
// ------------------------------------------------------
struct BuildingDefinition {

	int id;
	Sign sign;
	bool permanent;
	bool regular;
	int max_count;
	bool destructable;
	char name[32];
	int size_x;
	int size_y;
};


// ------------------------------------------------------
// Building registry
// ------------------------------------------------------
class BuildingRegistry : public BaseRegistry<BuildingDefinition> {

public:
	BuildingRegistry() {}
	~BuildingRegistry() {}
	bool getDefinition(int id, BuildingDefinition* definition);
	bool getDefinition(const Sign& s, BuildingDefinition* definition);
	const char* getSign(int id) const;
	const char* getName(int id) const;
	const bool runs_permanent(int id) const;
	const bool isDestructable(int id) const;
	const int getIndex(const char* c) const;
	const int getIndex(const Sign& s) const;
	const char** get_field_names() const;
	int get_field_num() const;
	bool load_entry(const RegistryReader& reader,int index,BuildingDefinition* t);
	virtual bool validate_data();	
private:
	const int getIndex(int id) const;
};
