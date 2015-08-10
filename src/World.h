#pragma once
#include <windows.h>
#include "Common.h"
#include "Resources.h"
#include "registries\PriceRegistry.h"
#include "registries\MaxResourcesRegistry.h"
#include "registries\ResourceRegistry.h"
#include "registries\IslandRegistry.h"
#include "registries\TaskRegistry.h"
#include "registries\RewardsRegistry.h"
#include "Tiles.h"
#include "utils\Messages.h"
#include <vector>
#include <map>
#include "queues\WorkQueue.h"
#include "queues\TaskQueue.h"
#include "Island.h"

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
	Messages messages;
	WorldContext() 
		: price_registry(&resource_registry,&building_definitions) 
		, task_registry(&building_definitions)
		, requirements_registry(&building_definitions) 
		, max_resources_registry(&resource_registry,&building_definitions) 
		, reward_registry(&resource_registry) 
		, task_queue(&task_registry) {
	}
};

extern WorldContext* gContext;

class World {

typedef std::vector<MyIsland*> Islands;
typedef std::map<int, SimWork*> WorkMap;

public:
	World();
	~World() {
		for ( size_t i = 0; i < _islands.size(); ++i ) {
			delete[] _islands[i]->tiles;
			delete _islands[i];
		}
		delete gContext;
	}
	void setCollectMode(CollectMode cm);
	MyIsland* createIsland(int width,int height);
	MyIsland* getIsland(int index) const;
	MyIsland* getSelectedIsland() const;
	void selectIsland(int index);
	void tick(int timeUnits);
	void addResource(const Sign& sign, int value);
	void save(DWORD recent_time);
	void load();
	void execute(int work_type, const TextLine& line);
private:
	void tick(MyIsland* island, int timeUnits);
	int _selected;
	Islands _islands;
	WorkMap _work_map;
};
