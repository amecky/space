#include "DeleteWork.h"
#include "..\registries\IslandRegistry.h"
#include "..\utils\files.h"
#include "..\World.h"
#include "..\utils\Serializer.h"
#include "..\utils\CSVFile.h"

// ------------------------------------------------------
// DeleteWork
// ------------------------------------------------------
bool DeleteWork::start(MyIsland* island, const TextLine& line) {
	int x = line.get_int(1);
	int y = line.get_int(2);
	LOGC("DeleteWork") << "start at: " << x << " " << y;
	Tiles* _tiles = island->tiles;
	int idx = x + y * _tiles->width;
	if (_tiles->is_empty(x, y)) {
		printf("Error: There is no building at %d %d\n", x, y);
		return false;
	}
	Tile& t = _tiles->get(x, y);
	if (!_context->building_definitions.isDestructable(t.building_id)) {
		printf("Error: You cannot remove this building\n");
		return false;
	}
	_tiles->set_state(x, y, TS_ACTIVE);
	createWork(island,PT_DELETE, x, y, t.building_id, t.level);
	return true;
}

void DeleteWork::finish(MyIsland* island, const Event& e) {
	LOGC("DeleteWork") << "finish";
	Resources saved;
	island->tiles->clear_state(e.tile_x, e.tile_y, TS_ACTIVE);
	if (!_context->price_registry.get(e.work_type, 2, e.building_id, e.level, &saved)) {
		LOGC("DeleteWork") << "Delete - no resources to collect defined - removing right away";
		removeBuilding(island,e.building_id, e.tile_x, e.tile_y);
	}
}

const int DeleteWork::getWorkType() const {
	return PT_DELETE;
}
