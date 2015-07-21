#pragma once
#include <windows.h>

class GlobalTimer {

public:
	GlobalTimer();
	~GlobalTimer();
	int getElapsed();
private:
	int _seconds;
	int _accu;
	DWORD _now;

};

