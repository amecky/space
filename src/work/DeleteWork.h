#pragma once
#include "Work.h"

// ------------------------------------------------------
// DeleteWork
// ------------------------------------------------------
class DeleteWork : public SimWork {

public:
	DeleteWork() : SimWork() {}
	virtual ~DeleteWork() {}
	bool start(MyIsland* island, const TextLine& line);
	void finish(MyIsland* island, const Event& e);
	const int getWorkType() const;
};


