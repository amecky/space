#pragma once
#include "Work.h"

// ------------------------------------------------------
// UpgradeWork
// ------------------------------------------------------
class UpgradeWork : public SimWork {

public:
	UpgradeWork() : SimWork() {}
	virtual ~UpgradeWork() {}
	bool start(Island* island, const TextLine& line);
	void finish(Island* island, const Event& e);
	const int getWorkType() const;
};

