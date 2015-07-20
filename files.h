#pragma once
#include <string>
#include <vector>

namespace file {

	bool read(const char* name,const char* dir,std::vector<std::string>& content);
}