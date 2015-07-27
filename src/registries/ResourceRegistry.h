#pragma once
#include "BaseRegistry.h"
#include "..\Common.h"
#include "RegistryReader.h"
// ------------------------------------------------------
// resource definition
// ------------------------------------------------------
struct ResourceDefinition {

	int id;
	Sign sign;
	char name[32];
	bool global;
};

// ------------------------------------------------------
// resource registry
// ------------------------------------------------------
class ResourceRegistry : public BaseRegistry<ResourceDefinition> {

public:
	ResourceRegistry() {}
	~ResourceRegistry() {}
	const char* getName(int id) const;
	const int getIndex(const Sign& c) const;
	const bool isGlobal(int id) const;
	const char** get_field_names() const;
	int get_field_num() const;
	bool load_entry(const RegistryReader& reader,int index,ResourceDefinition* def);
private:
	const int getIndex(int id) const;
};


