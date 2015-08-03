#include "utils.h"
#include <assert.h>
#include <stdio.h>
#include <string>

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

namespace string {

	int append_int(int value,int offset,char* buffer,int max) {
		int start = offset;
		if ( value < 10 ) {
			sprintf_s(buffer+offset,max - offset,"0%d",value);			
		}
		else {
			sprintf_s(buffer+offset,max - offset,"%d",value);			
		}
		return offset + 2;
	}

	int append_string(const char* str,int offset,char* buffer,int max) {
		int len = strlen(str);
		sprintf_s(buffer+offset,max - offset,"%s",str);					
		return offset + len;
	}

	void format_duration(int seconds,char* buffer,int max) {
		int hours = seconds / 3600;
		int x = seconds - hours * 3600;
		int minutes = x / 60;
		x -= minutes * 60;
		int next = 0;
		if ( hours > 0 ) {
			next = append_int(hours,0,buffer,max);
			next = append_string(":",next,buffer,max);
		}
		next = append_int(minutes,next,buffer,max);
		next = append_string(":",next,buffer,max);
		next = append_int(x,next,buffer,max);
	}
}