#pragma once
#include <vector>
#include "Common.h"
#include "Tiles.h"
#include "queues\WorkQueue.h"
#include "work\Work.h"

struct MyIsland {

	int id;
	Resources resources;
	Resources maxResources;
	Tiles* tiles;
	WorkQueue queue;
	Collectables collectables;

};

namespace island {

	void load(MyIsland* island);

	void save(MyIsland* island);

	bool is_available(MyIsland* island, const Resources& costs);

	bool check_requirements(MyIsland* island, int building_id, int level);

	void add_resource(MyIsland* island, int resource_id, int amount);

	void add_resource(MyIsland* island, const Sign& sign, int value);

	void add_resource(MyIsland* island,const char* sign,int value);

	void add_resources(MyIsland* island,const Resources& r);

	void sub_resources(MyIsland* island,const Resources& r);

	void calculate_max_resources(MyIsland* island);

	void add_building(MyIsland* island,int x,int y,const Sign& s);

	void add_building(MyIsland* island,int x,int y,const char* s);

	bool collect(MyIsland* island,int x, int y);

	bool move(MyIsland* island,int oldX,int oldY,int x,int y);

	void remove_building(MyIsland* island,int building_id,int x,int y);
}