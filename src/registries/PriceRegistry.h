#pragma once
#include "..\Resources.h"
#include "RegistryReader.h"
#include <vector>
#include "..\Common.h"
#include <assert.h>
#include "BaseRegistry.h"
#include "BuildingRegistry.h"

const int PT_WORK      = 0;
const int PT_UPGRADE   = 1;
const int PT_DELETE    = 2;
const int PT_BUILD     = 3;
const int PT_PERMANENT = 4;
const int PT_REGULAR   = 5;

// Work Upgrade Delete Build Permanent Regular
const char PRICE_TYPES[] = { 'W', 'U', 'D', 'B', 'P', 'R' };

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

struct Tile;
// ------------------------------------------------------
// Price registry
// ------------------------------------------------------
class PriceRegistry : public BaseRegistry<RegistryEntry> {

public:
	PriceRegistry(ResourceRegistry* res_reg,BuildingRegistry* bld_reg);
	~PriceRegistry(void);
	bool get(int price_type, int stage, const Tile& tile, Resources* resources);
	bool get(int price_type, int stage, int building_type, int level, Resources* resources);
	int getIndex(int price_type, int stage, int building_type, int level);
	const RegistryEntry& getEntry(int idx) const {
		return _items[idx];
	}
	int getDuration(int price_type, int building_type, int level);
	const char** get_field_names() const;
	int get_field_num() const;
	bool load_entry(const RegistryReader& reader,int index,RegistryEntry* entry);	
private:
	ResourceRegistry* _resource_registry;
	BuildingRegistry* _building_registry;
};

