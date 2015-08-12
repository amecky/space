#pragma once
#include "..\Resources.h"
#include "RegistryReader.h"
#include <vector>
#include "..\Common.h"
#include <assert.h>
#include "BaseRegistry.h"
#include "BuildingRegistry.h"

struct PriceDefinition {
	
	WorkType type;
	char sign;
	const char* name;

	PriceDefinition() {}
	PriceDefinition(WorkType _type,char _sign,const char* _name) : type(_type) , sign(_sign) , name(_name) {}

};

// ------------------------------------------------------
// registry entry
// ------------------------------------------------------
struct RegistryEntry {

	WorkType work_type;
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
	bool get(WorkType work_type, int stage, const Tile& tile, Resources* resources);
	bool get(WorkType work_type, int stage, int building_type, int level, Resources* resources);
	int getIndex(WorkType work_type, int stage, int building_type, int level);
	const RegistryEntry& getEntry(int idx) const {
		return _items[idx];
	}
	int getDuration(WorkType work_type, int building_type, int level);
	const char** get_field_names() const;
	int get_field_num() const;
	bool load_entry(const RegistryReader& reader,int index,RegistryEntry* entry);	
	WorkType findBySign(char s);
	const char* translateWorkType(WorkType work_type);
private:	
	PriceDefinition _definitions[PT_ITEM_COUNT];
	ResourceRegistry* _resource_registry;
	BuildingRegistry* _building_registry;
};

