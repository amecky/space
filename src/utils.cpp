#include "utils.h"

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