#include "Island.h"
#include "registries\IslandRegistry.h"
#include "utils\files.h"
#include "World.h"
#include "utils\Serializer.h"

Island::Island(int id,int width,int height) : _id(id) , _width(width) , _height(height) {
	_tiles = new Tiles(_width, _height);
	_tiles->clear();
}

Island::~Island() {
	delete[] _tiles;
}

// ------------------------------------------------------
// save Island
// ------------------------------------------------------
void Island::save() {
	char buffer[256];
	sprintf(buffer, "i_%d.bin", _id);
	Serializer writer;
	if (writer.open(buffer, "data", BM_WRITE)) {
		// save resources
		int sz = gContext->resource_registry.size();
		writer.write(sz);
		LOGC("Island") << "saving resources: " << sz;
		for (int i = 0; i < sz; ++i) {
			writer.write(_resources._values[i]);
		}
		res::log_resources(gContext->resource_registry, _resources, false);
		LOGC("Island") << "saving tiles: " << _tiles->width << " " << _tiles->height;
		// save tiles
		for (int y = 0; y < _tiles->height; ++y) {
			for (int x = 0; x < _tiles->width; ++x) {
				writer.write(&_tiles->get(x, y), sizeof(Tile));
			}
		}
		_queue.save(writer);
	}
}

// ------------------------------------------------------
// load Island
// ------------------------------------------------------
void Island::load() {
	char buffer[256];
	sprintf(buffer, "i_%d.bin", _id);
	Serializer reader;
	LOGC("Island") << "reading island - file: " << buffer;
	//_queue.clear();
	int max = _tiles->total;
	_tiles->clear();
	if (reader.open(buffer, "data", BM_READ)) {
		int num = 0;
		// load resources
		reader.read(&num);
		LOGC("Island") << "resources: " << num;
		for (int i = 0; i < num; ++i) {
			reader.read(&_resources._values[i]);
		}
		res::log_resources(gContext->resource_registry, _resources, false);
		LOGC("Island") << "loading tiles: " << _tiles->width << " " << _tiles->height;
		// load buildings
		for (int y = 0; y < _tiles->height; ++y) {
			for (int x = 0; x < _tiles->width; ++x) {
				Tile t;
				reader.read(&t, sizeof(Tile));
				_tiles->set(x, y, t);
			}
		}
		_queue.load(reader);			
		calculateMaxResources();
	}
	else {
		LOGEC("Island") << "No file '" << buffer << "' found";
	}

}

// ------------------------------------------------------
// is_available - check if the required resources are available
// ------------------------------------------------------
bool Island::isAvailable(const Resources& costs) {
	bool ret = true;
	for (int i = 0; i < _resources.total; ++i) {
		int current = costs._values[i];
		int available = _resources._values[i];
		int global_available = gContext->global_resources.get(i);
		if (current > 0) {
			if (available < current && global_available < current) {
				gContext->messages.report_error("Not enough resources %s - required: %d available: %d", gContext->resource_registry.getName(i), current, available);
				ret = false;
			}
		}
	}
	return ret;
}

// ------------------------------------------------------
// check requirements
// ------------------------------------------------------
bool Island::checkRequirements(int building_id, int level) {
	BuildRequirement requirement;
	if (gContext->requirements_registry.getRequirement(building_id, level, &requirement)) {
		int cnt = 0;
		for (int i = 0; i < _tiles->total; ++i) {
			if (_tiles->match(i, requirement)) {
				++cnt;
			}
		}
		if (cnt != requirement.required_count) {
			gContext->messages.report_error("You cannot build because of missing requirements");
			gContext->messages.report_error("You need %d of %s with level %d", requirement.required_count, gContext->building_definitions.getName(requirement.required_building), requirement.required_level);
			return false;
		}
	}
	return true;
}

