#pragma once
#include <vector>
#include "..\Common.h"
#include "..\registries\PriceRegistry.h"

class Serializer;

// ------------------------------------------------------
// Work item
// ------------------------------------------------------
struct WorkItem {

	int tile_x;
	int tile_y;
	WorkType work_type; 
	int timer;
	int duration;
	int price_index;
	int building_id;
	int level;
	bool reschedule;
	bool done;
};

// ------------------------------------------------------
// Event
// ------------------------------------------------------
struct Event {

	WorkType work_type;
	int tile_x;
	int tile_y;
	int building_id;
	int level;

};

// ------------------------------------------------------
// event buffer
// ------------------------------------------------------
struct EventBuffer {

	Event events[256];
	int size;

	void clear() {
		size = 0;
	}

	void add(const Event& e) {
		events[size++] = e;
	}

	const Event& get(int index) const {
		return events[index];
	}
};

// ------------------------------------------------------
// Work queue
// ------------------------------------------------------
class WorkQueue {

typedef std::vector<WorkItem> Queue;

public:
	WorkQueue(PriceRegistry* price_registry);
	~WorkQueue(void);	
	void tick(int timeUnits);
	void createWork(WorkType work_type,int x,int y,int building_id, int level,int duration);
	bool hasEvents() {
		return _buffer.size > 0;
	}
	const Event& getEvent(int index) const {
		return _buffer.get(index);
	}
	const int event_size() const {
		return _buffer.size;
	}
	void remove(WorkType work_type, int x,int y);
	void show() const;
	void save(Serializer& writer);
	void load(Serializer& reader);
private:
	PriceRegistry* _price_registry;
	Queue _queue;
	EventBuffer _buffer;
};

