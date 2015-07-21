#pragma once
#include <vector>
// ------------------------------------------------------
// Work type
// ------------------------------------------------------
enum WorkType {
	WT_BUILD,
	WT_RUN,
	WT_UPGRADE,
	WT_DELETE
};

// ------------------------------------------------------
// Work item
// ------------------------------------------------------
struct WorkItem {

	int tile_x;
	int tile_y;
	int type; //WorkType
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

	int work_type;
	int tile_x;
	int tile_y;
	int modus;
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
	WorkQueue(void);
	~WorkQueue(void);	
	void tick(int timeUnits);
	void createWork(int price_type,int x,int y,int building_id, int level,int duration);
	bool hasEvents() {
		return _buffer.size > 0;
	}
	const Event& getEvent(int index) const {
		return _buffer.get(index);
	}
	const int event_size() const {
		return _buffer.size;
	}
	void show() const;
	void save(FILE* f);
	void load(FILE* f);
private:
	void processQueueCommon(int timeUnits);
	Queue _queue;
	EventBuffer _buffer;
};

