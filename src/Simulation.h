#pragma once
#include "utils\GlobalTimer.h"
#include <iostream>
#include "World.h"
#include "ui\Parser.h"
#include "ui\vis.h"
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
	virtual CommandType get_token_type() const = 0;
	virtual const char* get_command() const = 0;
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
		_world->getSelectedIsland()->printStatus();
	}
	void write_syntax() {
		printf("status - prints the current status\n");
	}
	int num_params() {
		return 0;
	}
	CommandType get_token_type() const {
		return TOK_STATUS;
	}
	const char* get_command() const {
		return "status";
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
		vis::print_map(_world->getSelectedIsland(), x, y, 8);
	}
	void write_syntax() {
		printf("map [x] [y] - prints a part of the map centered at x,y and 16x16 size\n");
	}
	CommandType get_token_type() const {
		return TOK_MAP;
	}
	const char* get_command() const {
		return "map";
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
		_world->execute(PT_WORK, line);
	}
	void write_syntax() {
		printf("start [x] [y] [level] - start work with level at the building at x,y\n");
	}
	CommandType get_token_type() const {
		return TOK_START;
	}
	const char* get_command() const {
		return "start";
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
		gContext->time_multiplier = x;
	}
	void write_syntax() {
		printf("step [mul] - sets the time multiplier to mul\n");
	}
	CommandType get_token_type() const {
		return TOK_STEP;
	}
	const char* get_command() const {
		return "step";
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
		vis::print_building_definitions();
	}
	void write_syntax() {
		printf("buildings - lists all available buildings\n");
	}
	CommandType get_token_type() const {
		return TOK_BUILDINGS;
	}
	const char* get_command() const {
		return "buildings";
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
		vis::describe_building(_world->getSelectedIsland(), x, y);
	}
	void write_syntax() {
		printf("describe [x] [y] - shows a detailed description about the building at x,y\n");
	}
	CommandType get_token_type() const {
		return TOK_DESCRIBE;
	}
	const char* get_command() const {
		return "describe";
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
		_world->execute(PT_BUILD, line);
	}
	void write_syntax() {
		printf("build [x] [y] [sign] - builds a new building defined by sign at x,y\n");
	}
	CommandType get_token_type() const {
		return TOK_BUILD;
	}
	const char* get_command() const {
		return "build";
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
		_world->getSelectedIsland()->collect(x,y);
	}
	void write_syntax() {
		printf("collect [x] [y] - collects the resources at x,y\n");
	}
	CommandType get_token_type() const {
		return TOK_COLLECT;
	}
	const char* get_command() const {
		return "collect";
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
		_world->execute(PT_UPGRADE,line);
	}
	void write_syntax() {
		printf("upgrade [x] [y] - upgrades the building at x,y to the next level\n");
	}
	CommandType get_token_type() const {
		return TOK_UPGRADE;
	}
	const char* get_command() const {
		return "upgrade";
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
		_world->execute(PT_DELETE, line);
	}
	void write_syntax() {
		printf("remove [x] [y] - removes the building at x,y\n");
	}
	CommandType get_token_type() const {
		return TOK_REMOVE;
	}
	const char* get_command() const {
		return "remove";
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
		printf("load - loads the last saved MyWorld\n");
	}
	CommandType get_token_type() const {
		return TOK_LOAD;
	}
	const char* get_command() const {
		return "load";
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
	CommandType get_token_type() const {
		return TOK_MOVE;
	}
	const char* get_command() const {
		return "move";
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
		vis::print_tasks(_world->getSelectedIsland());
	}
	void write_syntax() {
		printf("tasks - lists all active tasks\n");
	}
	CommandType get_token_type() const {
		return TOK_TASKS;
	}
	const char* get_command() const {
		return "tasks";
	}
	int num_params() {
		return 0;
	}
};

// ------------------------------------------------------
// tasks
// ------------------------------------------------------
class SimQuit : public SimCommand {

public:
	SimQuit(World* w) : SimCommand(w) {}
	void execute(const TextLine& line) {
		// nothing to do here
	}
	void write_syntax() {
		printf("quit - quits the game\n");
	}
	CommandType get_token_type() const {
		return TOK_QUIT;
	}
	const char* get_command() const {
		return "quit";
	}
	int num_params() {
		return 0;
	}
};
/*
else if (line.type == TOK_LOAD_TXT) {
island->load_txt(line.values[0]);
}
else if (line.type == TOK_SWITCH) {
island = MyWorld.getIsland(line.values[0]);
}
*/

// ------------------------------------------------------
// Simulation
// ------------------------------------------------------
class Simulation {

typedef std::map<CommandType, SimCommand*> Commands;

public:
	Simulation();
	~Simulation();
	void intialize();
	void tick();	
	void execute_command(CommandType type,const TextLine& line);
	void quit();
	template<class T>
	void add() {
		SimCommand* cmd = new T(_world);
		add_command(cmd);
	}
	void add_command(SimCommand* cmd);
	bool extract(const char* p,CommandLine * command_line);
	void setCollectMode(CollectMode cm);
private:
	void createArea(Island* island, const AreaDefinition& definition);
	void add(Island* island, int x, int y, const Sign& s);
	World* _world;
	Commands _commands;
	GlobalTimer _timer;
};

