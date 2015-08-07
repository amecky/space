#include "StartWork.h"
#include "..\registries\IslandRegistry.h"
#include "..\utils\files.h"
#include "..\World.h"
#include "..\utils\Serializer.h"
#include "..\utils\CSVFile.h"

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
		gContext->messages.report_error("There is no building at %d %d", x, y);
		return false;
	}
	if (_tiles->isActive(idx)) {
		gContext->messages.report_error("The building is already active");
		return false;
	}
	if (level > _tiles->getLevel(idx)) {
		gContext->messages.report_error("The selected level %d is not supported yet - You need to upgrade", level);
		return false;
	}
	if (gContext->collect_mode == CM_MANUAL && _tiles->has_state(x, y, TS_COLLECTABLE)) {
		gContext->messages.report_error("You need to collect the resources before you can start work again");
		return false;
	}
	Resources tmp;
	if (!gContext->price_registry.get(PT_WORK, 0, _tiles->getBuildingID(idx), level, &tmp)) {
		gContext->messages.report_error("The selected level %d is not supported yet - You need to upgrade", level);
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