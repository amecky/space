#pragma once
#include "Work.h"

// ------------------------------------------------------
// BuildWork
// ------------------------------------------------------
class BuildWork : public SimWork {

public:
	BuildWork(WorldContext* context) : SimWork(context) {}
	virtual ~BuildWork() {}
	bool start(MyIsland* island, const TextLine& line);
	void finish(MyIsland* island, const Event& e);
	const int getWorkType() const;
};


