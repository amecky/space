#include "ui\Parser.h"
#include "Simulation.h"
#include "utils\utils.h"

int main(int argc,char* argv[]) {
	Simulation sim;
	sim.intialize();
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
	}
	return 0;
}

