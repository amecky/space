#pragma once
#include "BaseRegistry.h"
#include "ResourceRegistry.h"
// ------------------------------------------------------
// Rewards registry
// ------------------------------------------------------
// task_id : 1 , resource : MO , amount : 100

struct Reward {

	int task_id;
	int resource_id;
	int amount;
};



class RewardRegistry : public BaseRegistry<Reward> {

public:
	RewardRegistry(ResourceRegistry* res_reg) : _resource_registry(res_reg) {}
	~RewardRegistry() {}
	const char** get_field_names() const;
	int get_field_num() const;
	bool load_entry(const RegistryReader& reader, int index, Reward* t);
	const bool contains(int task_id) const;
	int get(int task_id, Reward* rewards, int max);
private:
	ResourceRegistry* _resource_registry;
};
