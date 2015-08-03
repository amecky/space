#pragma once
#include <vector>
#include "RegistryReader.h"
#include "..\utils\Log.h"
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
			LOGC("Registry") << "loading data/" << fileName;
			for ( int i = 0; i < r.size(); ++i ) {
				T t;
				if ( load_entry(r,i,&t) ) {
					_items.push_back(t);
				}
			}
			LOGC("Registry") << "items: " << _items.size();
			validate_data();
		}
		else {
			LOGEC("Registry") << "File data/" << fileName << " not found";
		}

	}
	virtual bool load_entry(const RegistryReader& reader,int index,T* t) = 0;
	virtual bool validate_data() {
		return true;
	}
protected:
	Items _items;

};