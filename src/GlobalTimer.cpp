#include "GlobalTimer.h"
#include <stdio.h>


GlobalTimer::GlobalTimer() {
	_now = GetTickCount();
	_accu = 0;
}


GlobalTimer::~GlobalTimer() {
}

int GlobalTimer::getElapsed() {
	DWORD current = GetTickCount();
	int diff = current - _now;
	// milliseconds
	//diff += _accu;
	int tu = (diff + _accu) / 1000;
	_accu = diff - tu * 1000;
	_now = current;
	//printf("diff %d accu %d tu %d current %d\n",diff,_accu,tu,current);
	return tu;
}
