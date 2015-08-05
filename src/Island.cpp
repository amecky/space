#include "Island.h"
#include "registries\IslandRegistry.h"
#include "utils\files.h"
#include "World.h"
#include "utils\Serializer.h"

Island::Island(WorldContext* context,int id,int size_x,int size_y) 
	: _context(context) , _id(id) {
	_tiles = new Tiles(size_x,size_y);
	_tiles->clear();	
	
}


Island::~Island() {
	delete _tiles;
}
// ------------------------------------------------------
// create area
// ------------------------------------------------------
void Island::createArea(const AreaDefinition& definition) {
	for ( int y = 0; y < definition.size_y; ++y) {
		for ( int x = 0; x < definition.size_x; ++x ) {
			int xp = definition.start_x + x;
			int yp = definition.start_y + y;
			if ( definition.locked) {
				_tiles->set_state(xp,yp,TS_LOCKED);
			}
		}
	}
	std::vector<std::string> content;
	if ( file::read(definition.file,"data",content)) {
		size_t len = content.size();
		for ( int i = (len - 1); i >= 0; --i ) {
			std::string line = content[i];
			for ( int j = 0; j < definition.size_x; ++j ) {
				int xp = definition.start_x + j;
				int yp = (definition.start_y + definition.size_y - 1) - i;
				char f = line[j *2];
				char s = line[j*2 +1];
				Sign sgn(f,s);
				int bid = _context->building_definitions.getIndex(sgn);
				if ( bid != -1 ) {
					add(xp,yp,sgn);
				}
				if ( f == 'x' && s == 'x' ) {
					_tiles->set_state(xp,yp,TS_UNDEFINED);
				}
			}
		}
	}
	calculateMaxResources();
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
			_context->max_resources_registry.add(_tiles->getBuildingID(i), _tiles->getLevel(i), &_maxResources);
		}
	}
}

// ------------------------------------------------------
// add building
// ------------------------------------------------------
void Island::add(int x,int y,const Sign& s) {
	BuildingDefinition def;
	_context->building_definitions.getDefinition(s,&def);
	if (_tiles->set(def.id,1,x,y,def.size_x,def.size_y)) {
		calculateMaxResources();
		Resources tmp;
		if (_context->price_registry.get(PT_REGULAR, 0, def.id, 1, &tmp)) {		
			_queue.createWork(PT_REGULAR,x,y,def.id,1,_context->price_registry.getDuration(PT_REGULAR, def.id, 1));		
		}
	}
	else {
		printf("ERROR: Cannot set %s at %d %d\n",def.name,x,y);
	}
}

// ------------------------------------------------------
// add building
// ------------------------------------------------------
void Island::add(const char* s,int x,int y) {
	assert(strlen(s) == 2);
	BuildingDefinition def;
	Sign sign(s[0],s[1]);
	_context->building_definitions.getDefinition(sign,&def);
	if (_tiles->set(def.id,1,x,y,def.size_x,def.size_y)) {
		calculateMaxResources();
		Resources tmp;
		if (_context->price_registry.get(PT_REGULAR, 0, def.id, 1, &tmp)) {		
			_queue.createWork(PT_REGULAR,x,y,def.id,1,_context->price_registry.getDuration(PT_REGULAR, def.id, 1));		
		}
	}
}

// ------------------------------------------------------
// add resource
// ------------------------------------------------------
void Island::addResource(const Sign& sign,int value) {
	int id = _context->resource_registry.getIndex(sign);
	_resources.set(id,value);
}

// ------------------------------------------------------
// add resource
// ------------------------------------------------------
void Island::addResource(const char* sign,int value) {
	assert(strlen(sign) == 2);
	Sign s(sign[0],sign[1]);
	int id = _context->resource_registry.getIndex(s);
	_resources.set(id,value);
}

// ------------------------------------------------------
// add resources
// ------------------------------------------------------
void Island::addResources(const Resources& r) {
	for (int i = 0; i < _context->resource_registry.size(); ++i) {
		if (_context->resource_registry.isGlobal(i)) {
			_context->global_resources.add(i, r._values[i]);
		}
		else {
			_resources.add(i,r._values[i]);
		}
	}
}

// ------------------------------------------------------
// add resources
// ------------------------------------------------------
void Island::addResource(int resource_id,int amount) {
	if (_context->resource_registry.isGlobal(resource_id)) {
		_context->global_resources.add(resource_id,amount);
	}
	else {
		_resources.add(resource_id,amount);
	}
}

