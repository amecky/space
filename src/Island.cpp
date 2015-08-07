#include "Island.h"
#include "registries\IslandRegistry.h"
#include "utils\files.h"
#include "World.h"
#include "utils\Serializer.h"

namespace island {

	// ------------------------------------------------------
	// save Island
	// ------------------------------------------------------
	void save(MyIsland* island) {
		char buffer[256];
		sprintf(buffer, "i_%d.bin", island->id);
		Serializer writer;
		if (writer.open(buffer, "data", BM_WRITE)) {
			// save resources
			int sz = gContext->resource_registry.size();
			writer.write(sz);
			LOGC("Island") << "saving resources: " << sz;
			for (int i = 0; i < sz; ++i) {
				writer.write(island->resources._values[i]);
			}
			res::log_resources(gContext->resource_registry, island->resources, false);
			LOGC("Island") << "saving tiles: " << island->tiles->width << " " << island->tiles->height;
			// save tiles
			for (int y = 0; y < island->tiles->height; ++y) {
				for (int x = 0; x < island->tiles->width; ++x) {
					writer.write(&island->tiles->get(x, y), sizeof(Tile));
				}
			}
			island->queue.save(writer);
		}
	}

	// ------------------------------------------------------
	// load Island
	// ------------------------------------------------------
	void load(MyIsland* island) {
		char buffer[256];
		sprintf(buffer, "i_%d.bin", island->id);
		Serializer reader;
		LOGC("Island") << "reading island - file: " << buffer;
		//_queue.clear();
		int max = island->tiles->total;
		island->tiles->clear();
		if (reader.open(buffer, "data", BM_READ)) {
			int num = 0;
			// load resources
			reader.read(&num);
			LOGC("Island") << "resources: " << num;
			for (int i = 0; i < num; ++i) {
				reader.read(&island->resources._values[i]);
			}
			res::log_resources(gContext->resource_registry, island->resources, false);
			LOGC("Island") << "loading tiles: " << island->tiles->width << " " << island->tiles->height;
			// load buildings
			for (int y = 0; y < island->tiles->height; ++y) {
				for (int x = 0; x < island->tiles->width; ++x) {
					Tile t;
					reader.read(&t, sizeof(Tile));
					island->tiles->set(x, y, t);
				}
			}
			island->queue.load(reader);			
			calculate_max_resources(island);
		}
		else {
			LOGEC("Island") << "No file '" << buffer << "' found";
		}

	}

