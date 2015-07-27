#pragma once
#include "PriceRegistry.h"
// ------------------------------------------------------
// build requirement
// ------------------------------------------------------
struct BuildRequirement {

	int building_type;
	int level;

	int required_building;
	int required_level;

	int required_count;
};

class RequirementsRegistry : public BaseRegistry<BuildRequirement> {

public:
	RequirementsRegistry(BuildingRegistry* bld_reg) : _building_registry(bld_reg) {}
	~RequirementsRegistry() {}
	const char** get_field_names() const;
	int get_field_num() const;
	bool load_entry(const RegistryReader& reader,int index,BuildRequirement* t);	
	const bool contains(int building_id,int level) const;
	bool getRequirement(int building_id, int level, BuildRequirement* requirement);
private:
	BuildingRegistry* _building_registry;

};


