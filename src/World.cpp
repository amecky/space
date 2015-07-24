#include "World.h"
#include <stdio.h>
#include "files.h"
#include <assert.h>
#include "CSVFile.h"
#include <vector>
#include <string>

Island::Island(WorldContext* context,int size_x,int size_y) : _context(context) {
	_tiles = new Tiles(size_x,size_y);
	_tiles->clear();	
	
}


Island::~Island() {
	delete _tiles;
}

void Island::setCollectMode(CollectMode cm) {
	_collect_mode = cm;
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
			_context->price_registry.add_max_resources(_tiles->getBuildingID(i), _tiles->getLevel(i), &_maxResources);
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
void Island::tick(int timeUnits) {
	_queue.tick(timeUnits);
	if ( _queue.hasEvents()) {
		Resources saved;
		for ( int i = 0; i < _queue.event_size();++i) {
			const Event& e = _queue.getEvent(i);
			printf("event %d type %d at %d %d\n",i,e.work_type,e.tile_x,e.tile_y);
			// process type = finish immediately
			if (_context->price_registry.get(e.work_type, 1, e.building_id, e.level, &saved)) {
				//_resources.add(saved);
				addResources(saved);
			}
			// check if we have a collectable 
			if (_context->price_registry.get(e.work_type, 2, e.building_id, e.level, &saved)) {
				// simply build collectable
				Collectable c;
				c.building_id = e.building_id;
				c.level = e.level;
				c.price_type = e.work_type;
				c.tile_x = e.tile_x;
				c.tile_y = e.tile_y;
				_collectables.push_back(c);
			}
			else if (e.work_type == PT_UPGRADE) {
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
				Tile& t = _tiles->get(e.tile_x,e.tile_y);
				// FIXME: remove all tiles
				t.building_id = -1;
			}
		}
	}

	// immediate mode -> process all collectables directly
	if ( _collect_mode == CM_IMMEDIATE ) {
		for ( size_t i = 0; i < _collectables.size(); ++i ) {
			const Collectable& c = _collectables[i];
			Resources saved;
			if (_context->price_registry.get(c.price_type, 2, c.building_id, c.level, &saved)) {
				//_resources.add(saved);
				addResources(saved);
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
// show resources
// ------------------------------------------------------
/*
void Island::showResources(const Resources& res,bool complete) {
	printf("Resources:\n");
	for ( int i = 0; i < _context->resource_registry.size(); ++i ) {
		if ( complete) {
			printf("%10s : %d\n",_context->resource_registry.getName(i),res.get(i));
		}
		else {
			if ( res.get(i) > 0 ) {
				printf("%10s : %d\n",_context->resource_registry.getName(i),res.get(i));
			}
		}
	}	
}
*/
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
	if ( _collect_mode == CM_MANUAL) {
		printf("Collectable:\n");
		for ( size_t i = 0; i < _collectables.size(); ++i ) {
			printf("  %s at %d %d\n",_context->building_definitions.getName(_collectables[i].building_id),_collectables[i].tile_x,_collectables[i].tile_y);
		}
	}
}

// ------------------------------------------------------
// show building definitions
// ------------------------------------------------------
void Island::showBuildingDefinitions() {
	_context->building_definitions.show();
}

// ------------------------------------------------------
// show map
// ------------------------------------------------------
void Island::showMap(int centerX, int centerY) const {

	int xmin = centerX - 8;
	int xmax = centerX + 8;
	int ymin = centerY - 8;
	int ymax = centerY + 8;
	if (xmin < 0) {
		int d = 8 - centerX;
		xmin += d;
		xmax += d;
	}
	if (xmax >= _tiles->width) {
		int d = _tiles->width - 8;
		xmin -= d;
		xmax -= d;
	}
	if (ymin < 0) {
		int d = 8 - centerY;
		ymin += d;
		ymax += d;
	}
	if (ymax >= _tiles->height) {
		int d = _tiles->height - 8;
		ymin -= d;
		ymax -= d;
	}
	for ( int y = ymax - 1; y >= ymin; --y ) {
		printf("%2d ", y);
		for ( int x = xmin; x < xmax; ++x ) {
			int idx = x + y * _tiles->width;
			if ( _tiles->has_state(x,y,TS_LOCKED)) {
				printf("??  ");
			}
			else if ( _tiles->has_state(x,y,TS_UNDEFINED)) {
				printf("    ");
			}
			else {
				if ( _tiles->getBuildingID(idx) != -1 ) {
					//printf("%s%1d", _building_definitions.getSign(_tiles->getBuildingID(idx)), _tiles->getLevel(idx));
					printf("%s", _context->building_definitions.getSign(_tiles->getBuildingID(idx)));
					if ( _tiles->isActive(idx)) {			
						printf("#");
					}
					else {
						printf(" ");
					}
					if ( isCollectable(x,y)) {
						printf("*");
					}
					else {
						printf(" ");
					}
				}
				else if ( _tiles->_tiles[idx].ref_id != -1 ) {
					printf("xx  ");
				}
				else {
					printf("-   ");
				}
			}
		}
		printf("\n");
	}
	printf("  ");
	for (int i = xmin; i < xmax; ++i) {
		printf("%2d  ", i);
	}
	printf("\n");
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
	if ( !isCollectable(x,y)) {
		printf("Error: There is nothing to collect at %d %d\n", x, y);
		return false;
	}
	bool found = false;
	Collectables::iterator it = _collectables.begin();
	Resources saved;
	while ( it != _collectables.end() ) {
		if ( it->tile_x == x && it->tile_y == y ) {
			if (_context->price_registry.get(it->price_type, 2, it->building_id, it->level, &saved)) {
				//_resources.add(saved);
				addResources(saved);
				res::show_resources(_context->resource_registry,saved,false);
			}
			if (_context->price_registry.get(PT_REGULAR, 0, it->building_id, it->level, &saved)) {
				_queue.createWork(PT_REGULAR,x,y,it->building_id,it->level,_context->price_registry.getDuration(PT_REGULAR,it->building_id,it->level));
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
	Resources tmp;
	if ( !_context->price_registry.get(PT_WORK,0,_tiles->getBuildingID(idx),level,&tmp)) {
		printf("Error: The selected level %d is not supported yet - You need to upgrade\n",level);
		return false;
	}
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
	createWork(PT_DELETE,x,y,t.building_id,t.level);	
	return true;
}

// ------------------------------------------------------
// check requirements
// ------------------------------------------------------
bool Island::checkRequirements(int building_id, int level) {
	BuildRequirement requirement;
	if (_context->price_registry.getRequirement(building_id, level, &requirement)) {
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
void Island::save(int index) {
	char buffer[256];
	sprintf(buffer,"i_%d.bin",index);
	FILE *f = fopen(buffer,"wb");
	if (f) {
		// save resources
		int sz = _context->resource_registry.size();
		fwrite(&sz,sizeof(int),1,f);
        for ( int i = 0; i < sz; ++i ) {
			fwrite(&_resources._values[i],sizeof(int),1,f);
        }
		// save tiles
		for (int y = 0; y < _tiles->height; ++y) {
			for (int x = 0; x < _tiles->width; ++x) {
				const Tile& t = _tiles->get(x,y);
				fwrite(&t, sizeof(Tile), 1, f);
			}
		}   
		_queue.save(f);		
        fclose(f);
	}
}

// ------------------------------------------------------
// load Island
// ------------------------------------------------------
void Island::load(int index) {
	char buffer[256];
	sprintf(buffer,"i_%d.bin",index);
	FILE *f = fopen(buffer,"rb");
	//_queue.clear();
	int max = _tiles->total;
	_tiles->clear();
	if (f) {
        int num = 0;
		// load resources
		fread(&num,sizeof(int),1,f);
		for ( int i = 0; i < num; ++i ) {
			fread(&_resources._values[i],sizeof(int),1,f);
		}

		res::show_resources(_context->resource_registry,_resources,false);

		// load buildings
		for (int y = 0; y < _tiles->height; ++y) {
			for (int x = 0; x < _tiles->width; ++x) {	
				Tile t;
				fread(&t, sizeof(Tile), 1, f);
				_tiles->set(x,y,t);
			}
		}
		_queue.load(f);
		calculateMaxResources();
        fclose(f);
    }
	else {
		printf("ERROR: No file %s found\n",buffer);
	}

}

// ------------------------------------------------------
// load Island
// ------------------------------------------------------
void Island::load_txt(int index) {
	CSVFile file;
	_tiles->clear();
	//_queue.clear();
	if ( file.load("i_d_1.txt","data") ) {
		for ( size_t i = 0; i < file.size(); ++i ) {
			const TextLine& l = file.get(i);
			Sign s = l.get_sign(0);
			int x = l.get_int(1);
			int y = l.get_int(2);
			add(x,y,s);
		}
	}	
	calculateMaxResources();    
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

// ------------------------------------------------------
// is collectable - check if building at position has pending collections
// ------------------------------------------------------
bool Island::isCollectable(int x,int y) const {
	for ( size_t i = 0; i < _collectables.size(); ++i ) {
		if ( _collectables[i].tile_x == x && _collectables[i].tile_y == y ) {
			return true;
		}
	}
	return false;
}

World::World() : _selected(-1) {
	_context.building_definitions.load();
	_context.resource_registry.load();		
	_context.price_registry.load();
	_context.price_registry.load_requirements();
	_context.price_registry.load_max_resources();
	_context.task_registry.load();
	_context.collect_mode = CM_IMMEDIATE;
	_context.time_multiplier = 1;
}

Island* World::createIsland(int width,int height) {
	Island* i = new Island(&_context,width,height);
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

void World::tick(int timeUnits) {
	for (size_t i = 0; i < _islands.size(); ++i) {
		_islands[i]->tick(timeUnits);
	}
}

void World::addResource(const Sign& sign, int value) {
	int id = _context.resource_registry.getIndex(sign);
	_context.global_resources.add(id, value);
}

void World::save() {
	FILE *f = fopen("world.bin","wb");
	if (f) {
		// save global resources
		int sz = _context.global_resources.total;
		fwrite(&sz,sizeof(int),1,f);
        for ( int i = 0; i < sz; ++i ) {
			fwrite(&_context.global_resources._values[i],sizeof(int),1,f);
        }
		int num = _islands.size();
		fwrite(&num,sizeof(int),1,f);
		fclose(f);
		// save every island
		for ( size_t i = 0; i < _islands.size(); ++i ) {
			_islands[i]->save(i);
		}
	}
}

void World::load() {
	FILE *f = fopen("world.bin","rb");
	for ( size_t i = 0; i < _islands.size(); ++i ) {
		delete _islands[i];
	}
	_islands.clear();
	//_queue.clear();
	if (f) {
        int num = 0;
		// load global resources
		fread(&num,sizeof(int),1,f);
		for ( int i = 0; i < num; ++i ) {
			fread(&_context.global_resources._values[i],sizeof(int),1,f);
		}
		// load islands
		fread(&num,sizeof(int),1,f);
		for ( int i = 0; i < num; ++i ) {
			Island* is = new Island(&_context,32,32);
			is->load(i);
			_islands.push_back(is);
		}
        fclose(f);
    }
}

WorldContext* World::getContext() {
	return &_context;
}