#pragma once
#include <assert.h>


const int MAX_RESOURCES = 16;

struct Resources {

	int _values[MAX_RESOURCES];
	int total;

	Resources() : total(MAX_RESOURCES) {
		for ( int i = 0; i < MAX_RESOURCES; ++i ) {
			_values[i] = 0;
		}
	}

	void add(const Resources& other) {
		for ( int i = 0; i < MAX_RESOURCES;++i ) {
			_values[i] += other._values[i];
		}
	}
	
	void add(int type,int value) {
		if ( type >= 0 && type < MAX_RESOURCES) {
			_values[type] += value;
		}
	}
	
	void sub(int type,int value) {
		if ( type >= 0 && type < MAX_RESOURCES) {
			_values[type] -= value;
			if ( _values[type] < 0 ) {
				_values[type] = 0;
			}
		}
	}
	
	const int get(int type) const {
		if ( type >= 0 && type < MAX_RESOURCES) {
			return _values[type];
		}
		return 0;
	}

	void set(int type,int value) {
		assert(type >= 0 && type < MAX_RESOURCES);
		_values[type] = value;
	}
};

class ResourceRegistry;

namespace res {

	void show_resources(const ResourceRegistry& reg,const Resources& res,bool complete);

	void log_resources(const ResourceRegistry& reg,const Resources& res,bool complete);

}