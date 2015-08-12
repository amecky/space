#pragma once
#include "BaseRegistry.h"
#include "BuildingRegistry.h"
#include "PriceRegistry.h"
#include "..\queues\WorkQueue.h"
// ------------------------------------------------------
// Task registry
// ------------------------------------------------------
struct Task {

	int id;
	int island;
	WorkType work_type;
	int resource_id;
	int building_id;
	int level;
	int amount;
	char text[256];
	int previous;
};

typedef std::vector<Task> TaskList;

class TaskRegistry : public BaseRegistry<Task> {

public:	
	TaskRegistry(PriceRegistry* prc_reg,BuildingRegistry* bld_reg) : _price_registry(prc_reg) , _building_registry(bld_reg) {}
	~TaskRegistry() {}
	const char** get_field_names() const;
	int get_field_num() const;
	bool load_entry(const RegistryReader& reader,int index,Task* t);	
	const Task& get(int id) const;
	Task& get(int id);
	void get_tasks(int island,int previous,TaskList& list);
private:
	PriceRegistry* _price_registry;
	BuildingRegistry* _building_registry;
};
