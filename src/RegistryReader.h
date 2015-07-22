#pragma once
#include <string>
#include <vector>
#include <assert.h>
#include "Common.h"

const int MAX_FIELDS = 16;

class RegistryReader {

struct Entry {

	std::string line;
	int positions[MAX_FIELDS];
	int count;

	Entry() : count(0) {
		for ( int i = 0; i < MAX_FIELDS; ++i ) {
			positions[i] = -1;
		}
	}

	void set(int name_index,int position) {
		assert(name_index >= 0 && name_index < MAX_FIELDS);
		positions[name_index] = position;
	}

};

typedef std::vector<Entry> Lines;

public:
	RegistryReader(const char** field_names,int num);
	~RegistryReader(void);
	bool load(const char* fileName,const char* directory);
	const int size() const {
		return _lines.size();
	}
	int get_int(int line_nr,const char* name) const;
	char get_char(int line_nr,const char* name) const;
	bool get_bool(int line_nr,const char* name) const;
	Sign get_sign(int line_nr,const char* name) const;
	int get_string(int line_nr,const char* name,char* dest) const;
private:
	int find_index(const char* name) const;
	int find_end(const std::string& str,int start) const;
	const char** _field_names;
	int _num_fields;
	Lines _lines;	
};

