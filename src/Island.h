#pragma once
#include <vector>
#include "Common.h"
#include "Tiles.h"
#include "queues\WorkQueue.h"
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

struct WorldContext;
struct AreaDefinition;
// ------------------------------------------------------
// Island
// ------------------------------------------------------
class Island {

typedef std::vector<Collectable> Collectables;

public:
	Island(WorldContext* context,int id,int size_x,int size_y);
	~Island();
	
	void tick(int timeUnits);
	void add(const char* s,int x,int y);
	void add(int x,int y,const Sign& s);
	void addResource(const Sign& sign,int value);
	void addResource(const char* sign,int value);
	void status() const;
	bool start(int x,int y,int level);
	bool upgrade(int x,int y);
	bool build(int x, int y, int building_id);
	void save();
	void load(int index);
	//void load_txt(int index);
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
	//CollectMode _collect_mode;
	Resources _resources;
	Resources _maxResources;
	Tiles* _tiles;
	WorkQueue _queue;
	
	Collectables _collectables;
	WorldContext* _context;
	int _id;
};
