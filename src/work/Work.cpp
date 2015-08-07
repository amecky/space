#include "Work.h"
#include "..\registries\IslandRegistry.h"
#include "..\utils\files.h"
#include "..\World.h"
#include "..\utils\Serializer.h"
#include "..\utils\CSVFile.h"

SimWork::SimWork()  {}

bool SimWork::createWork(MyIsland* island,int price_type, int x, int y, int building_id, int level) {
	
	Resources costs;
	if (gContext->price_registry.get(price_type, 0, building_id, level, &costs)) {
		res::show_resources(gContext->resource_registry, costs, false);
		if (island::is_available(island,costs)) {
			// decrease resoruces
			island::sub_resources(island,costs);
			// create work
			island->queue.createWork(price_type, x, y, building_id, level, gContext->price_registry.getDuration(price_type, building_id, level));
			return true;
		}
	}
	else {
		gContext->messages.report_error("Unable to determine price - no registry entry");
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
	gContext->building_definitions.getDefinition(building_id, &def);
	island->tiles->remove(x, y, def.size_x, def.size_y);
}

