#pragma once
#include "..\registries\TaskRegistry.h"
#include "..\registries\PriceRegistry.h"

struct Event;
class Serializer;

struct ActiveTask {

	Task*  task;
	int count;

};

typedef std::vector<ActiveTask> ActiveTasks;

class TaskQueue {

public:
	TaskQueue(PriceRegistry* prc_reg,TaskRegistry* tsk_reg);
	~TaskQueue();
	int handle_event(int island,const Event& event,int* ids,int max);
	void get_active_tasks(int island,ActiveTasks& tasks);
	void init(int numIslands);
	void load(Serializer& reader);
	void save(Serializer& writer);
private:
	bool matches(Task* task,const Event& event);
	TaskRegistry* _task_registry;
	PriceRegistry* _price_registry;
	ActiveTasks _tasks;
};

