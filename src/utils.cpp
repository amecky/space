#include "utils.h"
#include <assert.h>

namespace bits {

	void set(int* value,int index) {
		*value = *value | 1 << index;
	}

	void clear(int* value,int index) {
		*value = *value & ~(1 << index);
	}

	bool is_set(int value,int index) {
		return value & 1 << index;
	}
}

const char PRICE_TYPES[] = { 'W', 'U', 'D', 'B', 'P', 'R' };
const char* PRICE_NAMES[] = { "Work", "Update", "Delete", "Build", "Permanent", "Regular" };
const char* STAGE_NAMES[] = { "Start", "Finish", "Collect", "Unknown"};

namespace reg {

	int translate_work(char c) {
		for (int j = 0; j < 6; ++j) {
			if (c == PRICE_TYPES[j]) {
				return j;
			}
		}
		return -1;
	}

	const char* translate_work(int idx) {
		assert(idx >= 0 && idx < 6);
		return PRICE_NAMES[idx];
	}

	int translate_stage(char c) {
		if ( c == 'S' ) {
			return 0;
		}
		else if (c== 'F') {
			return 1;
		}
		else if (c== 'C') {
			return 2;
		}
		return -1;	
	}

	const char* translate_stage(int id) {
		if ( id >= 0 && id < 3 ) {
			return STAGE_NAMES[id];
		}
		return STAGE_NAMES[3];
		
	}

}