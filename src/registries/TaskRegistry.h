#pragma once
#include "BaseRegistry.h"
#include "BuildingRegistry.h"
// ------------------------------------------------------
// Task registry
// ------------------------------------------------------
// island : 0 , job_id : 1 , type : B , building : HT , level : 3 , amount : 2 , text : "Build 2 Huts"
enum TaskType {
	TT_BUILD,
	TT_COLLECT,
	TT_UPGRADE
};

struct Task {

	int island;
	int job_id;
	TaskType type;
	int building_id;
	int level;
	int amount;
	char text[256];

};



class TaskRegistry : public BaseRegistry<Task> {

public:
	TaskRegistry(BuildingRegistry* bld_reg) : _building_registry(bld_reg) {}
	~TaskRegistry() {}
	const char** get_field_names() const;
	int get_field_num() const;
	bool load_entry(const RegistryReader& reader,int index,Task* t);	
private:
	BuildingRegistry* _building_registry;
};
