#pragma once

struct Tiles;
struct WorldContext;
class Island;

namespace vis {

	void print_map(Island* island, int x, int y, int size);

	bool describe_building(Island* island, int x, int y);

	void print_tasks(Island* island);

	void print_building_definitions();
}

