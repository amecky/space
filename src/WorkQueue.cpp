#include "WorkQueue.h"
#include "Resources.h"
#include "PriceRegistry.h"

WorkQueue::WorkQueue(void) {
}


WorkQueue::~WorkQueue(void) {
}

// ------------------------------------------------------
// tick
// ------------------------------------------------------
void WorkQueue::tick(int timeUnits) {
	_buffer.clear();
	processQueueCommon(timeUnits);
}

// ------------------------------------------------------
// process work queue common
// ------------------------------------------------------
void WorkQueue::processQueueCommon(int timeUnits) {
	Queue::iterator it = _queue.begin();
	while (it != _queue.end()) {
		if (!it->done) {
			it->timer += timeUnits;
		}
		if (it->timer >= it->duration) {
			Event e;
			e.work_type = it->type;
			e.modus = 1;
			e.tile_x = it->tile_x;
			e.tile_y = it->tile_y;
			e.building_id = it->building_id;
			e.level = it->level;
			_buffer.add(e);
			it = _queue.erase(it);
		}
		else {
			++it;
		}
	}
}

// ------------------------------------------------------
// remove work at location for specific price_type
// ------------------------------------------------------
void WorkQueue::remove(int price_type, int x, int y) {
	Queue::iterator it = _queue.begin();
	while (it != _queue.end()) {
		if ( it->tile_x == x && it->tile_y == y && it->price_index == price_type) {
			it = _queue.erase(it);
		}
		else {
			++it;
		}
	}
}

// ------------------------------------------------------
// create work item
// ------------------------------------------------------
void WorkQueue::createWork(int price_type,int x,int y, int building_id, int level,int duration) {
	WorkItem item;
	item.tile_x = x;
	item.tile_y = y;
	if ( price_type == PT_REGULAR) {
		item.reschedule = true;
	}
	else {
		item.reschedule = false;
	}
	item.duration = duration;
	item.timer = 0;
	item.type = price_type;
	item.building_id = building_id;
	item.level = level;
	item.done = false;
	_queue.push_back(item);	
}

void WorkQueue::show() const {
	printf("Queue:\n");
	for ( size_t i = 0; i < _queue.size(); ++i ) {
		int remaining = _queue[i].duration - _queue[i].timer;
		if (remaining < 0) {
			remaining = 0;
		}
		printf(" %d %d remaining time %d\n",_queue[i].tile_x,_queue[i].tile_y,remaining);
	}
}

void WorkQueue::save(FILE* f) {
	int nr = _queue.size();
	fwrite(&nr,sizeof(int),1,f);
	printf("## size %d\n",nr);
    for ( int i = 0; i < nr; ++i ) {
		WorkItem& b = _queue[i];		
		fwrite(&b,sizeof(WorkItem),1,f);
    }
}

void WorkQueue::load(FILE* f) {
	_queue.clear();
	int nr = 0;
	fread(&nr,sizeof(int),1,f);
    for ( int i = 0; i < nr; ++i ) {
		WorkItem b;
		fread(&b,sizeof(WorkItem),1,f);
		_queue.push_back(b);
    }
}