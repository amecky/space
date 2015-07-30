#pragma once

struct Tiles;
struct WorldContext;

namespace vis {

	void print_map(const Tiles* tiles,int x,int y,int size,WorldContext* context);
}

