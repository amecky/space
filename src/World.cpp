#include "World.h"
#include <stdio.h>
#include "utils\files.h"
#include <assert.h>
#include "utils\CSVFile.h"
#include <vector>
#include <string>
#include "utils\Serializer.h"

World::World() : _selected(-1) {
	_context.building_definitions.load("buildings.txt");
	_context.resource_registry.load("resource_definitions.txt");		
	char buffer[256];
	for ( size_t i = 0; i < _context.building_definitions.size(); ++i ) {
		const BuildingDefinition& def = _context.building_definitions.get(i);
		sprintf(buffer,"%s.txt",def.sign.c_str());
		_context.price_registry.load(buffer);
	}
	_context.requirements_registry.load("requirements.txt");
	_context.max_resources_registry.load("max_resources.txt");
	_context.task_registry.load("tasks.txt");
	_context.reward_registry.load("rewards.txt");
	_context.collect_mode = CM_IMMEDIATE;
	_context.time_multiplier = 1;
}

Island* World::createIsland(int width,int height) {
	int id = _islands.size();
	Island* i = new Island(&_context,id,width,height);
	_islands.push_back(i);
	return i;
}

Island* World::getIsland(int index) const {
	return _islands[index];
}

Island* World::getSelectedIsland() const {
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
	int id = _context.resource_registry.getIndex(sign);
	_context.global_resources.add(id, value);
}

// ------------------------------------------------------
// save
// ------------------------------------------------------
void World::save(DWORD recent_time) {
	Serializer writer;
	if (writer.open("world.bin", "data",BM_WRITE)) {
		int sz = _context.global_resources.total;
		LOGC("World") << "saving global resources: " << sz;
		writer.write(sz);
		for (int i = 0; i < sz; ++i) {
			writer.write(_context.global_resources._values[i]);
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
		_context.task_queue.save(writer);
	}	
}

// ------------------------------------------------------
// load 
// ------------------------------------------------------
void World::load() {
	LOGC("World") << "loading world";
	for ( size_t i = 0; i < _islands.size(); ++i ) {
		delete _islands[i];
	}
	_islands.clear();
	Serializer reader;
	if (reader.open("world.bin", "data", BM_READ)) {
        int num = 0;
		// load global resources
		reader.read(&num);
		for ( int i = 0; i < num; ++i ) {
			reader.read(&_context.global_resources._values[i]);
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
			Island* is = new Island(&_context,i,sx,sy);
			is->load(i);
			_islands.push_back(is);
		}
		LOGC("World") << "loading active tasks";
		_context.task_queue.load(reader);
    }
}

// ------------------------------------------------------
// show tasks
// ------------------------------------------------------
void World::show_tasks() {
	ActiveTasks tasks;
	Reward rewards[16];
	_context.task_queue.get_active_tasks(_selected,tasks);
	for (size_t i = 0; i < tasks.size(); ++i) {
		printf("Task: ");
		printf("%s  ", tasks[i].task->text);
		if (_context.reward_registry.contains(tasks[i].task->id)) {
			int cnt = _context.reward_registry.get(tasks[i].task->id, rewards, 16);
			if (cnt > 0) {
				printf("Rewards: ");
				for (int j = 0; j < cnt; ++j) {
					printf("%d %s ", rewards[j].amount,_context.resource_registry.getName(rewards[j].resource_id));
				}				
			}
		}
		printf("\n");
		printf(" => %d / %d\n",tasks[i].count,tasks[i].task->amount);		
	}
}

// ------------------------------------------------------
// show building definitions
// ------------------------------------------------------
void World::showBuildingDefinitions() {
	_context.building_definitions.show();
}

WorldContext* World::getContext() {
	return &_context;
}

void World::setCollectMode(CollectMode cm) {
	_context.collect_mode = cm;
}