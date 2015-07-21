#pragma once
#include <stdio.h>

const int GRID_SIZE = 256;
// ------------------------------------------------------
// Tile
// ------------------------------------------------------
struct Tile {

	int building_id;
	bool active;
	int level;
	int ref_id;

	Tile() : building_id(-1) , active(false) , level(0) , ref_id(-1) {}
	Tile(int bid,int lvl) : building_id(bid) , active(false) , level(lvl) , ref_id(-1) {}

};

// ------------------------------------------------------
// Tiles
// ------------------------------------------------------
struct Tiles {

	Tile _tiles[GRID_SIZE * GRID_SIZE];

	void clear() {
		for ( int i = 0; i< GRID_SIZE * GRID_SIZE; ++i ) {
			_tiles[i].building_id = -1;
			_tiles[i].ref_id = -1;
			_tiles[i].active = false;
			_tiles[i].level = 0;
		}
	}

	void set(int x,int y,const Tile& t) {
		_tiles[x + y * GRID_SIZE] = t;
	}

	const Tile& get(int x,int y) const {
		return _tiles[x + y * GRID_SIZE];
	}

	const int get_index(int x,int y) const {
		return x + y * GRID_SIZE;
	}

	Tile& get(int x,int y) {
		return _tiles[x + y * GRID_SIZE];
	}

	const int getBuildingID(int index) const {
		if ( index >= 0 && index < (GRID_SIZE * GRID_SIZE)) {
			return _tiles[index].building_id;
		}
		return -1;
	}

	const int getLevel(int index) const {
		if ( index >= 0 && index < (GRID_SIZE * GRID_SIZE)) {
			return _tiles[index].level;
		}
		return -1;
	}

	const bool isActive(int index) const {
		return _tiles[index].active;
	}
	
	const bool is_valid(int x,int y) const {
		if ( x >= 0 && x < GRID_SIZE && y >= 0 && y < GRID_SIZE) {
			return true;
		}
		return false;
	}

	const bool is_empty(int x,int y) const {
		if ( is_valid(x,y)) {
			const Tile& t = get(x,y);
			return t.building_id == -1 && t.ref_id == -1;
		}
		return false;
	}

	void setActive(int index,bool active) {
		_tiles[index].active = active;
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

	bool has_space(int x,int y,int size_x,int size_y) {
		if ( !is_valid(x,y))  {
			return false;
		}
		if ( size_x == 1 && size_y == 1 ) {
			return is_empty(x,y);
		}
		int dy = y - size_y + 1;
		if ( (x + size_x) >= GRID_SIZE ) {
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
};
