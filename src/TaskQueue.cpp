#include "TaskQueue.h"
#include "World.h"

TaskQueue::TaskQueue(TaskRegistry* tsk_reg) : _task_registry(tsk_reg) {
}

TaskQueue::~TaskQueue() {
}

int TaskQueue::find_match_task(const Event& e) {
	for (size_t i = 0; i < _tasks.size(); ++i) {
		ActiveTask& at = _tasks[i];
		if ( at.task->price_type == e.work_type ) {
			

			// check if it matches
		}
	}
	return 0;
}

int TaskQueue::handle_event(int island,const Event& event) {
	for (size_t i = 0; i < _tasks.size(); ++i) {
		ActiveTask& at = _tasks[i];
		if ( at.task->island == island || at.task->island == -1 ) {
			if ( at.task->price_type == event.work_type ) {
				printf("matching worktype\n");

				// check if it matches
			}
		}
	}
	return 0;
}

void TaskQueue::init(int numIslands) {
	TaskList list;
	_task_registry->get_tasks(-1,-1,list);
	for ( size_t i = 0; i < list.size(); ++i ) {
		ActiveTask at;
		at.task = &_task_registry->get(list[i].id);
		at.count = 0;
		_tasks.push_back(at);
	}
	for ( int j = 0; j < numIslands; ++j ) {
		_task_registry->get_tasks(j,-1,list);
		for ( size_t i = 0; i < list.size(); ++i ) {
			ActiveTask at;
			at.task = &_task_registry->get(list[i].id);
			at.count = 0;
			_tasks.push_back(at);
		}
	}
}

void TaskQueue::get_active_tasks(int island, ActiveTasks& tasks) {
	for (size_t i = 0; i < _tasks.size(); ++i) {
		if (_tasks[i].task->island == island || _tasks[i].task->island == -1) {
			tasks.push_back(_tasks[i]);
		}
	}
}
