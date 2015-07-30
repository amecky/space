#include "TaskQueue.h"
#include "..\World.h"
#include "..\utils\Log.h"

TaskQueue::TaskQueue(TaskRegistry* tsk_reg) : _task_registry(tsk_reg) {
}

TaskQueue::~TaskQueue() {
}

// ------------------------------------------------------
// matches
// ------------------------------------------------------
bool TaskQueue::matches(Task* task,const Event& event) {
	if ( task->price_type == event.work_type ) {
		LOGC("TaskQueue")  << "matching worktype";
		if ( event.work_type == PT_WORK ) {
		}
		else {
			if ( task->building_id == event.building_id ) {
				LOGC("TaskQueue")  << "matching building";
				if ( task->level == event.level ) {
					LOGC("TaskQueue")  << "matching level";
					return true;
				}					
			}
		}
	}
	return false;
}

// ------------------------------------------------------
// handle event
// ------------------------------------------------------
int TaskQueue::handle_event(int island,const Event& event,int* ids,int max) {
	int cnt = 0;
	LOGC("TaskQueue") << "=> incoming event - island: " << island << " and " << reg::translate_work(event.work_type);
	ActiveTasks::iterator it = _tasks.begin();
	while ( it != _tasks.end()) {
		if ( matches(it->task,event) ) {
			++it->count;
			if ( it->count >= it->task->amount ) {
				ids[cnt++] = it->task->id;
				LOGC("TaskQueue")  << "task finished - id : " << ids[cnt-1];
				it = _tasks.erase(it);
			}
			else {
				++it;
			}
		}
		else {
			++it;
		}
	}
	// get next tasks
	TaskList list;
	for ( int j = 0; j < cnt; ++j ) {
		list.clear();
		_task_registry->get_tasks(island,ids[j],list);
		LOGC("TaskQueue") << "found " << list.size() << " for previous task " << ids[j];
		for ( size_t i = 0; i < list.size(); ++i ) {
			ActiveTask at;
			at.task = &_task_registry->get(list[i].id);
			at.count = 0;
			LOGC("TaskQueue")  << "adding task: " << list[i].id;
			_tasks.push_back(at);
		}
	}
	return cnt;
}

// ------------------------------------------------------
// init
// ------------------------------------------------------
void TaskQueue::init(int numIslands) {
	LOGC("TaskQueue")  << "-- init --";
	TaskList list;
	_task_registry->get_tasks(-1,-1,list);
	LOGC("TaskQueue")  << "Global tasks: " << list.size();
	for ( size_t i = 0; i < list.size(); ++i ) {
		ActiveTask at;
		at.task = &_task_registry->get(list[i].id);
		at.count = 0;
		_tasks.push_back(at);
	}
	for ( int j = 0; j < numIslands; ++j ) {
		list.clear();
		_task_registry->get_tasks(j,-1,list);
		LOGC("TaskQueue")  << "island: " << j << " tasks: " << list.size();
		for ( size_t i = 0; i < list.size(); ++i ) {
			ActiveTask at;
			at.task = &_task_registry->get(list[i].id);
			at.count = 0;
			LOGC("TaskQueue")  << "adding task: " << list[i].id;
			_tasks.push_back(at);
		}
	}
}

// ------------------------------------------------------
// get active tasks
// ------------------------------------------------------
void TaskQueue::get_active_tasks(int island, ActiveTasks& tasks) {
	for (size_t i = 0; i < _tasks.size(); ++i) {
		if (_tasks[i].task->island == island || _tasks[i].task->island == -1) {
			tasks.push_back(_tasks[i]);
		}
	}
}