// ------------------------------------------------------
// add resources
// ------------------------------------------------------
void Island::addResource(int resource_id, int amount) {
	if (gContext->resource_registry.isGlobal(resource_id)) {
		gContext->global_resources.add(resource_id, amount);
	}
	else {
		_resources.add(resource_id, amount);
	}
}

// ------------------------------------------------------
// add resource
// ------------------------------------------------------
void Island::addResource(const Sign& sign, int value) {
	int id = gContext->resource_registry.getIndex(sign);
	addResource(id, value);
}

// ------------------------------------------------------
// add resource
// ------------------------------------------------------
void Island::addResource(const char* sign,int value) {
	assert(strlen(sign) == 2);
	Sign s(sign[0],sign[1]);
	int id = gContext->resource_registry.getIndex(s);
	_resources.set(id,value);
}

// ------------------------------------------------------
// add resources
// ------------------------------------------------------
void Island::addResources(const Resources& r) {
	for (int i = 0; i < gContext->resource_registry.size(); ++i) {
		if (gContext->resource_registry.isGlobal(i)) {
			gContext->global_resources.add(i, r._values[i]);
		}
		else {
			_resources.add(i,r._values[i]);
		}
	}
}


// ------------------------------------------------------
// subtract resources
// ------------------------------------------------------
void Island::subResources(const Resources& r) {
	for (int i = 0; i < gContext->resource_registry.size(); ++i) {
		if (gContext->resource_registry.isGlobal(i)) {
			gContext->global_resources.sub(i, r._values[i]);
		}
		else {
			_resources.sub(i, r._values[i]);
		}
	}
}


// ------------------------------------------------------
// calculate max resources
// ------------------------------------------------------
void Island::calculateMaxResources() {
	for (int i = 0; i < _maxResources.total; ++i) {
		_maxResources.set(i, 0);
	}
	for (int i = 0; i < _tiles->total; ++i) {
		if (_tiles->getBuildingID(i) != -1) {
			gContext->max_resources_registry.add(_tiles->getBuildingID(i), _tiles->getLevel(i), &_maxResources);
		}
	}
}

// ------------------------------------------------------
// add building
// ------------------------------------------------------
void Island::addBuilding(int x,int y,const Sign& s) {
	BuildingDefinition def;
	gContext->building_definitions.getDefinition(s,&def);
	if (_tiles->set(def.id,1,x,y,def.size_x,def.size_y)) {
		calculateMaxResources();
		Resources tmp;
		if (gContext->price_registry.get(PT_REGULAR, 0, def.id, 1, &tmp)) {		
			_queue.createWork(PT_REGULAR,x,y,def.id,1,gContext->price_registry.getDuration(PT_REGULAR, def.id, 1));		
		}
	}
	else {
		LOGEC("Island") << "Cannot set " << def.name << " at " << x << " " << y;
	}
}

// ------------------------------------------------------
// add building
// ------------------------------------------------------
void Island::addBuilding(int x,int y,const char* s) {
	assert(strlen(s) == 2);
	BuildingDefinition def;
	Sign sign(s[0],s[1]);
	gContext->building_definitions.getDefinition(sign,&def);
	if (_tiles->set(def.id,1,x,y,def.size_x,def.size_y)) {
		calculateMaxResources();
		Resources tmp;
		if (gContext->price_registry.get(PT_REGULAR, 0, def.id, 1, &tmp)) {		
			_queue.createWork(PT_REGULAR,x,y,def.id,1,gContext->price_registry.getDuration(PT_REGULAR, def.id, 1));		
		}
	}
}

