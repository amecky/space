#pragma once
#include <stdio.h>
#include "utils\utils.h"
#include "registries\RequirementsRegistry.h"
// tile is not defined and cannot be used
const int TS_UNDEFINED   = 1;
// tile active
const int TS_ACTIVE      = 2;
// tile empty 
const int TS_EMPTY       = 3;
// tile is part of an unlocked area
const int TS_LOCKED      = 4;
// there is something to collect
const int TS_COLLECTABLE = 5;
// ------------------------------------------------------
// Tile
// ------------------------------------------------------
struct Tile {

	int building_id;
	int state;
	int level;
	int ref_id;

	Tile() : building_id(-1) , level(0) , ref_id(-1) {
		bits::set(&state,TS_EMPTY);
	}
	Tile(int bid,int lvl) : building_id(bid) , level(lvl) , ref_id(-1) {
		bits::set(&state,TS_EMPTY);
	}

};

// ------------------------------------------------------
// Tiles
// ------------------------------------------------------
struct Tiles {

	Tile* _tiles;
	int width;
	int height;
	int total;

	Tiles(int w,int h) : width(w) , height(h) {
		total = w * h;
		_tiles = new Tile[total];
	}

	~Tiles() {
		delete[] _tiles;
	}

	void clear() {
		for ( int i = 0; i < total; ++i ) {
			_tiles[i].building_id = -1;
			_tiles[i].ref_id = -1;
			_tiles[i].level = 0;
			_tiles[i].state = 0;
			bits::set(&_tiles[i].state,TS_EMPTY);
		}
	}

	void set(int x,int y,const Tile& t) {
		_tiles[x + y * width] = t;
	}

	const Tile& get(int x,int y) const {
		return _tiles[x + y * width];
	}

	const int get_index(int x,int y) const {
		return x + y * width;
	}

	Tile& get(int x,int y) {
		return _tiles[x + y * width];
	}

	const int getBuildingID(int index) const {
		if ( index >= 0 && index < total) {
			return _tiles[index].building_id;
		}
		return -1;
	}

	const int getLevel(int index) const {
		if ( index >= 0 && index < total) {
			return _tiles[index].level;
		}
		return -1;
	}

	const int get_level(int x,int y) const {
		assert(x >= 0 && x < width);
		assert(y >= 0 && y < height);
		return _tiles[x + y * width].level;
	}

	const int get_building(int x,int y) const {
		assert(x >= 0 && x < width);
		assert(y >= 0 && y < height);
		return _tiles[x + y * width].building_id;
	}

	const bool isActive(int index) const {
		return bits::is_set(_tiles[index].state,TS_ACTIVE);
	}
	
	const bool is_valid(int x,int y) const {
		if ( x >= 0 && x < width && y >= 0 && y < height) {
			return true;
		}
		return false;
	}

	const bool is_empty(int x,int y) const {
		if ( is_valid(x,y)) {
			const Tile& t = get(x,y);
			return t.building_id == -1 && t.ref_id == -1 && !bits::is_set(t.state,TS_LOCKED);
		}
		return false;
	}

	void setActive(int index,bool active) {
		bits::set(&_tiles[index].state,TS_ACTIVE);
	}

	const bool match(int index,const BuildRequirement& requirement) const {
		if (_tiles[index].building_id == requirement.building_type && _tiles[index].level >= requirement.level) {
			return true;
		}
		return false;
	}

	void move(int oldIndex,int index) {
		_tiles[index] = _tiles[oldIndex];
		_tiles[oldIndex].building_id = -1;
	}

	bool set(int building_id,int level,int x,int y,int size_x,int size_y) {
		if ( has_space(x,y,size_x,size_y)) {
			int idx = get_index(x,y);
			_tiles[idx].building_id = building_id;
			_tiles[idx].level = level;
			_tiles[idx].ref_id = -1;
			for ( int i = 0 ; i < size_x; ++i ) {
				for ( int j = 0; j < size_y; ++j ) {
					int xp = x + i;
					int yp = y - j;
					int current = get_index(xp,yp);
					if ( current != idx ) {
						_tiles[current].ref_id = idx;
					}
				}
			}
			return true;
		}
		return false;
	}

	const bool has_space(int x,int y,int size_x,int size_y) const {
		if ( !is_valid(x,y))  {
			return false;
		}
		if ( size_x == 1 && size_y == 1 ) {
			return is_empty(x,y);
		}
		int dy = y - size_y + 1;
		if ( (x + size_x) >= width ) {
			return false;
		}
		if ( (y - size_y) < 0 ) {
			return false;
		}
		for ( int i = 0 ; i < size_x; ++i ) {
			for ( int j = 0; j < size_y; ++j ) {
				int xp = x + i;
				int yp = y - j;
				if ( !is_empty(xp,yp) ) {
					return false;
				}
			}
		}
		return true;
	}

	void set_state(int x,int y,int state) {
		bits::set(&_tiles[x + y * width].state,state);
	}

	void clear_state(int x,int y,int state) {
		bits::clear(&_tiles[x + y * width].state,state);
	}

	const bool has_state(int x,int y,int state) const {
		return bits::is_set(_tiles[x + y * width].state,state);
	}
};
