#include "Parser.h"
#include <stdio.h>
#include <iostream>

namespace str {

        bool isCharacter(const char p) {
            if (p >= 'a' && p <= 'z') {
                return true;
            } else if (p >= 'A' && p <= 'Z') {
                return true;
            }
            return false;
        }

        bool isWhitespace(const char c) {
            if (c == ' ' || c == '\t' || c == '\r' || c == '\n') {
                return true;
            }
            return false;
        }

        bool isDigit(const char c) {
            return ((c >= '0' && c <= '9') || c == '-');
        }

        float strtof(const char* p, char** endPtr) {
            while (isWhitespace(*p)) {
                ++p;
            }
            float sign = 1.0f;
            if (*p == '-') {
                sign = -1.0f;
                ++p;
            }
            float value = 0.0f;
            while (isDigit(*p)) {
                value *= 10.0f;
                value = value + (*p - '0');
                ++p;
            }
            if (*p == '.') {
                ++p;
                float dec = 1.0f;
                float frac = 0.0f;
                while (isDigit(*p)) {
                    frac *= 10.0f;
                    frac = frac + (*p - '0');
                    dec *= 10.0f;
                    ++p;
                }
                value = value + (frac / dec);
            }
            if (endPtr) {
                *endPtr = (char *) (p);
            }
            return value * sign;
        }

    }

static Token TOKENS[] = {
	{"unknown" , Token::TOK_UNKNOWN, 0},
	{"quit" , Token::TOK_QUIT, 0},
	{"show" , Token::TOK_SHOW, 0},
	{"buildings" , Token::TOK_BUILDINGS, 0},
	{"status",Token::TOK_STATUS,0},
	{"map",Token::TOK_MAP,2},
	{"start",Token::TOK_START,3},
	{"upgrade",Token::TOK_UPGRADE,2},
	{"build",Token::TOK_BUILD,3},
	{"load",Token::TOK_LOAD,0},
	{"save",Token::TOK_SAVE,0},
	{"move",Token::TOK_MOVE,4},
	{"collect",Token::TOK_COLLECT,2},
	{ "step", Token::TOK_STEP, 1 },
	{ "remove", Token::TOK_REMOVE, 2 },
	{ "ldtxt", Token::TOK_LOAD_TXT, 1 },
	{ "switch", Token::TOK_SWITCH, 1 },
	{ "describe", Token::TOK_DESCRIBE, 2 }
};

const int NUM_TOKENS = 18;

namespace parse {

	CommandLine extract(const char* p) {
		CommandLine line;
		line.type = Token::TOK_UNKNOWN;
        int cnt = 0;
        int dataIndex = 0;
        while (*p != 0) {
            if (str::isCharacter(*p)) {
                const char* ident = p;
                while (str::isCharacter(*p)) {
                    ++p;
                }
				int del = p - ident;
				int idx = -1;
				for ( int i = 0; i < NUM_TOKENS; ++i ) {
					if (strncmp(TOKENS[i].name, ident, del) == 0 && strlen(TOKENS[i].name) == del ) {
						idx = i;
					}
				}
				if ( idx == -1 ) {
					printf("Error: Unknown command line\n");
					idx = 0;
				}						
				else {
					line.type = TOKENS[idx].type;
					int pc = TOKENS[idx].param_count;
					int cnt = 0;
					while ( *p != 0) {
                        ++p;
                        while (str::isWhitespace(*p)) {
                            ++p;
                        }
                        if (str::isDigit(*p)) {
                            char* out;
                            float value = str::strtof(p, &out);
                            p = out;
							line.values[cnt++] = value;
                        }
                    }

					line.count = cnt;					
					if (cnt != pc) {
						printf("Missing arguments!!\n");
						line.type = Token::TOK_UNKNOWN;
					}
                }
            }
			else {
                ++p;
            }
        }
		return line;
	}
}

