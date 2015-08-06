#include "vis.h"
#include <stdio.h>
#include "..\Tiles.h"
#include "..\World.h"
#include "..\Island.h"

namespace vis {

	void print_map(WorldContext* context, MyIsland* island, int centerX, int centerY, int size) {
		Tiles* tiles = island->tiles;
		int xmin = centerX - size;
		int xmax = centerX + size;
		int ymin = centerY - size;
		int ymax = centerY + size;
		if (xmin < 0) {
			int d = 8 - centerX;
			xmin += d;
			xmax += d;
		}
		if (xmax >= tiles->width) {
			int d = tiles->width - size;
			xmin -= d;
			xmax -= d;
		}
		if (ymin < 0) {
			int d = size - centerY;
			ymin += d;
			ymax += d;
		}
		if (ymax >= tiles->height) {
			int d = tiles->height - size;
			ymin -= d;
			ymax -= d;
		}
		for ( int y = ymax - 1; y >= ymin; --y ) {
			printf("%2d ", y);
			for ( int x = xmin; x < xmax; ++x ) {
				int idx = x + y * tiles->width;
				if ( tiles->has_state(x,y,TS_LOCKED)) {
					printf("??  ");
				}
				else if ( tiles->has_state(x,y,TS_UNDEFINED)) {
					printf("    ");
				}
				else {
					if ( tiles->getBuildingID(idx) != -1 ) {
						printf("%s", context->building_definitions.getSign(tiles->getBuildingID(idx)));
						if ( tiles->isActive(idx)) {			
							printf("# ");
						}
						else if ( tiles->has_state(x,y,TS_COLLECTABLE)) {
							printf("* ");
						}
						else {
							printf("  ");
						}
					}
					else if ( tiles->_tiles[idx].ref_id != -1 ) {
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
	// show status
	// ------------------------------------------------------
	void print_status(WorldContext* context,MyIsland* island) {
		printf("Global Resources:\n");
		for (int i = 0; i < context->resource_registry.size(); ++i) {
			if (context->resource_registry.isGlobal(i)) {
				printf("%10s : %d\n", context->resource_registry.getName(i), context->global_resources.get(i));
			}
		}
		printf("Resources:\n");
		for (int i = 0; i < context->resource_registry.size(); ++i) {
			if (!context->resource_registry.isGlobal(i)) {
				printf("%10s : %d / %d\n", context->resource_registry.getName(i), island->resources.get(i), island->maxResources.get(i));
			}
		}
		island->queue.show();
		if (context->collect_mode == CM_MANUAL) {
			printf("Collectable:\n");
			for (size_t i = 0; i < island->collectables.size(); ++i) {
				printf("  %s at %d %d\n", context->building_definitions.getName(island->collectables[i].building_id), island->collectables[i].tile_x, island->collectables[i].tile_y);
			}
		}
	}

	// ------------------------------------------------------
	// describe building
	// ------------------------------------------------------
	bool describe_building(WorldContext* context,MyIsland* island,int x, int y) {
		Tiles* tiles = island->tiles;
		int idx = x + y * tiles->width;
		if (tiles->getBuildingID(idx) == -1) {
			printf("Error: There is no building at %d %d\n", x, y);
			return false;
		}
		const Tile& t = tiles->get(x, y);
		printf("Building: %s\n", context->building_definitions.getName(t.building_id));
		printf("   Level: %d\n", t.level);
		Resources tmp;
		if (context->price_registry.get(PT_REGULAR, 2, t.building_id, t.level, &tmp)) {
			printf("Regular income:\n");
			printf("Duration: %d\n", context->price_registry.getDuration(PT_REGULAR, t.building_id, t.level));
			res::show_resources(context->resource_registry, tmp, false);
		}
		printf("Available work:\n");
		for (int i = 1; i <= t.level; ++i) {
			if (context->price_registry.get(PT_WORK, 0, t.building_id, i, &tmp)) {
				printf(" Level %d\n", i);
				printf("Duration: %d\n", context->price_registry.getDuration(PT_WORK, t.building_id, i));
				printf("Costs:\n");
				res::show_resources(context->resource_registry, tmp, false);
			}
			if (context->price_registry.get(PT_WORK, 1, t.building_id, i, &tmp)) {
				printf("Income:\n");
				Resources collect;
				if (context->price_registry.get(PT_WORK, 2, t.building_id, i, &collect)) {
					tmp.add(collect);

				}
				res::show_resources(context->resource_registry, tmp, false);
			}
		}
		// FIXME: list start options
		return true;
	}
}