// ------------------------------------------------------
// subtract resources
// ------------------------------------------------------
void Island::subResources(const Resources& r) {
	for (int i = 0; i < _context->resource_registry.size(); ++i) {
		if (_context->resource_registry.isGlobal(i)) {
			_context->global_resources.sub(i, r._values[i]);
		}
		else {
			_resources.sub(i, r._values[i]);
		}
	}
}

// ------------------------------------------------------
// tick Island
// ------------------------------------------------------
void Island::removeBuilding(int building_id,int x,int y) {
	LOGC("Island") << "removing build at " << x << " " << y;
	Tile& t = _tiles->get(x,y);
	BuildingDefinition def;
	_context->building_definitions.getDefinition(building_id,&def);
	_tiles->remove(x,y,def.size_x,def.size_y);
}

// ------------------------------------------------------
// tick Island
// ------------------------------------------------------
void Island::tick(int timeUnits) {
	_queue.tick(timeUnits);
	if ( _queue.hasEvents()) {
		Resources saved;
		for ( int i = 0; i < _queue.event_size();++i) {
			const Event& e = _queue.getEvent(i);
			LOGC("Simulation") << "event: " << i << " type: " << e.work_type << " at: " << e.tile_x << " " << e.tile_y;
			// process type = finish immediately
			if (_context->price_registry.get(e.work_type, 1, e.building_id, e.level, &saved)) {
				//_resources.add(saved);
				LOGC("Simulation") << "There are resources to get immediately";
				res::log_resources(_context->resource_registry,saved,false);
				_tiles->clear_state(e.tile_x,e.tile_y,TS_ACTIVE);
				addResources(saved);
			}
			// check if we have a collectable 
			if (_context->price_registry.get(e.work_type, 2, e.building_id, e.level, &saved)) {
				// simply build collectable
				_tiles->set_state(e.tile_x,e.tile_y,TS_COLLECTABLE);
				_tiles->clear_state(e.tile_x,e.tile_y,TS_ACTIVE);
				LOGC("Simulation") << "There are resources to collect";
				res::log_resources(_context->resource_registry,saved,false);
				Collectable c;
				c.building_id = e.building_id;
				c.level = e.level;
				c.price_type = e.work_type;
				c.tile_x = e.tile_x;
				c.tile_y = e.tile_y;
				if ( e.work_type == PT_DELETE) {
					c.remove = true;
				}
				else {
					c.remove = false;
				}
				_collectables.push_back(c);
			}
			else if (e.work_type == PT_UPGRADE) {
				_tiles->clear_state(e.tile_x,e.tile_y,TS_ACTIVE);
				Tile& t = _tiles->get(e.tile_x, e.tile_y);
				++t.level;
				// remove old regular work
				_queue.remove(PT_REGULAR,e.tile_x, e.tile_y);
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
			else if ( e.work_type == PT_DELETE) {
				_tiles->clear_state(e.tile_x,e.tile_y,TS_ACTIVE);
				if (!_context->price_registry.get(e.work_type, 2, e.building_id, e.level, &saved)) {
					LOGC("Island") << "Delete - no resources to collect defined - removing right away";
					removeBuilding(e.building_id,e.tile_x,e.tile_y);
				}
			}

			int ids[16];
			Reward rewards[16];
			int count = _context->task_queue.handle_event(_id, e,ids,16);
			if ( count > 0 ) {				
				LOG << "finished tasks: " << count;
				for ( int c = 0; c < count; ++c ) {
					LOG << c << " : " << ids[c];
					printf("Congratulations - You haved successfully finished a task\n");
					if ( _context->reward_registry.contains(ids[c])) {
						int cnt = _context->reward_registry.get(ids[c],rewards,16);
						LOG << "rewards: " << cnt;
						printf("Your rewards:\n");
						for ( int r = 0; r < cnt; ++r ) {
							Reward rew = rewards[r];
							printf(" %d %s\n",rew.amount,_context->resource_registry.getName(rew.resource_id));
							addResource(rew.resource_id,rew.amount);
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
	if ( _context->collect_mode == CM_IMMEDIATE ) {
		if ( _collectables.size() > 0 ) {
			LOGC("Island") << "immediately collecting: " << _collectables.size();
		}
		for ( size_t i = 0; i < _collectables.size(); ++i ) {
			const Collectable& c = _collectables[i];
			_tiles->clear_state(c.tile_x,c.tile_y,TS_COLLECTABLE);
			Resources saved;
			if (_context->price_registry.get(c.price_type, 2, _tiles->get(c.tile_x,c.tile_y), &saved)) {
				//_resources.add(saved);
				addResources(saved);
			}
			if ( c.remove ) {
				removeBuilding(c.building_id,c.tile_x,c.tile_y);
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
	for ( int i = 0; i < _context->resource_registry.size(); ++i ) {
		if ( _resources.get(i) > _maxResources.get(i) && _maxResources.get(i) != -1 && !_context->resource_registry.isGlobal(i)) {
			_resources.set(i,_maxResources.get(i));
		}
	}
}

// ------------------------------------------------------
// show status
// ------------------------------------------------------
void Island::status() const {	
	printf("Global Resources:\n");
	for (int i = 0; i < _context->resource_registry.size(); ++i) {
		if (_context->resource_registry.isGlobal(i)) {
			printf("%10s : %d\n", _context->resource_registry.getName(i), _context->global_resources.get(i));
		}
	}
	printf("Resources:\n");
	for ( int i = 0; i < _context->resource_registry.size(); ++i ) {
		if (!_context->resource_registry.isGlobal(i)) {
			printf("%10s : %d / %d\n", _context->resource_registry.getName(i), _resources.get(i), _maxResources.get(i));		
		}
	}	
	_queue.show();	
	if ( _context->collect_mode == CM_MANUAL) {
		printf("Collectable:\n");
		for ( size_t i = 0; i < _collectables.size(); ++i ) {
			printf("  %s at %d %d\n",_context->building_definitions.getName(_collectables[i].building_id),_collectables[i].tile_x,_collectables[i].tile_y);
		}
	}
}

// ------------------------------------------------------
// collect resources
// ------------------------------------------------------
bool Island::collect(int x, int y) {
	int idx = x + y * _tiles->width;
	if (_tiles->getBuildingID(idx) == -1) {
		printf("Error: There is no building at %d %d\n", x, y);
		return false;
	}
	if ( !_tiles->has_state(x,y,TS_COLLECTABLE)) {
		printf("Error: There is nothing to collect at %d %d\n", x, y);
		return false;
	}
	bool found = false;
	Collectables::iterator it = _collectables.begin();
	Resources saved;
	while ( it != _collectables.end() ) {
		if ( it->tile_x == x && it->tile_y == y ) {
			if (_context->price_registry.get(it->price_type, 2, _tiles->get(it->tile_x,it->tile_y), &saved)) {
				//_resources.add(saved);
				addResources(saved);
				res::show_resources(_context->resource_registry,saved,false);
			}
			if (_context->price_registry.get(PT_REGULAR, 0,  _tiles->get(it->tile_x,it->tile_y), &saved)) {
				_queue.createWork(PT_REGULAR,x,y,it->building_id,it->level,_context->price_registry.getDuration(PT_REGULAR,it->building_id,it->level));
			}
			_tiles->clear_state(x,y,TS_COLLECTABLE);
			if ( it->remove ) {
				BuildingDefinition def;
				_context->building_definitions.getDefinition(it->building_id,&def);
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
		printf("Error: There is nothing to collect at %d %d\n",x,y);
	}
	return found;	
}

// ------------------------------------------------------
// start creating new building
// ------------------------------------------------------
bool Island::build(int x,int y,int building_id) {
	int idx = x + y * _tiles->width;
	if ( !_tiles->is_empty(x,y)) {
		printf("Error: There is already a building at %d %d\n",x,y);
		return false;
	}
	BuildingDefinition def;
	_context->building_definitions.getDefinition(building_id,&def);
	if ( !_tiles->has_space(x,y,def.size_x,def.size_y)) {
		printf("Error: There is not enough space to place the building at %d %d\n",x,y);
		return false;
	}
	if (isUsed(x, y)) {
		printf("Error: There is already a building under construction\n");
		return false;
	}
	if ( _context->price_registry.getDuration(PT_BUILD,building_id,1) == -1 ) {
		printf("Error: No duration defined in registry\n");
		return false;
	}
	int index = x + y * _tiles->width;
	if (checkRequirements(building_id, 1)) {
		createWork(PT_BUILD,x,y,building_id,1);
		return true;
	}
	return false;
}

// ------------------------------------------------------
// start work at building
// ------------------------------------------------------
bool Island::start(int x,int y,int level) {
	int idx = x + y * _tiles->width;
	if ( _tiles->getBuildingID(idx) == -1 ) {
		printf("Error: There is no building at %d %d\n",x,y);
		return false;
	}	
	if ( _tiles->isActive(idx)) {
		printf("Error: The building is already active\n");
		return false;
	}
	if ( level > _tiles->getLevel(idx) ) {
		printf("Error: The selected level %d is not supported yet - You need to upgrade\n",level);
		return false;
	}
	if ( _context->collect_mode == CM_MANUAL && _tiles->has_state(x,y,TS_COLLECTABLE)) {
		printf("ERROR: You need to collect the resources before you can start work again\n");
		return false;
	}
	Resources tmp;
	if ( !_context->price_registry.get(PT_WORK,0,_tiles->getBuildingID(idx),level,&tmp)) {
		printf("Error: The selected level %d is not supported yet - You need to upgrade\n",level);
		return false;
	}
	_tiles->set_state(x,y,TS_ACTIVE);
	createWork(PT_WORK,x,y,_tiles->getBuildingID(idx),level);
	return true;
}

// ------------------------------------------------------
// upgrade building
// ------------------------------------------------------
bool Island::upgrade(int x,int y) {
	int idx = x + y * _tiles->width;
	if ( _tiles->getBuildingID(idx) == -1 ) {
		printf("Error: There is no building at %d %d\n",x,y);
		return false;
	}
	// FIXME: check if there is only a regular work item
	if ( _tiles->isActive(idx)) {
		printf("Error: The building is active - upgrade is not available\n");
		return false;
	}
	if (checkRequirements(_tiles->getBuildingID(idx), _tiles->getLevel(idx) + 1)) {
		_tiles->set_state(x,y,TS_ACTIVE);
		createWork(PT_UPGRADE,x,y,_tiles->getBuildingID(idx),_tiles->getLevel(idx)+1);
		return true;
	}
	return false;
}

// ------------------------------------------------------
// describe building
// ------------------------------------------------------
bool Island::describe(int x,int y) {
	int idx = x + y * _tiles->width;
	if ( _tiles->getBuildingID(idx) == -1 ) {
		printf("Error: There is no building at %d %d\n",x,y);
		return false;
	}
	const Tile& t = _tiles->get(x,y);
	printf("Building: %s\n",_context->building_definitions.getName(t.building_id));
	printf("   Level: %d\n",t.level);
	Resources tmp;
	if (_context->price_registry.get(PT_REGULAR, 2, t.building_id, t.level, &tmp)) {
		printf("Regular income:\n");
		printf("Duration: %d\n", _context->price_registry.getDuration(PT_REGULAR, t.building_id, t.level));
		res::show_resources(_context->resource_registry, tmp, false);
	}
	printf("Available work:\n");	
	for ( int i = 1; i <= t.level; ++i ) {				
		if ( _context->price_registry.get(PT_WORK,0,t.building_id,i,&tmp)) {
			printf(" Level %d\n", i);
			printf("Duration: %d\n", _context->price_registry.getDuration(PT_WORK, t.building_id, i));
			printf("Costs:\n");
			res::show_resources(_context->resource_registry,tmp,false);
		}		
		if (_context->price_registry.get(PT_WORK, 1, t.building_id, i, &tmp)) {
			printf("Income:\n");
			Resources collect;
			if (_context->price_registry.get(PT_WORK, 2, t.building_id, i, &collect)) {
				tmp.add(collect);

			}
			res::show_resources(_context->resource_registry, tmp, false);
		}
	}
	// FIXME: list start options
	return true;
}

// ------------------------------------------------------
// move
// ------------------------------------------------------
bool Island::move(int oldX,int oldY,int x,int y) {
	int oldIndex = oldX + oldY * _tiles->width;
	int index = x + y * _tiles->width;
	if ( _tiles->is_empty(oldX,oldY) ) {
		printf("Error: There is no building at %d %d\n",x,y);
		return false;
	}
	if ( _tiles->isActive(oldIndex)) {
		printf("Error: The building is active - you cannot move an active building\n");
		return false;
	}
	if ( !_tiles->is_empty(x,y)) {
		printf("Error: There is already a building at %d %d\n",x,y);
		return false;
	}
	_tiles->move(oldIndex,index);
	return true;
}

// ------------------------------------------------------
// remove building
// ------------------------------------------------------
bool Island::remove(int x,int y) {
	int idx = x + y * _tiles->width;
	if ( _tiles->is_empty(x,y)) {
		printf("Error: There is no building at %d %d\n",x,y);
		return false;
	}
	Tile& t = _tiles->get(x,y);
	if ( !_context->building_definitions.isDestructable(t.building_id)) {
		printf("Error: You cannot remove this building\n");
		return false;
	}
	_tiles->set_state(x,y,TS_ACTIVE);
	createWork(PT_DELETE,x,y,t.building_id,t.level);	
	return true;
}

// ------------------------------------------------------
// check requirements
// ------------------------------------------------------
bool Island::checkRequirements(int building_id, int level) {
	BuildRequirement requirement;
	if (_context->requirements_registry.getRequirement(building_id, level, &requirement)) {
		int cnt = 0;
		for (int i = 0; i < _tiles->total; ++i) {
			if ( _tiles->match(i,requirement) ) {
				++cnt;
			}
		}
		if (cnt != requirement.required_count) {
			printf("Error: You cannot build because of missing requirements\n");
			printf("You need %d of %s with level %d\n", requirement.required_count, _context->building_definitions.getName(requirement.required_building), requirement.required_level);
			return false;
		}
	}
	return true;
}

// ------------------------------------------------------
// create work item
// ------------------------------------------------------
bool Island::createWork(int price_type, int x, int y, int building_id, int level) {
	Resources costs;
	if (_context->price_registry.get(price_type, 0, building_id, level, &costs)) {
		res::show_resources(_context->resource_registry,costs,false);
		if (is_available(costs)) {
			// descrease resoruces
			subResources(costs);
			// create work
			_queue.createWork(price_type, x, y, building_id, level, _context->price_registry.getDuration(price_type, building_id, level));
			return true;
		}
	}
	else {
		printf("Error: Unable to determine price - no registry entry\n");
	}
	return false;
}

// ------------------------------------------------------
// save Island
// ------------------------------------------------------
void Island::save() {
	char buffer[256];
	sprintf(buffer, "i_%d.bin", _id);
	Serializer writer;
	if (writer.open(buffer, "data",BM_WRITE)) {
		// save resources
		int sz = _context->resource_registry.size();
		writer.write(sz);
		LOGC("Island") << "saving resources: " << sz;
		for (int i = 0; i < sz; ++i) {
			writer.write(_resources._values[i]);
		}
		res::log_resources(_context->resource_registry, _resources, false);
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
void Island::load(int index) {
	char buffer[256];
	sprintf(buffer,"i_%d.bin",index);
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
		for ( int i = 0; i < num; ++i ) {
			reader.read(&_resources._values[i]);
		}
		res::log_resources(_context->resource_registry,_resources,false);
		LOGC("Island") << "loading tiles: " << _tiles->width << " " << _tiles->height;
		// load buildings
		for (int y = 0; y < _tiles->height; ++y) {
			for (int x = 0; x < _tiles->width; ++x) {	
				Tile t;
				reader.read(&t, sizeof(Tile));
				_tiles->set(x,y,t);
			}
		}
		_queue.load(reader);
		calculateMaxResources();
    }
	else {
		printf("ERROR: No file %s found\n",buffer);
	}

}

// ------------------------------------------------------
// is_available - check if the required resources are available
// ------------------------------------------------------
bool Island::is_available(const Resources& costs) {
	bool ret = true;
	for ( int i = 0; i < _resources.total; ++i ) {
		int current = costs._values[i];
		int available = _resources._values[i];
		int global_available = _context->global_resources.get(i);
		if ( current > 0 ) {
			if ( available < current && global_available < current ) {
				printf("Not enough resources %s - required: %d available: %d\n",_context->resource_registry.getName(i),current,available);
				ret = false;
			}
		}
	}
	return ret;
}

// ------------------------------------------------------
// is used - check if tile is already in use
// ------------------------------------------------------
bool Island::isUsed(int x, int y) {
	if (!_tiles->is_empty(x,y)) {
		return true;
	}
	/*
	for (size_t i = 0; i < _queue.size(); ++i) {
		WorkItem& item = _queue[i];
		if (item.tile_x == x && item.tile_y == y) {
			return true;
		}
	}
	*/
	return false;
}
