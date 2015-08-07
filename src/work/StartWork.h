#pragma once
#include "Work.h"

// ------------------------------------------------------
// StartWork
// ------------------------------------------------------
class StartWork : public SimWork {

public:
	StartWork() : SimWork() {}
	virtual ~StartWork() {}
	bool start(MyIsland* island, const TextLine& line);
	void finish(MyIsland* island, const Event& e);
	const int getWorkType() const;
};

