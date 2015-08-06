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

World::World() : _selected(-1) {
	// first build context
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
	// build work map
	_work_map[PT_WORK] = new StartWork(&_context);
	_work_map[PT_DELETE] = new DeleteWork(&_context);
	_work_map[PT_BUILD] = new BuildWork(&_context);
}

MyIsland* World::createIsland(int width,int height) {
	int id = _islands.size();
	MyIsland* i = new MyIsland;
	i->id = id;
	i->tiles = new Tiles(width, height);
	i->tiles->clear();
	_islands.push_back(i);
	return i;
}

MyIsland* World::getIsland(int index) const {
	return _islands[index];
}

MyIsland* World::getSelectedIsland() const {
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
		tick(_islands[i],timeUnits);
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
			const Tiles* tiles = _islands[i]->tiles;
			writer.write(tiles->width);
			writer.write(tiles->height);
			island::save(&_context, _islands[i]);
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
			MyIsland* is = createIsland(sx, sy);
			island::load(&_context, is);
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

void World::execute(int work_type, const TextLine& line) {
	if (_work_map.find(work_type) != _work_map.end()) {
		SimWork* work = _work_map[work_type];
		work->start(getSelectedIsland(), line);
	}
}

void World::tick(MyIsland* island,int timeUnits) {
	island->queue.tick(timeUnits);
	if (island->queue.hasEvents()) {
		Resources saved;
		for (int i = 0; i < island->queue.event_size(); ++i) {
			const Event& e = island->queue.getEvent(i);
			LOGC("Simulation") << "event: " << i << " type: " << e.work_type << " at: " << e.tile_x << " " << e.tile_y;
			// process type = finish immediately
			if (_context.price_registry.get(e.work_type, 1, e.building_id, e.level, &saved)) {
				//_resources.add(saved);
				LOGC("Simulation") << "There are resources to get immediately";
				res::log_resources(_context.resource_registry, saved, false);
				island->tiles->clear_state(e.tile_x, e.tile_y, TS_ACTIVE);
				// FIXME:
				//addResources(saved);
			}
			// check if we have a collectable 
			if (_context.price_registry.get(e.work_type, 2, e.building_id, e.level, &saved)) {
				// simply build collectable
				island->tiles->set_state(e.tile_x, e.tile_y, TS_COLLECTABLE);
				island->tiles->clear_state(e.tile_x, e.tile_y, TS_ACTIVE);
				LOGC("Simulation") << "There are resources to collect";
				res::log_resources(_context.resource_registry, saved, false);
				Collectable c;
				c.building_id = e.building_id;
				c.level = e.level;
				c.price_type = e.work_type;
				c.tile_x = e.tile_x;
				c.tile_y = e.tile_y;
				if (e.work_type == PT_DELETE) {
					c.remove = true;
				}
				else {
					c.remove = false;
				}
				island->collectables.push_back(c);
			}
			if (_work_map.find(e.work_type) != _work_map.end()) {
				SimWork* work = _work_map[e.work_type];
				work->finish(island, e);
			}
			/*
			else if (e.work_type == PT_UPGRADE) {
				_tiles->clear_state(e.tile_x, e.tile_y, TS_ACTIVE);
				Tile& t = _tiles->get(e.tile_x, e.tile_y);
				++t.level;
				// remove old regular work
				_queue.remove(PT_REGULAR, e.tile_x, e.tile_y);
				// create new
				calculateMaxResources();
				Resources tmp;
				if (_context->price_registry.get(PT_REGULAR, 0, t.building_id, t.level, &tmp)) {
					createWork(PT_REGULAR, e.tile_x, e.tile_y, t.building_id, t.level);
				}
			}
			else if (e.work_type == PT_BUILD) {
				BuildingDefinition def;
				_context->building_definitions.getDefinition(e.building_id, &def);
				if (_tiles->set(e.building_id, 1, e.tile_x, e.tile_y, def.size_x, def.size_y)) {
					_queue.remove(PT_REGULAR, e.tile_x, e.tile_y);
					calculateMaxResources();
					Resources tmp;
					if (_context->price_registry.get(PT_REGULAR, 0, def.id, 1, &tmp)) {
						createWork(PT_REGULAR, e.tile_x, e.tile_y, def.id, 1);
					}
				}
			}
			else if (e.work_type == PT_DELETE) {
				_tiles->clear_state(e.tile_x, e.tile_y, TS_ACTIVE);
				if (!_context->price_registry.get(e.work_type, 2, e.building_id, e.level, &saved)) {
					LOGC("Island") << "Delete - no resources to collect defined - removing right away";
					removeBuilding(e.building_id, e.tile_x, e.tile_y);
				}
			}
			*/
			int ids[16];
			Reward rewards[16];
			int count = _context.task_queue.handle_event(island->id, e, ids, 16);
			if (count > 0) {
				LOG << "finished tasks: " << count;
				for (int c = 0; c < count; ++c) {
					LOG << c << " : " << ids[c];
					printf("Congratulations - You haved successfully finished a task\n");
					if (_context.reward_registry.contains(ids[c])) {
						int cnt = _context.reward_registry.get(ids[c], rewards, 16);
						LOG << "rewards: " << cnt;
						printf("Your rewards:\n");
						for (int r = 0; r < cnt; ++r) {
							Reward rew = rewards[r];
							printf(" %d %s\n", rew.amount, _context.resource_registry.getName(rew.resource_id));
							// FIXME:
							//addResource(rew.resource_id, rew.amount);
						}
					}
					else {
						LOG << "No reward defined";
					}
				}
			}

		}

	}

	// immediate mode -> process all collectables directly
	if (_context.collect_mode == CM_IMMEDIATE) {
		if (island->collectables.size() > 0) {
			LOGC("Island") << "immediately collecting: " << island->collectables.size();
		}
		for (size_t i = 0; i < island->collectables.size(); ++i) {
			const Collectable& c = island->collectables[i];
			island->tiles->clear_state(c.tile_x, c.tile_y, TS_COLLECTABLE);
			Resources saved;
			if (_context.price_registry.get(c.price_type, 2, island->tiles->get(c.tile_x, c.tile_y), &saved)) {
				//_resources.add(saved);
				// FIXME:
				//addResources(saved);
			}
			if (c.remove) {
				// FIXME:
				//removeBuilding(c.building_id, c.tile_x, c.tile_y);
			}
		}
		island->collectables.clear();
	}

	/* No permanents at the moment
	Resources res;
	for ( int i = 0; i < GRID_SIZE * GRID_SIZE; ++i ) {
	if ( _context->building_definitions.runs_permanent(_tiles->getBuildingID(i))) {
	if ( _context->price_registry.get(PT_PERMANENT,true,_tiles->getBuildingID(i),_tiles->getLevel(i),&res) ) {
	_resources.add(res,timeUnits);
	}
	}
	}
	*/
	for (int i = 0; i < _context.resource_registry.size(); ++i) {
		if (island->resources.get(i) > island->maxResources.get(i) && island->maxResources.get(i) != -1 && !_context.resource_registry.isGlobal(i)) {
			island->resources.set(i, island->maxResources.get(i));
		}
	}
}