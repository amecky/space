#pragma once
#include "Work.h"

// ------------------------------------------------------
// StartWork
// ------------------------------------------------------
class StartWork : public SimWork {

public:
	StartWork() : SimWork() {}
	virtual ~StartWork() {}
	bool start(Island* island, const TextLine& line);
	void finish(Island* island, const Event& e);
	const int getWorkType() const;
};