// ------------------------------------------------------
// collect resources
// ------------------------------------------------------
bool Island::collect(int x, int y) {
	LOGC("Island") << "collecting at " << x << " " << y;
	int idx = x + y * _tiles->width;
	if (_tiles->getBuildingID(idx) == -1) {
		gContext->messages.report_error("There is no building at %d %d", x, y);
		return false;
	}
	if ( !_tiles->has_state(x,y,TS_COLLECTABLE)) {
		gContext->messages.report_error("There is nothing to collect at %d %d", x, y);
		return false;
	}
	bool found = false;
	Collectables::iterator it = _collectables.begin();
	Resources saved;
	while ( it != _collectables.end() ) {
		if ( it->tile_x == x && it->tile_y == y ) {
			if (gContext->price_registry.get(it->price_type, 2, _tiles->get(it->tile_x,it->tile_y), &saved)) {
				//_resources.add(saved);
				addResources(saved);
				res::show_resources(gContext->resource_registry,saved,false);
			}
			if (gContext->price_registry.get(PT_REGULAR, 0,  _tiles->get(it->tile_x,it->tile_y), &saved)) {
				_queue.createWork(PT_REGULAR,x,y,it->building_id,it->level,gContext->price_registry.getDuration(PT_REGULAR,it->building_id,it->level));
			}
			_tiles->clear_state(x,y,TS_COLLECTABLE);
			if ( it->remove ) {
				BuildingDefinition def;
				gContext->building_definitions.getDefinition(it->building_id,&def);
				_tiles->remove(it->tile_x,it->tile_y,def.size_x,def.size_y);
			}
			it = _collectables.erase(it);
			found = true;
		}
		else {
			++it;
		}
	}
	if ( !found) {
		gContext->messages.report_error("There is nothing to collect at %d %d",x,y);
	}
	return found;	
}

// ------------------------------------------------------
// move
// ------------------------------------------------------
bool Island::move(int oldX,int oldY,int x,int y) {
	int oldIndex = oldX + oldY * _tiles->width;
	int index = x + y * _tiles->width;
	if ( _tiles->is_empty(oldX,oldY) ) {
		gContext->messages.report_error("There is no building at %d %d",x,y);
		return false;
	}
	if ( _tiles->isActive(oldIndex)) {
		gContext->messages.report_error("The building is active - you cannot move an active building");
		return false;
	}
	if ( !_tiles->is_empty(x,y)) {
		gContext->messages.report_error("There is already a building at %d %d",x,y);
		return false;
	}
	_tiles->move(oldIndex,index);
	return true;
}

// ------------------------------------------------------
// remove building
// ------------------------------------------------------
void Island::removeBuilding(int building_id,int x,int y) {
	LOGC("Island") << "removing build at " << x << " " << y;
	Tile& t = _tiles->get(x,y);
	BuildingDefinition def;
	gContext->building_definitions.getDefinition(building_id,&def);
	_tiles->remove(x,y,def.size_x,def.size_y);
}

