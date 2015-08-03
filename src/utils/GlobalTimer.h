#pragma once
#include <windows.h>

class GlobalTimer {

public:
	GlobalTimer();
	~GlobalTimer();
	int tick(DWORD last_time = 0);
	DWORD getRecentTime();
private:
	int _seconds;
	int _accu;
	DWORD _now;

};

