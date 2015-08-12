#pragma once
#include <vector>
#include "Common.h"
#include "Tiles.h"
#include "queues\WorkQueue.h"
#include "work\Work.h"

class Island {

public:
	Island(int id,int width,int height);
	~Island();
	void load();
	void save();
	bool isAvailable(const Resources& costs);
	bool checkRequirements(int building_id, int level);
	void addResource(int resource_id, int amount);
	void addResource(const Sign& sign, int value);
	void addResource(const char* sign,int value);
	void addResources(const Resources& r);
	void subResources(const Resources& r);
	void calculateMaxResources();
	void addBuilding(int x,int y,const Sign& s);
	void addBuilding(int x,int y,const char* s);
	bool collect(int x, int y);
	bool move(int oldX,int oldY,int x,int y);
	void removeBuilding(int building_id,int x,int y);
	void tick(int timeUnits);
	bool createWork(WorkType work_type, int x, int y, int building_id, int level);
	Tiles* getTiles();
	void removeWork(WorkType work_type, int x,int y);
	const int getID() const;
	void printStatus() const;
private:
	int _id;
	int _width;
	int _height;
	Resources _resources;
	Resources _maxResources;
	Tiles* _tiles;
	WorkQueue _queue;
	Collectables _collectables;
};
