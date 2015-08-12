#include "World.h"
#include <stdio.h>
#include "utils\files.h"
#include <assert.h>
#include "utils\CSVFile.h"
#include <vector>
#include <string>
#include "utils\Serializer.h"
#include "work\Work.h"
#include "work\DeleteWork.h"
#include "work\BuildWork.h"
#include "work\StartWork.h"
#include "work\UpgradeWork.h"

WorldContext* gContext;

void initialize_context() {
	gContext = new WorldContext;
	// first build context
	gContext->building_definitions.load("buildings.txt");
	gContext->resource_registry.load("resource_definitions.txt");		
	char buffer[256];
	for ( size_t i = 0; i < gContext->building_definitions.size(); ++i ) {
		const BuildingDefinition& def = gContext->building_definitions.get(i);
		sprintf(buffer,"%s.txt",def.sign.c_str());
		gContext->price_registry.load(buffer);
	}
	gContext->requirements_registry.load("requirements.txt");
	gContext->max_resources_registry.load("max_resources.txt");
	gContext->task_registry.load("tasks.txt");
	gContext->reward_registry.load("rewards.txt");
	gContext->collect_mode = CM_IMMEDIATE;
	gContext->time_multiplier = 1;
	gContext->work_map[PT_WORK] = new StartWork;
	gContext->work_map[PT_DELETE] = new DeleteWork;
	gContext->work_map[PT_BUILD] = new BuildWork;
	gContext->work_map[PT_UPGRADE] = new UpgradeWork;
}


World::World() {
	initialize_context();
	_selected = -1;
}

World::~World() {
	clearIslands();
}

void World::clearIslands() {
	for ( size_t i = 0; i < _islands.size(); ++i ) {			
		delete _islands[i];
	}
}

Island* World::createIsland(int width,int height) {
	int id = _islands.size();
	Island* i = new Island(id,width,height);				
	_islands.push_back(i);
	return i;
}

Island* World::getIsland(int index) {
	return _islands[index];
}

Island* World::getSelectedIsland() {
	return _islands[_selected];
}

void World::selectIsland(int index) {
	_selected = index;
}

// ------------------------------------------------------
// tick
// ------------------------------------------------------
void World::tick(int timeUnits) {	
	for (size_t i = 0; i < _islands.size(); ++i) {
		_islands[i]->tick(timeUnits);
	}
}

// ------------------------------------------------------
// add global resource
// ------------------------------------------------------
void World::addResource(const Sign& sign, int value) {
	int id = gContext->resource_registry.getIndex(sign);
	gContext->global_resources.add(id, value);
}

// ------------------------------------------------------
// save
// ------------------------------------------------------
void World::save(DWORD recent_time) {
	Serializer writer;
	if (writer.open("world.bin", "data",BM_WRITE)) {
		int sz = gContext->global_resources.total;
		LOGC("World") << "saving global resources: " << sz;
		writer.write(sz);
		for (int i = 0; i < sz; ++i) {
			writer.write(gContext->global_resources._values[i]);
		}
		int num = _islands.size();
		writer.write(num);
		// save every island
		for (size_t i = 0; i < _islands.size(); ++i) {
			const Tiles* tiles = _islands[i]->getTiles();
			writer.write(tiles->width);
			writer.write(tiles->height);
			_islands[i]->save();
		}
		LOGC("World") << "saving active tasks";
		gContext->task_queue.save(writer);
	}
}

// ------------------------------------------------------
// load 
// ------------------------------------------------------
void World::load() {
	LOGC("World") << "loading world";
	clearIslands();
	_islands.clear();
	Serializer reader;
	if (reader.open("world.bin", "data", BM_READ)) {
		int num = 0;
		// load global resources
		reader.read(&num);
		for ( int i = 0; i < num; ++i ) {
			reader.read(&gContext->global_resources._values[i]);
		}
		LOGC("World") << "number of resources loaded: " << num;
		// load islands
		reader.read(&num);
		LOGC("World") << "number of islands: " << num;
		for ( int i = 0; i < num; ++i ) {
			int sx = 0;
			int sy = 0;
			reader.read(&sx);
			reader.read(&sy);
			// FIXME:
			Island* is = createIsland(sx, sy);
			is->load();
			_islands.push_back(is);
		}
		LOGC("World") << "loading active tasks";
		gContext->task_queue.load(reader);
	}
}

void World::execute(int work_type, const TextLine& line) {
	if (gContext->work_map.find(work_type) != gContext->work_map.end()) {
		SimWork* work = gContext->work_map[work_type];
		work->start(getSelectedIsland(), line);
	}
}


