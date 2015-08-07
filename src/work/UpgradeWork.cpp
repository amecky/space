#include "UpgradeWork.h"
#include "..\registries\IslandRegistry.h"
#include "..\utils\files.h"
#include "..\World.h"
#include "..\utils\Serializer.h"
#include "..\utils\CSVFile.h"

// ------------------------------------------------------
// StartWork
// ------------------------------------------------------
bool UpgradeWork::start(MyIsland* island, const TextLine& line) {
	int x = line.get_int(1);
	int y = line.get_int(2);
	int idx = x + y * island->tiles->width;
	if ( island->tiles->getBuildingID(idx) == -1 ) {
		gContext->messages.report_error("There is no building at %d %d",x,y);
		return false;
	}
	// FIXME: check if there is only a regular work item
	if ( island->tiles->isActive(idx)) {
		gContext->messages.report_error("The building is active - upgrade is not available");
		return false;
	}
	if (island::check_requirements(island,island->tiles->getBuildingID(idx), island->tiles->getLevel(idx) + 1)) {
		island->tiles->set_state(x,y,TS_ACTIVE);
		createWork(island,PT_UPGRADE,x,y,island->tiles->getBuildingID(idx),island->tiles->getLevel(idx)+1);
		return true;
	}
	return false;
}

void UpgradeWork::finish(MyIsland* island, const Event& e) {
	island->tiles->clear_state(e.tile_x,e.tile_y,TS_ACTIVE);
	Tile& t = island->tiles->get(e.tile_x, e.tile_y);
	++t.level;
	// remove old regular work
	island->queue.remove(PT_REGULAR,e.tile_x, e.tile_y);
	// create new
	island::calculate_max_resources(island);
	Resources tmp;
	if (gContext->price_registry.get(PT_REGULAR, 0, t.building_id, t.level, &tmp)) {
		createWork(island,PT_REGULAR, e.tile_x, e.tile_y, t.building_id, t.level);
	}
}

const int UpgradeWork::getWorkType() const {
	return PT_UPGRADE;
}