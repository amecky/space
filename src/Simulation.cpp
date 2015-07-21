#include "Simulation.h"
#include <stdio.h>
/*
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
*/
Simulation::Simulation() {
	_commands[Token::TOK_STATUS] = new SimStatus(&_world);
	_commands[Token::TOK_MAP] = new SimMap(&_world);
	Island* island = _world.createIsland();
	Island* next = _world.createIsland();
	_world.addResource(Sign('M','O'),1000);
	_world.selectIsland(0);
}


Simulation::~Simulation(void)
{
}

void Simulation::tick() {
	int timeUnits = _timer.getElapsed() * _world.getContext()->time_multiplier;
	_world.tick(timeUnits);
}

void Simulation::execute_command(Token::TokenType type,const TextLine& line) {
	if ( _commands.find(type) == _commands.end()) {
		printf("Unknown command!\n");
	}
	else {
		SimCommand* cmd = _commands[type];
		if ( cmd->num_params() == (line.num_tokens() -1) ) {
			cmd->execute(_world,line);
		}
		else {
			printf("Error: Missing arguments\n");
			printf("  ");
			cmd->write_syntax();
		}
	}
}

void Simulation::quit() {
	_world.save();
}