// ------------------------------------------------------
// tick
// ------------------------------------------------------
void Island::tick(int timeUnits) {
	_queue.tick(timeUnits);
	if (_queue.hasEvents()) {
		Resources saved;
		for (int i = 0; i < _queue.event_size(); ++i) {
			const Event& e = _queue.getEvent(i);
			LOGC("Simulation") << "event: " << i << " type: " << e.work_type << " at: " << e.tile_x << " " << e.tile_y;
			// process type = finish immediately
			if (gContext->price_registry.get(e.work_type, 1, e.building_id, e.level, &saved)) {
				//island::add_resources(island,saved);//_resources.add(saved);
				LOGC("Simulation") << "There are resources to get immediately";
				res::log_resources(gContext->resource_registry, saved, false);
				_tiles->clear_state(e.tile_x, e.tile_y, TS_ACTIVE);
				addResources(saved);
			}
			// check if we have a collectable 
			if (gContext->price_registry.get(e.work_type, 2, e.building_id, e.level, &saved)) {
				// simply build collectable
				_tiles->set_state(e.tile_x, e.tile_y, TS_COLLECTABLE);
				_tiles->clear_state(e.tile_x, e.tile_y, TS_ACTIVE);
				LOGC("Simulation") << "There are resources to collect";
				res::log_resources(gContext->resource_registry, saved, false);
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
				_collectables.push_back(c);
			}
			if (gContext->work_map.find(e.work_type) != gContext->work_map.end()) {
				SimWork* work = gContext->work_map[e.work_type];
				work->finish(this, e);
			}			
			int ids[16];
			Reward rewards[16];
			int count = gContext->task_queue.handle_event(_id, e, ids, 16);
			if (count > 0) {
				LOG << "finished tasks: " << count;
				for (int c = 0; c < count; ++c) {
					LOG << c << " : " << ids[c];
					printf("Congratulations - You haved successfully finished a task\n");
					if (gContext->reward_registry.contains(ids[c])) {
						int cnt = gContext->reward_registry.get(ids[c], rewards, 16);
						LOG << "rewards: " << cnt;
						printf("Your rewards:\n");
						for (int r = 0; r < cnt; ++r) {
							Reward rew = rewards[r];
							printf(" %d %s\n", rew.amount, gContext->resource_registry.getName(rew.resource_id));
							addResource(rew.resource_id, rew.amount);
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
	if (gContext->collect_mode == CM_IMMEDIATE) {
		if (_collectables.size() > 0) {
			LOGC("Island") << "immediately collecting: " << _collectables.size();
		}
		for (size_t i = 0; i < _collectables.size(); ++i) {
			const Collectable& c = _collectables[i];
			_tiles->clear_state(c.tile_x, c.tile_y, TS_COLLECTABLE);
			Resources saved;
			if (gContext->price_registry.get(c.price_type, 2, _tiles->get(c.tile_x, c.tile_y), &saved)) {
				//_resources.add(saved);
				addResources(saved);
			}
			if (c.remove) {
				removeBuilding(c.building_id, c.tile_x, c.tile_y);
			}
		}
		_collectables.clear();
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
	for (int i = 0; i < gContext->resource_registry.size(); ++i) {
		if (_resources.get(i) > _maxResources.get(i) && _maxResources.get(i) != -1 && !gContext->resource_registry.isGlobal(i)) {
			_resources.set(i, _maxResources.get(i));
		}
	}
}

const int Island::getID() const {
	return _id;
}

void Island::removeWork(int price_type, int x,int y) {
	_queue.remove(price_type,x,y);
}

bool Island::createWork(int price_type, int x, int y, int building_id, int level) {
	
	Resources costs;
	if (gContext->price_registry.get(price_type, 0, building_id, level, &costs)) {
		res::show_resources(gContext->resource_registry, costs, false);
		if (isAvailable(costs)) {
			// decrease resoruces
			subResources(costs);
			// create work
			_queue.createWork(price_type, x, y, building_id, level, gContext->price_registry.getDuration(price_type, building_id, level));
			return true;
		}
	}
	else {
		gContext->messages.report_error("Unable to determine price - no registry entry");
	}
	return false;
}

Tiles* Island::getTiles() {
	return _tiles;
}

// ------------------------------------------------------
// show status
// ------------------------------------------------------
void Island::printStatus() const {
	printf("Global Resources:\n");
	for (int i = 0; i < gContext->resource_registry.size(); ++i) {
		if (gContext->resource_registry.isGlobal(i)) {
			printf("%10s : %d\n", gContext->resource_registry.getName(i), gContext->global_resources.get(i));
		}
	}
	printf("Resources:\n");
	for (int i = 0; i < gContext->resource_registry.size(); ++i) {
		if (!gContext->resource_registry.isGlobal(i)) {
			printf("%10s : %d / %d\n", gContext->resource_registry.getName(i), _resources.get(i), _maxResources.get(i));
		}
	}
	_queue.show();
	if (gContext->collect_mode == CM_MANUAL) {
		printf("Collectable:\n");
		for (size_t i = 0; i < _collectables.size(); ++i) {
			printf("  %s at %d %d\n", gContext->building_definitions.getName(_collectables[i].building_id), _collectables[i].tile_x, _collectables[i].tile_y);
		}
	}
}
