#pragma once
#include "..\utils\CSVFile.h"

enum CommandType {
	TOK_UNKNOWN,
	TOK_QUIT,
	TOK_SHOW,
	TOK_STATUS,
	TOK_MAP,
	TOK_START,
	TOK_UPGRADE,
	TOK_BUILD,
	TOK_LOAD,
	TOK_SAVE,
	TOK_MOVE,
	TOK_BUILDINGS,
	TOK_COLLECT,
	TOK_STEP,
	TOK_REMOVE,
	TOK_LOAD_TXT,
	TOK_SWITCH,
	TOK_DESCRIBE,
	TOK_TASKS,
	TOK_ITEM_COUNT
};


struct CommandLine {

	CommandType type;
	TextLine line;

};


