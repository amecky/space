#include "Simulation.h"
#include <stdio.h>
#include "registries\RegistryReader.h"
#include <map>
// ------------------------------------------------------
// constructor
// ------------------------------------------------------
Simulation::Simulation() {
	add<SimStatus>();
	add<SimMap>();
	add<SimStep>();
	add<SimStart>();
	add<SimBuildings>();
	add<SimDescribe>();
	add<SimBuild>();
	add<SimCollect>();
	add<SimUpgrade>();
	add<SimRemove>();
	add<SimLoad>();
	add<SimMove>();
	add<SimTasks>();
	add<SimQuit>();
	_world.addResource(Sign('M','O'),1000);
	_world.selectIsland(0);
}

void Simulation::add_command(SimCommand* cmd) {
	_commands[cmd->get_token_type()] = cmd;
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

void Simulation::intialize() {
	IslandRegistry registry;
	registry.load("islands.txt");
	std::map<int,std::vector<AreaDefinition>> islands;
	// group by island
	for ( size_t i = 0; i < registry.size(); ++i ) {
		const AreaDefinition& def = registry.get(i);
		islands[def.island].push_back(def);
	}
	std::map<int,std::vector<AreaDefinition>>::iterator it = islands.begin();
	while ( it != islands.end()) {
		int id = it->first;
		std::vector<AreaDefinition> defs = it->second;
		int sx = 0;
		int sy = 0;
		for ( size_t i = 0; i < defs.size(); ++i ) {
			if ( defs[i].start_x + defs[i].size_x > sx ) {
				sx = defs[i].start_x + defs[i].size_x;
			}
			if ( defs[i].start_y + defs[i].size_y > sy ) {
				sy = defs[i].start_y + defs[i].size_y;
			}
		}		
		LOGC("Simulation") << "island " << id << " size " << sx << " " << sy;
		Island* il = _world.createIsland(sx,sy);
		for ( size_t i = 0; i < defs.size(); ++i ) {
			const AreaDefinition& ad = defs[i];
			il->createArea(ad);
		}	
		++it;
	}
	LOGC("Simulation") << "islands: " << islands.size();
	_world.selectIsland(0);
	const char* names[] = {"island","resource","amount"};
	RegistryReader reader(names,3);
	if ( reader.load("island_resources.txt","data")) {
		for ( size_t i = 0; i < reader.size(); ++i ) {
			int is_idx = reader.get_int(i,"island");
			Sign s = reader.get_sign(i,"resource");
			int amount = reader.get_int(i,"amount");
			Island* il = _world.getIsland(is_idx);
			il->addResource(s,amount);
		}
	}
	_world.getContext()->task_queue.init(islands.size());
}

// ------------------------------------------------------
// tick
// ------------------------------------------------------
void Simulation::tick() {
	int timeUnits = _timer.tick() * _world.getContext()->time_multiplier;
	_world.tick(timeUnits);
}

// ------------------------------------------------------
// execute_command
// ------------------------------------------------------
void Simulation::execute_command(CommandType type,const TextLine& line) {
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
	_world.save(_timer.getRecentTime());
}

// ------------------------------------------------------
// build command line from text input
// ------------------------------------------------------
bool Simulation::extract(const char* p,CommandLine * command_line) {	
		command_line->line.set(p,' ');
		command_line->type = TOK_UNKNOWN;
		if ( command_line->line.num_tokens() > 0 ) {
			char buffer[256];
			int idx = -1;
			int len = command_line->line.get_string(0,buffer);
			Commands::iterator it = _commands.begin();
			int cnt = 0;
			while ( it != _commands.end() ) {
				if (strncmp(it->second->get_command(), buffer, len) == 0 && strlen(it->second->get_command()) == len ) {
					command_line->type = it->second->get_token_type();
					return true;
				}
				++cnt;
				++it;
			}			
		}		
		return false;
	}