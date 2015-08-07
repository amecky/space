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
	int building_id = gContext->building_definitions.getIndex(s);
	LOGC("BuildWork") << "start at: " << x << " " << y;
	Tiles* _tiles = island->tiles;
	int idx = x + y * _tiles->width;
	if (!_tiles->is_empty(x, y)) {
		gContext->messages.report_error("There is already a building at %d %d", x, y);
		return false;
	}
	BuildingDefinition def;
	gContext->building_definitions.getDefinition(building_id, &def);
	if (!_tiles->has_space(x, y, def.size_x, def.size_y)) {
		gContext->messages.report_error("There is not enough space to place the building at %d %d", x, y);
		return false;
	}
	// FIXME:
	//if (isUsed(x, y)) {
		//printf("Error: There is already a building under construction\n");
		//return false;
	//}
	if (gContext->price_registry.getDuration(PT_BUILD, building_id, 1) == -1) {
		gContext->messages.report_error("No duration defined in registry");
		return false;
	}
	int index = x + y * _tiles->width;
	if (island::check_requirements(island,building_id, 1)) {
		createWork(island,PT_BUILD,x,y,building_id,1);
		return true;
	}
	return false;
}

void BuildWork::finish(MyIsland* island, const Event& e) {
	LOGC("BuildWork") << "finish";
	BuildingDefinition def;
	gContext->building_definitions.getDefinition(e.building_id, &def);
	if (island->tiles->set(e.building_id, 1, e.tile_x, e.tile_y, def.size_x, def.size_y)) {
		island->queue.remove(PT_REGULAR, e.tile_x, e.tile_y);
		island::calculate_max_resources(island);
		Resources tmp;
		if (gContext->price_registry.get(PT_REGULAR, 0, def.id, 1, &tmp)) {
			createWork(island,PT_REGULAR, e.tile_x, e.tile_y, def.id, 1);
		}
	}
}

const int BuildWork::getWorkType() const {
	return PT_BUILD;
}
