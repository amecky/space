#include "RewardsRegistry.h"
// ------------------------------------------------------
// Reward registry
// ------------------------------------------------------

const char* RW_FIELD_NAMES[] = {"task_id","resource","amount"};

const char** RewardRegistry::get_field_names() const {
	return RW_FIELD_NAMES;
}

int RewardRegistry::get_field_num() const {
	return 3;
}
// ------------------------------------------------------
// load
// ------------------------------------------------------
bool RewardRegistry::load_entry(const RegistryReader& reader, int index, Reward* t) {
	bool valid = true;
	t->task_id = reader.get_int(index, "task_id");
	Sign bs = reader.get_sign(index,"resource");
	t->amount = reader.get_int(index, "amount");
	t->resource_id = _resource_registry->getIndex(bs);
	if (t->resource_id == -1) {
		LOGE << "ERROR: invalid resource type at " << reader.get_line_nr(index);
		printf("ERROR: invalid resource type at %d\n", reader.get_line_nr(index));
		valid = false;
	}
	return valid;
}

// ------------------------------------------------------
// contains
// ------------------------------------------------------
const bool RewardRegistry::contains(int task_id) const {
	LOG << "RewardRegistry contains - task_id: " << task_id;
	for (size_t i = 0; i < _items.size(); ++i) {
		if (_items[i].task_id == task_id) {
			return true;
		}
	}
	return false;
}

// ------------------------------------------------------
// get all rewards for the specified task
// ------------------------------------------------------
int RewardRegistry::get(int task_id, Reward* rewards, int max) {
	int cnt = 0;
	for (size_t i = 0; i < _items.size(); ++i) {
		if (_items[i].task_id == task_id && cnt < max) {
			rewards[cnt++] = _items[i];
		}
	}
	LOG << "RewardRegistry get - task_id: " << task_id << " found: " << cnt;
	return cnt;
}
