#pragma once
#include "GlobalTimer.h"
#include <iostream>
#include "World.h"
#include "Parser.h"
#include <map>

class SimCommand {

public:
	SimCommand(World* w) : _world(w) {}
	virtual ~SimCommand() {}
	virtual void execute(const World& w,const TextLine& line) = 0;
	virtual void write_syntax() = 0;
	virtual int num_params() = 0;
protected:
	World* _world;
};

class SimStatus : public SimCommand {

public:
	SimStatus(World* w) : SimCommand(w) {}
	void execute(const World& w,const TextLine& line) {
		w.getSelectedIsland()->status();
	}
	void write_syntax() {
		printf("status - prints the current status\n");
	}
	int num_params() {
		return 0;
	}
};

class SimMap : public SimCommand {

public:
	SimMap(World* w) : SimCommand(w) {}
	void execute(const World& w,const TextLine& line) {
		int x = line.get_int(1);
		int y = line.get_int(2);
		w.getSelectedIsland()->showMap(x,y);
	}
	void write_syntax() {
		printf("map [x] [y] - prints a part of the map centered at x,y and 16x16 size\n");
	}
	int num_params() {
		return 2;
	}
};

typedef std::map<Token::TokenType,SimCommand*> Commands;

class Simulation {

public:
	Simulation();
	~Simulation();
	void tick();	
	void execute_command(Token::TokenType type,const TextLine& line);
	void quit();
private:
	World _world;
	Commands _commands;
	GlobalTimer _timer;
};

