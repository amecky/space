#pragma once
#include <string>
#include <vector>
#include "Common.h"

class TextLine {

public:
	TextLine() {}
	TextLine(const std::string& str);
	~TextLine() {}
	void set(const std::string& str,const char delimiter = ',');
	const int find_pos(int field_index) const;
	const int get_int(int index) const;
	const char get_char(int index) const;
	const Sign get_sign(int index) const;
	const bool get_bool(int index) const;
	int get_string(int index,char* dest) const;
	const int num_tokens() const;
private:
	int _num_delimiters;
	std::string _content;
	char _delimiter;
};

class CSVFile {

typedef std::vector<TextLine> Lines;

public:
	CSVFile();
	~CSVFile();
	bool load(const char* fileName,const char* directory);
	const TextLine& get(int index) const;
	const size_t size() const;
private:
	Lines _lines;
};

