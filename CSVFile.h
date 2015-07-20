#pragma once
#include <string>
#include <vector>
#include "Common.h"

class CSVLine {

public:
	CSVLine(const std::string& str);
	~CSVLine() {}
	const int find_pos(int field_index) const;
	const int get_int(int index) const;
	const char get_char(int index) const;
	const Sign get_sign(int index) const;
	const bool get_bool(int index) const;
	void get_string(int index,char* dest) const;
private:
	int _num_delimiters;
	std::string _content;
};

class CSVFile {

typedef std::vector<CSVLine> Lines;

public:
	CSVFile();
	~CSVFile();
	bool load(const char* fileName,const char* directory);
	const CSVLine& get(int index) const;
	const size_t size() const;
private:
	Lines _lines;
};

