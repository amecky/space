#pragma once
#include "Work.h"

//class SimWork;
//struct WorldContext;
//struct MyIsland;
//class TextLine;
//struct Event;
// ------------------------------------------------------
// DeleteWork
// ------------------------------------------------------
class DeleteWork : public SimWork {

public:
	DeleteWork(WorldContext* context) : SimWork(context) {}
	virtual ~DeleteWork() {}
	bool start(MyIsland* island, const TextLine& line);
	void finish(MyIsland* island, const Event& e);
	const int getWorkType() const;
};


