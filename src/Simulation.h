#pragma once
#include "GlobalTimer.h"
#include <iostream>
#include "World.h"
#include "Parser.h"
#include <map>

// ------------------------------------------------------
// virtual base command
// ------------------------------------------------------
class SimCommand {

public:
	SimCommand(World* w) : _world(w) {}
	virtual ~SimCommand() {}
	virtual void execute(const TextLine& line) = 0;
	virtual void write_syntax() = 0;
	virtual int num_params() = 0;
protected:
	World* _world;
};

// ------------------------------------------------------
// status
// ------------------------------------------------------
class SimStatus : public SimCommand {

public:
	SimStatus(World* w) : SimCommand(w) {}
	void execute(const TextLine& line) {
		_world->getSelectedIsland()->status();
	}
	void write_syntax() {
		printf("status - prints the current status\n");
	}
	int num_params() {
		return 0;
	}
};

// ------------------------------------------------------
// map
// ------------------------------------------------------
class SimMap : public SimCommand {

public:
	SimMap(World* w) : SimCommand(w) {}
	void execute(const TextLine& line) {
		int x = line.get_int(1);
		int y = line.get_int(2);
		_world->getSelectedIsland()->showMap(x, y);
	}
	void write_syntax() {
		printf("map [x] [y] - prints a part of the map centered at x,y and 16x16 size\n");
	}
	int num_params() {
		return 2;
	}
};

// ------------------------------------------------------
// start
// ------------------------------------------------------
class SimStart : public SimCommand {

public:
	SimStart(World* w) : SimCommand(w) {}
	void execute(const TextLine& line) {
		int x = line.get_int(1);
		int y = line.get_int(2);
		int level = line.get_int(3);
		_world->getSelectedIsland()->start(x, y, level);
	}
	void write_syntax() {
		printf("start [x] [y] [level] - start work with level at the building at x,y\n");
	}
	int num_params() {
		return 3;
	}
};

// ------------------------------------------------------
// step
// ------------------------------------------------------
class SimStep : public SimCommand {

public:
	SimStep(World* w) : SimCommand(w) {}
	void execute(const TextLine& line) {
		int x = line.get_int(1);
		_world->getContext()->time_multiplier = x;
	}
	void write_syntax() {
		printf("step [mul] - sets the time multiplier to mul\n");
	}
	int num_params() {
		return 1;
	}
};

// ------------------------------------------------------
// buildings
// ------------------------------------------------------
class SimBuildings : public SimCommand {

public:
	SimBuildings(World* w) : SimCommand(w) {}
	void execute(const TextLine& line) {
		_world->getSelectedIsland()->showBuildingDefinitions();
	}
	void write_syntax() {
		printf("buildings - lists all available buildings\n");
	}
	int num_params() {
		return 0;
	}
};

// ------------------------------------------------------
// describe
// ------------------------------------------------------
class SimDescribe : public SimCommand {

public:
	SimDescribe(World* w) : SimCommand(w) {}
	void execute(const TextLine& line) {
		int x = line.get_int(1);
		int y = line.get_int(2);
		_world->getSelectedIsland()->describe(x, y);
	}
	void write_syntax() {
		printf("describe [x] [y] - shows a detailed description about the building at x,y\n");
	}
	int num_params() {
		return 2;
	}
};

// ------------------------------------------------------
// build
// ------------------------------------------------------
class SimBuild : public SimCommand {

public:
	SimBuild(World* w) : SimCommand(w) {}
	void execute(const TextLine& line) {
		int x = line.get_int(1);
		int y = line.get_int(2);
		Sign s = line.get_sign(3);
		int building_id = _world->getContext()->building_definitions.getIndex(s);
		_world->getSelectedIsland()->build(x, y, building_id);
	}
	void write_syntax() {
		printf("build [x] [y] [sign] - builds a new building defined by sign at x,y\n");
	}
	int num_params() {
		return 3;
	}
};

// ------------------------------------------------------
// collect
// ------------------------------------------------------
class SimCollect : public SimCommand {

public:
	SimCollect(World* w) : SimCommand(w) {}
	void execute(const TextLine& line) {
		int x = line.get_int(1);
		int y = line.get_int(2);
		_world->getSelectedIsland()->collect(x, y);
	}
	void write_syntax() {
		printf("collect [x] [y] - collects the resources at x,y\n");
	}
	int num_params() {
		return 2;
	}
};

// ------------------------------------------------------
// upgrade
// ------------------------------------------------------
class SimUpgrade : public SimCommand {

public:
	SimUpgrade(World* w) : SimCommand(w) {}
	void execute(const TextLine& line) {
		int x = line.get_int(1);
		int y = line.get_int(2);
		_world->getSelectedIsland()->upgrade(x, y);
	}
	void write_syntax() {
		printf("upgrade [x] [y] - upgrades the building at x,y to the next level\n");
	}
	int num_params() {
		return 2;
	}
};

// ------------------------------------------------------
// remove
// ------------------------------------------------------
class SimRemove : public SimCommand {

public:
	SimRemove(World* w) : SimCommand(w) {}
	void execute(const TextLine& line) {
		int x = line.get_int(1);
		int y = line.get_int(2);
		_world->getSelectedIsland()->remove(x, y);
	}
	void write_syntax() {
		printf("remove [x] [y] - removes the building at x,y\n");
	}
	int num_params() {
		return 2;
	}
};

// ------------------------------------------------------
// load
// ------------------------------------------------------
class SimLoad : public SimCommand {

public:
	SimLoad(World* w) : SimCommand(w) {}
	void execute(const TextLine& line) {
		_world->load();
	}
	void write_syntax() {
		printf("load - loads the last saved world\n");
	}
	int num_params() {
		return 0;
	}
};

// ------------------------------------------------------
// move
// ------------------------------------------------------
class SimMove : public SimCommand {

public:
	SimMove(World* w) : SimCommand(w) {}
	void execute(const TextLine& line) {
		_world->getSelectedIsland()->move(line.get_int(1), line.get_int(2), line.get_int(3), line.get_int(4));
	}
	void write_syntax() {
		printf("move [old_x] [old_y] [new_x] [new_y] - moves a building to the new location\n");
	}
	int num_params() {
		return 4;
	}
};

// ------------------------------------------------------
// tasks
// ------------------------------------------------------
class SimTasks : public SimCommand {

public:
	SimTasks(World* w) : SimCommand(w) {}
	void execute(const TextLine& line) {
		_world->show_tasks();
	}
	void write_syntax() {
		printf("tasks - lists all active tasks\n");
	}
	int num_params() {
		return 0;
	}
};
/*
else if (line.type == Token::TOK_LOAD_TXT) {
island->load_txt(line.values[0]);
}
else if (line.type == Token::TOK_SWITCH) {
island = world.getIsland(line.values[0]);
}
*/

// ------------------------------------------------------
// Simulation
// ------------------------------------------------------
class Simulation {

typedef std::map<Token::TokenType, SimCommand*> Commands;

public:
	Simulation();
	~Simulation();
	void intialize();
	void tick();	
	void execute_command(Token::TokenType type,const TextLine& line);
	void quit();
private:
	World _world;
	Commands _commands;
	GlobalTimer _timer;
};

