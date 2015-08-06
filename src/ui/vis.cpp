#include "vis.h"
#include <stdio.h>
#include "..\Tiles.h"
#include "..\World.h"

namespace vis {

	void print_map(const Tiles* tiles,int centerX,int centerY,int size,WorldContext* context) {
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
}