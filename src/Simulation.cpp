#include "Simulation.h"
#include <stdio.h>

// ------------------------------------------------------
// constructor
// ------------------------------------------------------
Simulation::Simulation() {
	_commands[Token::TOK_STATUS] = new SimStatus(&_world);
	_commands[Token::TOK_MAP] = new SimMap(&_world);
	_commands[Token::TOK_STEP] = new SimStep(&_world);
	_commands[Token::TOK_START] = new SimStart(&_world);
	_commands[Token::TOK_BUILDINGS] = new SimBuildings(&_world);
	_commands[Token::TOK_DESCRIBE] = new SimDescribe(&_world);
	_commands[Token::TOK_BUILD] = new SimBuild(&_world);
	_commands[Token::TOK_COLLECT] = new SimCollect(&_world);
	_commands[Token::TOK_UPGRADE] = new SimUpgrade(&_world);
	_commands[Token::TOK_REMOVE] = new SimRemove(&_world);
	_commands[Token::TOK_LOAD] = new SimLoad(&_world);
	_commands[Token::TOK_MOVE] = new SimMove(&_world);
	Island* island = _world.createIsland();
	Island* next = _world.createIsland();
	_world.addResource(Sign('M','O'),1000);
	_world.selectIsland(0);
}

// ------------------------------------------------------
// destructor
// ------------------------------------------------------
Simulation::~Simulation() {
	Commands::iterator it = _commands.begin();
	while (it != _commands.end()) {
		delete it->second;
		it = _commands.erase(it);
	}
}

// ------------------------------------------------------
// tick
// ------------------------------------------------------
void Simulation::tick() {
	int timeUnits = _timer.getElapsed() * _world.getContext()->time_multiplier;
	_world.tick(timeUnits);
}

// ------------------------------------------------------
// execute_command
// ------------------------------------------------------
void Simulation::execute_command(Token::TokenType type,const TextLine& line) {
	if ( _commands.find(type) == _commands.end()) {
		printf("Unknown command!\n");
	}
	else {
		SimCommand* cmd = _commands[type];
		if ( cmd->num_params() == (line.num_tokens() -1) ) {
			cmd->execute(line);
		}
		else {
			printf("Error: Missing arguments\n");
			printf("  \n  ");
			cmd->write_syntax();
			printf("\n");
		}
	}
}

// ------------------------------------------------------
// quit
// ------------------------------------------------------
void Simulation::quit() {
	_world.save();
}