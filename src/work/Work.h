#pragma once
#include <vector>
#include "..\Common.h"
#include "..\Tiles.h"
#include "..\queues\WorkQueue.h"
// ------------------------------------------------------
// collectable
// ------------------------------------------------------
struct Collectable {

	int price_type;
	int building_id;
	int level;
	int tile_x;
	int tile_y;
	bool remove;
};

struct AreaDefinition;
class TextLine;

typedef std::vector<Collectable> Collectables;

class Island;
// ------------------------------------------------------
// SimWork
// ------------------------------------------------------
class SimWork {

public:
	SimWork();
	virtual ~SimWork() {}
	virtual bool start(Island* island, const TextLine& line) = 0;
	virtual void finish(Island* island, const Event& e) = 0;
	virtual const int getWorkType() const = 0;
protected:	
	void removeBuilding(Island* island, int building_id, int x, int y);

};

