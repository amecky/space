#include "ui\Parser.h"
#include "Simulation.h"
#include "utils\utils.h"

int main(int argc,char* argv[]) {
	Simulation sim;
	sim.intialize();
	sim.setCollectMode(CM_MANUAL);
	char buffer[256];
	bool running = true;	
	CommandLine line;
	while ( running ) {
		sim.tick();
		std::cout << ":> ";
		std::cin.getline(buffer,256);
		//CommandLine line = parse::extract(buffer);		
		if ( sim.extract(buffer,&line)) {
			if ( line.type == TOK_QUIT ) {
				sim.quit();
				running = false;
			}
			else {
				sim.execute_command(line.type,line.line);
			}
		}
		if ( gContext->messages.contains_errors()) {
			printf("Errors:\n");
			for ( size_t i = 0; i < gContext->messages.num_errors(); ++i ) {
				printf("  %s\n",gContext->messages.get_error(i).c_str());
			}
		}
	}
	return 0;
}

