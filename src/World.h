#pragma once
#include "Resources.h"
#include "registries\PriceRegistry.h"
#include "registries\MaxResourcesRegistry.h"
#include "registries\ResourceRegistry.h"
#include "registries\IslandRegistry.h"
#include "registries\TaskRegistry.h"
#include "registries\RewardsRegistry.h"
#include "Tiles.h"
#include <vector>
#include "queues\WorkQueue.h"
#include "queues\TaskQueue.h"

// ------------------------------------------------------
// collect mode
// ------------------------------------------------------
enum CollectMode {
	CM_IMMEDIATE,
	CM_MANUAL
};

// ------------------------------------------------------
// world context
// ------------------------------------------------------
struct WorldContext {

	Resources global_resources;
	PriceRegistry price_registry;
	BuildingRegistry building_definitions;
	ResourceRegistry resource_registry;
	TaskRegistry task_registry;
	RequirementsRegistry requirements_registry;
	MaxResourcesRegistry max_resources_registry;
	RewardRegistry reward_registry;
	CollectMode collect_mode;
	int time_multiplier;
	TaskQueue task_queue;

	WorldContext() 
		: price_registry(&resource_registry,&building_definitions) 
		, task_registry(&building_definitions)
		, requirements_registry(&building_definitions) 
		, max_resources_registry(&resource_registry,&building_definitions) 
		, reward_registry(&resource_registry) 
		, task_queue(&task_registry) {
	}
};

// ------------------------------------------------------
// collectable
// ------------------------------------------------------
struct Collectable {

	int price_type;
	int building_id;
	int level;
	int tile_x;
	int tile_y;
	
};

// ------------------------------------------------------
// Island
// ------------------------------------------------------
class Island {

typedef std::vector<Collectable> Collectables;

public:
	Island(WorldContext* context,int id,int size_x,int size_y);
	~Island();
	void setCollectMode(CollectMode cm);
	void tick(int timeUnits);
	void add(const char* s,int x,int y);
	void add(int x,int y,const Sign& s);
	void addResource(const Sign& sign,int value);
	void addResource(const char* sign,int value);
	void status() const;
	bool start(int x,int y,int level);
	bool upgrade(int x,int y);
	bool build(int x, int y, int building_id);
	void save(int index);
	void load(int index);
	void load_txt(int index);
	bool move(int oldX,int oldY,int x,int y);
	bool collect(int x, int y);
	bool remove(int x, int y);
	bool describe(int x, int y);
	void createArea(const AreaDefinition& definition);
	const Tiles* getTiles() const {
		return _tiles;
	}
private:
	void addResource(int resource_id,int amount);
	bool createWork(int price_type,int x,int y,int building_id, int level);
	bool checkRequirements(int building_id, int level);
	void calculateMaxResources();
	bool is_available(const Resources& costs);
	bool isUsed(int x, int y);
	void addResources(const Resources& r);
	void subResources(const Resources& r);
	CollectMode _collect_mode;
	Resources _resources;
	Resources _maxResources;
	Tiles* _tiles;
	WorkQueue _queue;
	
	Collectables _collectables;
	WorldContext* _context;
	int _id;
};

class World {

typedef std::vector<Island*> Islands;

public:
	World();
	~World() {
		for ( size_t i = 0; i < _islands.size(); ++i ) {
			delete _islands[i];
		}
	}
	Island* createIsland(int width,int height);
	Island* getIsland(int index) const;
	Island* getSelectedIsland() const;
	void selectIsland(int index);
	void tick(int timeUnits);
	void addResource(const Sign& sign, int value);
	void save();
	void load();
	void show_tasks();
	WorldContext* getContext();
	void showBuildingDefinitions();
private:
	int _selected;
	Islands _islands;
	WorldContext _context;
};
