#include "BuildWork.h"
#include "..\registries\IslandRegistry.h"
#include "..\utils\files.h"
#include "..\World.h"
#include "..\utils\Serializer.h"
#include "..\utils\CSVFile.h"

// ------------------------------------------------------
// DeleteWork
// ------------------------------------------------------
bool BuildWork::start(MyIsland* island, const TextLine& line) {
	int x = line.get_int(1);
	int y = line.get_int(2);
	Sign s = line.get_sign(3);
	int building_id =_context->building_definitions.getIndex(s);
	LOGC("BuildWork") << "start at: " << x << " " << y;
	Tiles* _tiles = island->tiles;
	int idx = x + y * _tiles->width;
	if (!_tiles->is_empty(x, y)) {
		printf("Error: There is already a building at %d %d\n", x, y);
		return false;
	}
	BuildingDefinition def;
	_context->building_definitions.getDefinition(building_id, &def);
	if (!_tiles->has_space(x, y, def.size_x, def.size_y)) {
		printf("Error: There is not enough space to place the building at %d %d\n", x, y);
		return false;
	}
	// FIXME:
	//if (isUsed(x, y)) {
		//printf("Error: There is already a building under construction\n");
		//return false;
	//}
	if (_context->price_registry.getDuration(PT_BUILD, building_id, 1) == -1) {
		printf("Error: No duration defined in registry\n");
		return false;
	}
	int index = x + y * _tiles->width;
	if (island::check_requirements(_context,island,building_id, 1)) {
		createWork(island,PT_BUILD,x,y,building_id,1);
		return true;
	}
	return false;
}

void BuildWork::finish(MyIsland* island, const Event& e) {
	LOGC("BuildWork") << "finish";
	BuildingDefinition def;
	_context->building_definitions.getDefinition(e.building_id, &def);
	if (island->tiles->set(e.building_id, 1, e.tile_x, e.tile_y, def.size_x, def.size_y)) {
		island->queue.remove(PT_REGULAR, e.tile_x, e.tile_y);
		// FIXME:
		//calculateMaxResources();
		Resources tmp;
		if (_context->price_registry.get(PT_REGULAR, 0, def.id, 1, &tmp)) {
			createWork(island,PT_REGULAR, e.tile_x, e.tile_y, def.id, 1);
		}
	}
}

const int BuildWork::getWorkType() const {
	return PT_BUILD;
}