	// ------------------------------------------------------
	// is_available - check if the required resources are available
	// ------------------------------------------------------
	bool is_available(MyIsland* island,const Resources& costs) {
		bool ret = true;
		for (int i = 0; i < island->resources.total; ++i) {
			int current = costs._values[i];
			int available = island->resources._values[i];
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
	bool check_requirements(MyIsland* island, int building_id, int level) {
		BuildRequirement requirement;
		if (gContext->requirements_registry.getRequirement(building_id, level, &requirement)) {
			int cnt = 0;
			for (int i = 0; i < island->tiles->total; ++i) {
				if (island->tiles->match(i, requirement)) {
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
	void add_resource(MyIsland* island,int resource_id, int amount) {
		if (gContext->resource_registry.isGlobal(resource_id)) {
			gContext->global_resources.add(resource_id, amount);
		}
		else {
			island->resources.add(resource_id, amount);
		}
	}

	// ------------------------------------------------------
	// add resource
	// ------------------------------------------------------
	void add_resource(MyIsland* island, const Sign& sign, int value) {
		int id = gContext->resource_registry.getIndex(sign);
		add_resource(island, id, value);
	}

	// ------------------------------------------------------
	// add resource
	// ------------------------------------------------------
	void add_resource(MyIsland* island,const char* sign,int value) {
		assert(strlen(sign) == 2);
		Sign s(sign[0],sign[1]);
		int id = gContext->resource_registry.getIndex(s);
		island->resources.set(id,value);
	}

	// ------------------------------------------------------
	// add resources
	// ------------------------------------------------------
	void add_resources(MyIsland* island,const Resources& r) {
		for (int i = 0; i < gContext->resource_registry.size(); ++i) {
			if (gContext->resource_registry.isGlobal(i)) {
				gContext->global_resources.add(i, r._values[i]);
			}
			else {
				island->resources.add(i,r._values[i]);
			}
		}
	}


	// ------------------------------------------------------
	// subtract resources
	// ------------------------------------------------------
	void sub_resources(MyIsland* island,const Resources& r) {
		for (int i = 0; i < gContext->resource_registry.size(); ++i) {
			if (gContext->resource_registry.isGlobal(i)) {
				gContext->global_resources.sub(i, r._values[i]);
			}
			else {
				island->resources.sub(i, r._values[i]);
			}
		}
	}


	// ------------------------------------------------------
	// calculate max resources
	// ------------------------------------------------------
	void calculate_max_resources(MyIsland* island) {
		for (int i = 0; i < island->maxResources.total; ++i) {
			island->maxResources.set(i, 0);
		}
		for (int i = 0; i < island->tiles->total; ++i) {
			if (island->tiles->getBuildingID(i) != -1) {
				gContext->max_resources_registry.add(island->tiles->getBuildingID(i), island->tiles->getLevel(i), &island->maxResources);
			}
		}
	}

	// ------------------------------------------------------
	// add building
	// ------------------------------------------------------
	void add_building(MyIsland* island,int x,int y,const Sign& s) {
		BuildingDefinition def;
		gContext->building_definitions.getDefinition(s,&def);
		if (island->tiles->set(def.id,1,x,y,def.size_x,def.size_y)) {
			calculate_max_resources(island);
			Resources tmp;
			if (gContext->price_registry.get(PT_REGULAR, 0, def.id, 1, &tmp)) {		
				island->queue.createWork(PT_REGULAR,x,y,def.id,1,gContext->price_registry.getDuration(PT_REGULAR, def.id, 1));		
			}
		}
		else {
			LOGEC("Island") << "Cannot set " << def.name << " at " << x << " " << y;
		}
	}

	// ------------------------------------------------------
	// add building
	// ------------------------------------------------------
	void add_building(MyIsland* island,int x,int y,const char* s) {
		assert(strlen(s) == 2);
		BuildingDefinition def;
		Sign sign(s[0],s[1]);
		gContext->building_definitions.getDefinition(sign,&def);
		if (island->tiles->set(def.id,1,x,y,def.size_x,def.size_y)) {
			calculate_max_resources(island);
			Resources tmp;
			if (gContext->price_registry.get(PT_REGULAR, 0, def.id, 1, &tmp)) {		
				island->queue.createWork(PT_REGULAR,x,y,def.id,1,gContext->price_registry.getDuration(PT_REGULAR, def.id, 1));		
			}
		}
	}

	// ------------------------------------------------------
	// collect resources
	// ------------------------------------------------------
	bool collect(MyIsland* island,int x, int y) {
		LOGC("Island") << "collecting at " << x << " " << y;
		int idx = x + y * island->tiles->width;
		if (island->tiles->getBuildingID(idx) == -1) {
			gContext->messages.report_error("There is no building at %d %d", x, y);
			return false;
		}
		if ( !island->tiles->has_state(x,y,TS_COLLECTABLE)) {
			gContext->messages.report_error("There is nothing to collect at %d %d", x, y);
			return false;
		}
		bool found = false;
		Collectables::iterator it = island->collectables.begin();
		Resources saved;
		while ( it != island->collectables.end() ) {
			if ( it->tile_x == x && it->tile_y == y ) {
				if (gContext->price_registry.get(it->price_type, 2, island->tiles->get(it->tile_x,it->tile_y), &saved)) {
					//_resources.add(saved);
					add_resources(island,saved);
					res::show_resources(gContext->resource_registry,saved,false);
				}
				if (gContext->price_registry.get(PT_REGULAR, 0,  island->tiles->get(it->tile_x,it->tile_y), &saved)) {
					island->queue.createWork(PT_REGULAR,x,y,it->building_id,it->level,gContext->price_registry.getDuration(PT_REGULAR,it->building_id,it->level));
				}
				island->tiles->clear_state(x,y,TS_COLLECTABLE);
				if ( it->remove ) {
					BuildingDefinition def;
					gContext->building_definitions.getDefinition(it->building_id,&def);
					island->tiles->remove(it->tile_x,it->tile_y,def.size_x,def.size_y);
				}
				it = island->collectables.erase(it);
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
	bool move(MyIsland* island,int oldX,int oldY,int x,int y) {
		int oldIndex = oldX + oldY * island->tiles->width;
		int index = x + y * island->tiles->width;
		if ( island->tiles->is_empty(oldX,oldY) ) {
			gContext->messages.report_error("There is no building at %d %d",x,y);
			return false;
		}
		if ( island->tiles->isActive(oldIndex)) {
			gContext->messages.report_error("The building is active - you cannot move an active building");
			return false;
		}
		if ( !island->tiles->is_empty(x,y)) {
			gContext->messages.report_error("There is already a building at %d %d",x,y);
			return false;
		}
		island->tiles->move(oldIndex,index);
		return true;
	}

	// ------------------------------------------------------
	// remove building
	// ------------------------------------------------------
	void remove_building(MyIsland* island,int building_id,int x,int y) {
		LOGC("Island") << "removing build at " << x << " " << y;
		Tile& t = island->tiles->get(x,y);
		BuildingDefinition def;
		gContext->building_definitions.getDefinition(building_id,&def);
		island->tiles->remove(x,y,def.size_x,def.size_y);
	}

}