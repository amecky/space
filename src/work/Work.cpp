#include "Work.h"
#include "..\registries\IslandRegistry.h"
#include "..\utils\files.h"
#include "..\World.h"
#include "..\utils\Serializer.h"
#include "..\utils\CSVFile.h"

SimWork::SimWork()  {}

// ------------------------------------------------------
// tick Island
// ------------------------------------------------------
void SimWork::removeBuilding(Island* island,int building_id, int x, int y) {
	LOGC("Island") << "removing build at " << x << " " << y;
	island->removeBuilding(building_id,x,y);	
}

