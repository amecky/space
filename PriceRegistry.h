#pragma once
#include "Resources.h"
#include <vector>
#include "Common.h"

const int PT_WORK      = 0;
const int PT_UPGRADE   = 1;
const int PT_DELETE    = 2;
const int PT_BUILD     = 3;
const int PT_PERMANENT = 4;
const int PT_REGULAR   = 5;



// Work Upgrade Delete Build Permanent Regular
const char PRICE_TYPES[] = { 'W', 'U', 'D', 'B', 'P', 'R' };

// ------------------------------------------------------
// resource definition
// ------------------------------------------------------
struct ResourceDefinition {

	int id;
	Sign sign;
	char name[32];
	bool global;
};

// ------------------------------------------------------
// resource registry
// ------------------------------------------------------
class ResourceRegistry {

typedef std::vector<ResourceDefinition> ResourceDefinitions;

public:
	ResourceRegistry() {}
	~ResourceRegistry() {}
	void load();
	const int size() const {
		return _definitions.size();
	}
	const char* getName(int id) const;
	const int getIndex(const Sign& c) const;
	const bool isGlobal(int id) const;
private:
	const int getIndex(int id) const;
	ResourceDefinitions _definitions;
};

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
class BuildingRegistry {

typedef std::vector<BuildingDefinition> BuildingDefinitions;

public:
	BuildingRegistry() {}
	~BuildingRegistry() {}
	void load();
	bool getDefinition(int id, BuildingDefinition* definition);
	bool getDefinition(const Sign& s, BuildingDefinition* definition);
	const char* getSign(int id) const;
	const char* getName(int id) const;
	const bool runs_permanent(int id) const;
	void show();
	const bool isDestructable(int id) const;
	const int getIndex(const char* c) const;
	const int getIndex(const Sign& s) const;
private:
	const int getIndex(int id) const;
	BuildingDefinitions _definitions;
};

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

// ------------------------------------------------------
// max resource definition
// ------------------------------------------------------
struct MaxResourceDefinition {

	int building_id;
	int level;
	int resource_id;
	int amount;

};
// ------------------------------------------------------
// registry entry
// ------------------------------------------------------
struct RegistryEntry {

	int price_type;
	int building_type;
	int level;
	int duration;
	int stage; // 0 = start 1 = finish 2 = collectable
	int resource_id;
	int amount;
	
};

// ------------------------------------------------------
// Registry / databse
// ------------------------------------------------------
class PriceRegistry {

typedef std::vector<RegistryEntry> RegistryEntries;
typedef std::vector<BuildRequirement> BuildRequirements;
typedef std::vector<MaxResourceDefinition> MaxResourceDefinitions;

public:
	PriceRegistry(ResourceRegistry* res_reg,BuildingRegistry* bld_reg);
	~PriceRegistry(void);
	void load(const char* name);
	void load_requirements();
	void load_max_resources();
	bool get(int price_type, int stage, int building_type, int level, Resources* resources);
	int getIndex(int price_type, int stage, int building_type, int level);
	const RegistryEntry& getEntry(int idx) const {
		return _entries[idx];
	}
	int getDuration(int price_type, int building_type, int level);
	bool getRequirement(int building_id, int level, BuildRequirement* requirement);
	void add_max_resources(int building_id, int level, Resources* res);
private:
	RegistryEntries _entries;
	BuildRequirements _requirements;
	MaxResourceDefinitions _max_resources;
	ResourceRegistry* _resource_registry;
	BuildingRegistry* _building_registry;
};

