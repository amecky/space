// Space.cpp : Defines the entry point for the console application.
//

//#include "stdafx.h"
#include "GlobalTimer.h"
#include <iostream>
#include "World.h"
#include "Parser.h"
#include <map>
#include "Simulation.h"

int main(int argc,char* argv[]) {
	Simulation sim;
	char buffer[256];
	bool running = true;	
	while ( running ) {
		sim.tick();
		std::cout << "Turn: ";
		std::cin.getline(buffer,256);
		CommandLine line = parse::extract(buffer);		
		if ( line.type == Token::TOK_QUIT ) {
			sim.quit();
			running = false;
		}
		else {
			sim.execute_command(line.type,line.line);
		}
	}
	return 0;
}

