#pragma once
#include <string>
#include "..\utils\CSVFile.h"

struct Token {

	enum TokenType {
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
		TOK_TASKS
	};

	const char* name;
	TokenType type;
	int param_count;
};

struct CommandLine {

	Token::TokenType type;
	int values[8];
	int count;

	CommandLine() : type(Token::TOK_UNKNOWN) , count(0) {
		for (int i = 0; i < 8; ++i) {
			values[i] = -1;
		}
	}

	TextLine line;
};

namespace parse {

	CommandLine extract(const char* cmd);
}

