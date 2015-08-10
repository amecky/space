#include "vis.h"
#include <stdio.h>
#include "..\Tiles.h"
#include "..\World.h"
#include "..\Island.h"

namespace vis {

	void print_map(MyIsland* island, int centerX, int centerY, int size) {
		Tiles* tiles = island->tiles;
		int xmin = centerX - size;
		int xmax = centerX + size;
		int ymin = centerY - size;
		int ymax = centerY + size;
		if (xmin < 0) {
			xmin = 0;
			xmax = 2 * size;
		}
		if (xmax >= tiles->width) {
			xmin = tiles->width - 2 * size;
			xmax = tiles->width;
		}
		if (ymin < 0) {
			ymin = 0;
			ymax = 2 * size;
		}
		if (ymax >= tiles->height) {
			ymin = tiles->height - 2 * size;
			ymax = tiles->height;
		}
		for ( int y = ymax - 1; y >= ymin; --y ) {
			printf("%2d | ", y);
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
						printf("%s", gContext->building_definitions.getSign(tiles->getBuildingID(idx)));
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
		printf("   -----------------------------------------------------------------\n");
		printf("     ");
		for (int i = xmin; i < xmax; ++i) {
			printf("%2d  ", i);
		}
		printf("\n");
	}

	// ------------------------------------------------------
	// show status
	// ------------------------------------------------------
	void print_status(MyIsland* island) {
		printf("Global Resources:\n");
		for (int i = 0; i < gContext->resource_registry.size(); ++i) {
			if (gContext->resource_registry.isGlobal(i)) {
				printf("%10s : %d\n", gContext->resource_registry.getName(i), gContext->global_resources.get(i));
			}
		}
		printf("Resources:\n");
		for (int i = 0; i < gContext->resource_registry.size(); ++i) {
			if (!gContext->resource_registry.isGlobal(i)) {
				printf("%10s : %d / %d\n", gContext->resource_registry.getName(i), island->resources.get(i), island->maxResources.get(i));
			}
		}
		island->queue.show();
		if (gContext->collect_mode == CM_MANUAL) {
			printf("Collectable:\n");
			for (size_t i = 0; i < island->collectables.size(); ++i) {
				printf("  %s at %d %d\n", gContext->building_definitions.getName(island->collectables[i].building_id), island->collectables[i].tile_x, island->collectables[i].tile_y);
			}
		}
	}

	// ------------------------------------------------------
	// describe building
	// ------------------------------------------------------
	bool describe_building(MyIsland* island,int x, int y) {
		Tiles* tiles = island->tiles;
		int idx = x + y * tiles->width;
		if (tiles->getBuildingID(idx) == -1) {
			gContext->messages.report_error("There is no building at %d %d", x, y);
			return false;
		}
		const Tile& t = tiles->get(x, y);
		printf("Building: %s\n", gContext->building_definitions.getName(t.building_id));
		printf("   Level: %d\n", t.level);
		Resources tmp;
		if (gContext->price_registry.get(PT_REGULAR, 2, t.building_id, t.level, &tmp)) {
			printf("Regular income:\n");
			printf("Duration: %d\n", gContext->price_registry.getDuration(PT_REGULAR, t.building_id, t.level));
			res::show_resources(gContext->resource_registry, tmp, false);
		}
		printf("Available work:\n");
		for (int i = 1; i <= t.level; ++i) {
			if (gContext->price_registry.get(PT_WORK, 0, t.building_id, i, &tmp)) {
				printf(" Level %d\n", i);
				printf("Duration: %d\n", gContext->price_registry.getDuration(PT_WORK, t.building_id, i));
				printf("Costs:\n");
				res::show_resources(gContext->resource_registry, tmp, false);
			}
			if (gContext->price_registry.get(PT_WORK, 1, t.building_id, i, &tmp)) {
				printf("Income:\n");
				Resources collect;
				if (gContext->price_registry.get(PT_WORK, 2, t.building_id, i, &collect)) {
					tmp.add(collect);

				}
				res::show_resources(gContext->resource_registry, tmp, false);
			}
		}
		// FIXME: list start options
		return true;
	}

	// ------------------------------------------------------
	// show tasks
	// ------------------------------------------------------
	void print_tasks(MyIsland* island) {
		ActiveTasks tasks;
		Reward rewards[16];
		gContext->task_queue.get_active_tasks(island->id,tasks);
		for (size_t i = 0; i < tasks.size(); ++i) {
			printf("Task: ");
			printf("%s  ", tasks[i].task->text);
			if (gContext->reward_registry.contains(tasks[i].task->id)) {
				int cnt = gContext->reward_registry.get(tasks[i].task->id, rewards, 16);
				if (cnt > 0) {
					printf("Rewards: ");
					for (int j = 0; j < cnt; ++j) {
						printf("%d %s ", rewards[j].amount,gContext->resource_registry.getName(rewards[j].resource_id));
					}				
				}
			}
			printf("\n");
			printf(" => %d / %d\n",tasks[i].count,tasks[i].task->amount);		
		}
	}

	void print_building_definitions() {
		BuildingDefinition def;		
		for (size_t i = 0; i < gContext->building_definitions.size(); ++i) {
			gContext->building_definitions.getDefinition(i,&def);
			printf("%d %s %s\n",def.id,def.sign.c_str(),def.name);
		}
	}
}