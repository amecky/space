#pragma once

namespace bits {

	void set(int* value,int index);

	void clear(int* value,int index);

	bool is_set(int value,int index);
}

namespace reg {

	//int translate_work(char c);

	//const char* translate_work(int idx);

	int translate_stage(char s);

	const char* translate_stage(int id);
}

namespace string {

	void format_duration(int seconds,char* buffer,int max);
}