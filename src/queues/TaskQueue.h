#pragma once
#include "..\registries\TaskRegistry.h"

struct Event;
class BinaryWriter;

struct ActiveTask {

	Task*  task;
	int count;

};

typedef std::vector<ActiveTask> ActiveTasks;

class TaskQueue {

public:
	TaskQueue(TaskRegistry* tsk_reg);
	~TaskQueue();
	int handle_event(int island,const Event& event,int* ids,int max);
	void get_active_tasks(int island,ActiveTasks& tasks);
	void init(int numIslands);
	void load(BinaryWriter& reader);
	void save(BinaryWriter& writer);
private:
	bool matches(Task* task,const Event& event);
	TaskRegistry* _task_registry;
	ActiveTasks _tasks;
};

