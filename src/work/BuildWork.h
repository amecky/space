#pragma once
#include "Work.h"

// ------------------------------------------------------
// BuildWork
// ------------------------------------------------------
class BuildWork : public SimWork {

public:
	BuildWork() : SimWork() {}
	virtual ~BuildWork() {}
	bool start(Island* island, const TextLine& line);
	void finish(Island* island, const Event& e);
	const int getWorkType() const;
};


