#pragma once

struct Tiles;
struct WorldContext;
struct MyIsland;

namespace vis {

	void print_map(MyIsland* island, int x, int y, int size);

	void print_status(MyIsland* island);

	bool describe_building(MyIsland* island, int x, int y);
}

