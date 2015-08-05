#include "WorkQueue.h"
#include "..\Resources.h"
#include "..\registries\PriceRegistry.h"
#include "..\utils\utils.h"
#include "..\utils\Serializer.h"

WorkQueue::WorkQueue(void) {
}


WorkQueue::~WorkQueue(void) {
}

// ------------------------------------------------------
// tick
// ------------------------------------------------------
void WorkQueue::tick(int timeUnits) {
	_buffer.clear();
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
	LOGC("WorkQueue") << "removing " << reg::translate_work(price_type) << " at " << x << " " << y;
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
	LOGC("WorkQueue") << "create work " << reg::translate_work(price_type) << " at " << x << " " << y << " for building " << building_id << "/" << level << " duration: " << duration;
	WorkItem item;
	item.tile_x = x;
	item.tile_y = y;
	if ( price_type == PT_REGULAR) {
		item.reschedule = true;
	}
	else {
		item.reschedule = false;
	}
	item.price_index = price_type;
	item.duration = duration;
	item.timer = 0;
	item.type = price_type;
	item.building_id = building_id;
	item.level = level;
	item.done = false;
	_queue.push_back(item);	
}

void WorkQueue::show() const {
	printf("Queue: (%d)\n",_queue.size());
	char time[20];
	for ( size_t i = 0; i < _queue.size(); ++i ) {
		int remaining = _queue[i].duration - _queue[i].timer;
		if (remaining < 0) {
			remaining = 0;
		}
		string::format_duration(remaining,time,20);
		printf(" %s at %d %d remaining time %s\n",reg::translate_work(_queue[i].price_index),_queue[i].tile_x,_queue[i].tile_y,time);		
	}
}

void WorkQueue::save(Serializer& writer) {
	int nr = _queue.size();
	writer.write(nr);
	LOGC("WorkQueue") << "saving " << nr << " work items";
    for ( int i = 0; i < nr; ++i ) {
		WorkItem& b = _queue[i];		
		writer.write(&b,sizeof(WorkItem));
    }
}

void WorkQueue::load(Serializer& reader) {
	_queue.clear();
	int nr = 0;
	reader.read(&nr);
	LOGC("WorkQueue") << "loading " << nr << " work items";
    for ( int i = 0; i < nr; ++i ) {
		WorkItem b;
		reader.read(&b,sizeof(WorkItem));
		_queue.push_back(b);
    }
}