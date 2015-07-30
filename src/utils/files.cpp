#include "files.h"
#include <fstream>

namespace file {

	// ------------------------------------------------------
	// read file 
	// ------------------------------------------------------
	bool read(const char* name,const char* dir,std::vector<std::string>& content) {
		char buffer[256];
		sprintf(buffer,"%s\\%s",dir,name);
		std::string line;
		std::ifstream myfile(buffer);
		content.clear();
		if (myfile.is_open()) {
			while (std::getline(myfile, line)) {
				if (line.find("#") == std::string::npos) {
					content.push_back(line);					
				}
			}
			myfile.close();
			return true;
		}
		return false;
	}
}