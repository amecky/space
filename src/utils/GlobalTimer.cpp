#include "GlobalTimer.h"
#include "Log.h"


GlobalTimer::GlobalTimer() {
	_now = GetTickCount();
	_accu = 0;
}


GlobalTimer::~GlobalTimer() {
}

DWORD GlobalTimer::getRecentTime() {
	return _now;
}

int GlobalTimer::tick(DWORD last_time) {
	if ( last_time != 0 ) {
		_now = last_time;
		_accu = 0;
	}
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
