#pragma once

enum WorkType {
	PT_WORK,
	PT_UPGRADE,
	PT_DELETE,
	PT_BUILD,
	PT_PERMANENT,
	PT_REGULAR,
	PT_UNKNOWN,
	PT_ITEM_COUNT
};


struct Sign {

	char _items[3];

	Sign() {
		_items[0] = '-';
		_items[1] = '-';
		_items[2] = '\0';
	}

	Sign(char f,char s) {
		_items[0] = f;
		_items[1] = s;
		_items[2] = '\0';
	}

	bool compare(const Sign& other) const {
		return other._items[0] == _items[0] && other._items[1] == _items[1];
	}

	bool compare(const char* c) const {
		if ( strlen(c) > 1 ) {
			return c[0] == _items[0] && c[1] == _items[1];
		}
		return false;
	}

	const char* c_str() const {
		return _items;
	}
};