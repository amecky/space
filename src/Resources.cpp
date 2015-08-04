#include "Resources.h"
#include "registries\ResourceRegistry.h"

namespace res {

	// ------------------------------------------------------
	// show resources
	// ------------------------------------------------------
	void show_resources(const ResourceRegistry& reg,const Resources& res,bool complete) {
		//printf("Resources:\n");
		for ( int i = 0; i <reg.size(); ++i ) {
			if ( complete) {
				printf("%10s : %d\n",reg.getName(i),res.get(i));
			}
			else {
				if ( res.get(i) > 0 ) {
					printf("%10s : %d\n",reg.getName(i),res.get(i));
				}
			}
		}	
	}

	// ------------------------------------------------------
	// show resources
	// ------------------------------------------------------
	void log_resources(const ResourceRegistry& reg,const Resources& res,bool complete) {
		LOG << "Resources:";
		char buffer[128];
		for ( int i = 0; i <reg.size(); ++i ) {
			if ( complete) {
				sprintf(buffer,"%10s : %d",reg.getName(i),res.get(i));
				LOG << buffer;
			}
			else {
				if ( res.get(i) > 0 ) {
					sprintf(buffer,"%10s : %d",reg.getName(i),res.get(i));
					LOG << buffer;
				}
			}
		}	
	}

}