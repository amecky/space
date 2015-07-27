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

}