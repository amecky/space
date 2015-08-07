#pragma once
#include "Work.h"

// ------------------------------------------------------
// UpgradeWork
// ------------------------------------------------------
class UpgradeWork : public SimWork {

public:
	UpgradeWork() : SimWork() {}
	virtual ~UpgradeWork() {}
	bool start(MyIsland* island, const TextLine& line);
	void finish(MyIsland* island, const Event& e);
	const int getWorkType() const;
};

