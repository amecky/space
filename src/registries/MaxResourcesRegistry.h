#pragma once
#include "PriceRegistry.h"

// ------------------------------------------------------
// max resource definition
// ------------------------------------------------------
struct MaxResourceDefinition {

	int building_id;
	int level;
	int resource_id;
	int amount;

};

class MaxResourcesRegistry : public BaseRegistry<MaxResourceDefinition> {

public:
	MaxResourcesRegistry(ResourceRegistry* res_reg,BuildingRegistry* bld_reg) 
		: _resource_registry(res_reg) , _building_registry(bld_reg) {}
	~MaxResourcesRegistry() {}
	const char** get_field_names() const;
	int get_field_num() const;
	bool load_entry(const RegistryReader& reader,int index,MaxResourceDefinition* t);	
	const bool contains(int building_id,int level) const;
	bool getRequirement(int building_id, int level, MaxResourceDefinition* requirement);
	void add(int building_id, int level, Resources* res);
private:
	ResourceRegistry* _resource_registry;
	BuildingRegistry* _building_registry;

};


