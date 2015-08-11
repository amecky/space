#pragma once
#include "Work.h"

// ------------------------------------------------------
// DeleteWork
// ------------------------------------------------------
class DeleteWork : public SimWork {

public:
	DeleteWork() : SimWork() {}
	virtual ~DeleteWork() {}
	bool start(Island* island, const TextLine& line);
	void finish(Island* island, const Event& e);
	const int getWorkType() const;
};


