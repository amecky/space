#pragma once
#include "registries\TaskRegistry.h"

struct Event;
struct Task;

struct ActiveTask {

	Task*  task;
	int count;

};

typedef std::vector<ActiveTask> ActiveTasks;

class TaskQueue {

public:
	TaskQueue(TaskRegistry* tsk_reg);
	~TaskQueue();
	int handle_event(int island,const Event& event);
	void get_active_tasks(int island,ActiveTasks& tasks);
	void init(int numIslands);
	void load();
	void save();
private:
	int find_match_task(const Event& e);
	TaskRegistry* _task_registry;
	ActiveTasks _tasks;
};

