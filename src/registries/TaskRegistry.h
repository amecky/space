#pragma once
#include "BaseRegistry.h"
#include "BuildingRegistry.h"
#include "..\WorkQueue.h"
// ------------------------------------------------------
// Task registry
// ------------------------------------------------------
// island : 0 , id : 2 , type : B , resource : -- , building : HT , level : 1 , amount : 3 , previous : -1 , text : "Build 2 Huts"

struct Task {

	int id;
	int island;
	int price_type;
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
	TaskRegistry(BuildingRegistry* bld_reg) : _building_registry(bld_reg) {}
	~TaskRegistry() {}
	const char** get_field_names() const;
	int get_field_num() const;
	bool load_entry(const RegistryReader& reader,int index,Task* t);	
	void get_active_tasks(int island,TaskList& tasks);
	int handle_event(int island,const Event& event);
	virtual bool validate_data();
private:
	void add(int previous);
	BuildingRegistry* _building_registry;
	TaskList _active_tasks;
};
