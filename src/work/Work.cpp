#include "Work.h"
#include "..\registries\IslandRegistry.h"
#include "..\utils\files.h"
#include "..\World.h"
#include "..\utils\Serializer.h"
#include "..\utils\CSVFile.h"

SimWork::SimWork(WorldContext* context) : _context(context)  {}

bool SimWork::createWork(MyIsland* island,int price_type, int x, int y, int building_id, int level) {
	
	Resources costs;
	if (_context->price_registry.get(price_type, 0, building_id, level, &costs)) {
		res::show_resources(_context->resource_registry, costs, false);
		if (island::is_available(_context,island,costs)) {
			// descrease resoruces
				//FIXME:
			//subResources(costs);
			// create work
			island->queue.createWork(price_type, x, y, building_id, level, _context->price_registry.getDuration(price_type, building_id, level));
			return true;
		}
	}
	else {
		printf("Error: Unable to determine price - no registry entry\n");
	}
	
	return false;
}

// ------------------------------------------------------
// tick Island
// ------------------------------------------------------
void SimWork::removeBuilding(MyIsland* island,int building_id, int x, int y) {
	LOGC("Island") << "removing build at " << x << " " << y;
	Tile& t = island->tiles->get(x, y);
	BuildingDefinition def;
	_context->building_definitions.getDefinition(building_id, &def);
	island->tiles->remove(x, y, def.size_x, def.size_y);
}

// ------------------------------------------------------
// StartWork
// ------------------------------------------------------
bool StartWork::start(MyIsland* island, const TextLine& line) {
	int x = line.get_int(1);
	int y = line.get_int(2);
	int level = line.get_int(3);
	Tiles* _tiles = island->tiles;
	int idx = x + y * _tiles->width;
	if (_tiles->getBuildingID(idx) == -1) {
		printf("Error: There is no building at %d %d\n", x, y);
		return false;
	}
	if (_tiles->isActive(idx)) {
		printf("Error: The building is already active\n");
		return false;
	}
	if (level > _tiles->getLevel(idx)) {
		printf("Error: The selected level %d is not supported yet - You need to upgrade\n", level);
		return false;
	}
	if (_context->collect_mode == CM_MANUAL && _tiles->has_state(x, y, TS_COLLECTABLE)) {
		printf("ERROR: You need to collect the resources before you can start work again\n");
		return false;
	}
	Resources tmp;
	if (!_context->price_registry.get(PT_WORK, 0, _tiles->getBuildingID(idx), level, &tmp)) {
		printf("Error: The selected level %d is not supported yet - You need to upgrade\n", level);
		return false;
	}
	_tiles->set_state(x, y, TS_ACTIVE);
	return createWork(island,PT_WORK, x, y, _tiles->getBuildingID(idx), level);
}

void StartWork::finish(MyIsland* island, const Event& e) {

}

const int StartWork::getWorkType() const {
	return PT_WORK;
}