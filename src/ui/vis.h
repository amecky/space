#pragma once

struct Tiles;
struct WorldContext;
struct MyIsland;

namespace vis {

	void print_map(WorldContext* context,MyIsland* island, int x, int y, int size);

	void print_status(WorldContext* context, MyIsland* island);

	bool describe_building(WorldContext* context, MyIsland* island, int x, int y);
}

