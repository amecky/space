#pragma once
#include <vector>
#include "Common.h"
#include "Tiles.h"
#include "queues\WorkQueue.h"
#include "work\Work.h"

struct WorldContext;
struct AreaDefinition;
class TextLine;

struct MyIsland {

	int id;
	Resources resources;
	Resources maxResources;
	Tiles* tiles;
	WorkQueue queue;
	Collectables collectables;

};


// ------------------------------------------------------
// Island
// ------------------------------------------------------
class Island {

public:
	Island(WorldContext* context,int id,int size_x,int size_y);
	~Island();
	
	void add(const char* s,int x,int y);
	void add(int x,int y,const Sign& s);
	void addResource(const Sign& sign,int value);
	void addResource(const char* sign,int value);
	bool upgrade(int x,int y);
	bool move(int oldX,int oldY,int x,int y);
	bool collect(int x, int y);
private:
	void removeBuilding(int building_id,int x,int y);
	bool createWork(int price_type,int x,int y,int building_id, int level);
	void calculateMaxResources();
	bool isUsed(int x, int y);
	void addResources(const Resources& r);
	void subResources(const Resources& r);
	Resources _resources;
	Resources _maxResources;
	Tiles* _tiles;
	WorkQueue _queue;
	
	Collectables _collectables;
	WorldContext* _context;
	int _id;
};

namespace island {

	void load(WorldContext* context, MyIsland* island);

	void save(WorldContext* context, MyIsland* island);

	bool is_available(WorldContext* context, MyIsland* island, const Resources& costs);

	bool check_requirements(WorldContext* context, MyIsland* island, int building_id, int level);

	void add_resource(WorldContext* context, MyIsland* island, int resource_id, int amount);

	void add_resource(WorldContext* context, MyIsland* island, const Sign& sign, int value);

	void calculate_max_resources(WorldContext* context, MyIsland* island);
}