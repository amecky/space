#include "UpgradeWork.h"
#include "..\registries\IslandRegistry.h"
#include "..\utils\files.h"
#include "..\World.h"
#include "..\utils\Serializer.h"
#include "..\utils\CSVFile.h"

// ------------------------------------------------------
// StartWork
// ------------------------------------------------------
bool UpgradeWork::start(Island* island, const TextLine& line) {
	int x = line.get_int(1);
	int y = line.get_int(2);
	Tiles* tiles = island->getTiles();
	int idx = x + y * tiles->width;
	if ( tiles->getBuildingID(idx) == -1 ) {
		gContext->messages.report_error("There is no building at %d %d",x,y);
		return false;
	}
	// FIXME: check if there is only a regular work item
	if ( tiles->isActive(idx)) {
		gContext->messages.report_error("The building is active - upgrade is not available");
		return false;
	}
	if (island->checkRequirements(tiles->getBuildingID(idx), tiles->getLevel(idx) + 1)) {
		tiles->set_state(x,y,TS_ACTIVE);
		island->createWork(PT_UPGRADE,x,y,tiles->getBuildingID(idx),tiles->getLevel(idx)+1);
		return true;
	}
	return false;
}

void UpgradeWork::finish(Island* island, const Event& e) {
	Tiles* tiles = island->getTiles();
	tiles->clear_state(e.tile_x,e.tile_y,TS_ACTIVE);
	Tile& t = tiles->get(e.tile_x, e.tile_y);
	++t.level;
	// remove old regular work
	island->removeWork(PT_REGULAR,e.tile_x, e.tile_y);
	// create new
	island->calculateMaxResources();
	Resources tmp;
	if (gContext->price_registry.get(PT_REGULAR, 0, t.building_id, t.level, &tmp)) {
		island->createWork(PT_REGULAR, e.tile_x, e.tile_y, t.building_id, t.level);
	}
}

const int UpgradeWork::getWorkType() const {
	return PT_UPGRADE;
}