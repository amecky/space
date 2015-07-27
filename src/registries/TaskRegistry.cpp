#include "TaskRegistry.h"

// ------------------------------------------------------
// Task registry
// ------------------------------------------------------

const char* TR_FIELD_NAMES[] = {"id","island","type","resource","building","level","amount","previous","text"};

const char** TaskRegistry::get_field_names() const {		
	return TR_FIELD_NAMES;
}

int TaskRegistry::get_field_num() const {
	return 9;
}
// ------------------------------------------------------
// load
// ------------------------------------------------------
// id : 1 , island : 0 , type : B , resource : -- , building : HT , level : 1 , amount : 3 , previous : -1 , text : "Build 2 Huts"
// id: 2, island : 0, type : C, resource : MO, building : HT, level : 1, amount : 2, previous : 2, text : "Collect tax from 2 Huts"
bool TaskRegistry::load_entry(const RegistryReader& reader,int index,Task* t) {
	bool valid = true;
	t->island = reader.get_int(index,"island");
	t->id = reader.get_int(index,"id");
	char type = reader.get_char(index,"type");
	Sign bs = reader.get_sign(index,"building");
	t->building_id = _building_registry->getIndex(bs);
	if (t->building_id == -1) {
		printf("ERROR: invalid building type at %d\n", reader.get_line_nr(index));
		valid = false;
	}
	Sign rs = reader.get_sign(index, "resource");
	t->level = reader.get_int(index,"level");
	t->amount = reader.get_int(index,"amout");	
	t->previous = reader.get_int(index, "previous");
	reader.get_string(index,"text",t->text);
	return valid;
}

bool TaskRegistry::validate_data() {
	add(-1);
	return true;
}

void TaskRegistry::get_active_tasks(int island, TaskList& tasks) {
	for (size_t i = 0; i < _active_tasks.size(); ++i) {
		if (_active_tasks[i].island == island || _active_tasks[i].island == -1) {
			tasks.push_back(_active_tasks[i]);
		}
	}
}

int TaskRegistry::handle_event(int island,const Event& event) {
	return 0;
}

void TaskRegistry::add(int previous) {
	for (size_t i = 0; i < _items.size(); ++i) {
		if (_items[i].previous == previous) {
			_active_tasks.push_back(_items[i]);
		}
	}
}
