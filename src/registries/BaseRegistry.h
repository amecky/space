#pragma once
#include <vector>
#include "RegistryReader.h"
// ------------------------------------------------------
// base registry
// ------------------------------------------------------
template<class T>
class BaseRegistry {

public:
	typedef std::vector<T> Items;
	const size_t size() const {
		return _items.size();
	}
	const T& get(size_t index) const {
		assert(index >= 0 && index < size());
		return _items[index];
	}
	T& get(size_t index) {
		assert(index >= 0 && index < size());
		return _items[index];
	}
	virtual const char** get_field_names() const = 0;
	virtual int get_field_num() const = 0;
	void load(const char* fileName) {
		RegistryReader r(get_field_names(),get_field_num());
		if ( r.load(fileName,"data") ) {
			printf("loading data/%s\n",fileName);
			for ( int i = 0; i < r.size(); ++i ) {
				T t;
				if ( load_entry(r,i,&t) ) {
					_items.push_back(t);
				}
			}
			validate_data();
		}
		//else {
			//printf("ERROR: File data/%s not found\n",fileName);
		//}

	}
	virtual bool load_entry(const RegistryReader& reader,int index,T* t) = 0;
	virtual bool validate_data() {
		return true;
	}
protected:
	Items _items;

};