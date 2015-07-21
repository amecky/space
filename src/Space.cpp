// Space.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "GlobalTimer.h"
#include <iostream>
#include "World.h"
#include "Parser.h"

int _tmain(int argc, _TCHAR* argv[]) {
	int step = 1;
	GlobalTimer timer;
	World world;	
	Island* island = world.createIsland();
	Island* next = world.createIsland();
	world.addResource(Sign('M','O'),1000);
	world.selectIsland(0);
	char buffer[256];
	bool running = true;	
	while ( running ) {
		int timeUnits = timer.getElapsed() * step;
		world.tick(timeUnits);
		std::cout << "Turn: ";
		std::cin.getline(buffer,256);
		CommandLine line = parse::extract(buffer);
		
		if ( line.type == Token::TOK_QUIT ) {
			world.save();
			running = false;
		}
		else if ( line.type == Token::TOK_STATUS) {
			island->status();
		}
		else if ( line.type == Token::TOK_MAP) {
			island->showMap(line.values[0], line.values[1]);
		}
		else if ( line.type == Token::TOK_START) {
			island->start(line.values[0],line.values[1],line.values[2]);
		}
		else if ( line.type == Token::TOK_LOAD) {
			world.load();
		}
		else if ( line.type == Token::TOK_SAVE) {
			world.save();
		}
		else if (line.type == Token::TOK_STEP) {
			step = line.values[0];
		}
		else if ( line.type == Token::TOK_UPGRADE) {
			island->upgrade(line.values[0],line.values[1]);
		}
		else if ( line.type == Token::TOK_DESCRIBE) {
			island->describe(line.values[0],line.values[1]);
		}
		else if ( line.type == Token::TOK_REMOVE) {
			island->remove(line.values[0],line.values[1]);
		}
		else if (line.type == Token::TOK_COLLECT) {
			island->collect(line.values[0], line.values[1]);
		}
		else if (line.type == Token::TOK_LOAD_TXT) {
			island->load_txt(line.values[0]);
		}
		else if ( line.type == Token::TOK_BUILDINGS) {
			island->showBuildingDefinitions();
		}
		else if (line.type == Token::TOK_SWITCH) {
			island = world.getIsland(line.values[0]);
		}
		else if ( line.type == Token::TOK_BUILD) {
			island->build(line.values[0],line.values[1],line.values[2]);
		}
		
		
	}	
	return 0;
}

