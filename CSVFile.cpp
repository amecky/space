#include "CSVFile.h"
#include <fstream>
#include <assert.h>

// ------------------------------------------------------
// CSVLine
// ------------------------------------------------------
CSVLine::CSVLine(const std::string& str) {
	_content = str;
	_num_delimiters = 0;
	for ( int i = 0; i < _content.length();++i ) {
		if ( _content[i] == ',' ) {
			++_num_delimiters;
		}
	}
}

// ------------------------------------------------------
// find pos in string for field index
// ------------------------------------------------------
const int CSVLine::find_pos(int field_index) const {
	if ( field_index == 0 ) {
		return 0;
	}
	int cnt = 0;
	for ( int i = 0; i < _content.length();++i ) {
		if ( _content[i] == ',' ) {
			++cnt;
		}
		if ( cnt == field_index ) {
			return i + 1;
		}
	}
	return -1;
}

// ------------------------------------------------------
// get int
// ------------------------------------------------------
const int CSVLine::get_int(int index) const {
	int idx = find_pos(index);
	if ( idx != -1 ) {
		int v;
		int npos = find_pos(index+1);
		if ( npos != -1 ) {
			std::string str = _content.substr(idx,npos-idx-1);		
			sscanf(str.c_str(),"%d",&v);
		}
		else {
			std::string str = _content.substr(idx);		
			sscanf(str.c_str(),"%d",&v);
		}
		return v;
	}
	return -1;
}

// ------------------------------------------------------
// get string
// ------------------------------------------------------
void CSVLine::get_string(int index,char* dest) const {
	int idx = find_pos(index);
	if ( idx != -1 ) {
		int npos = find_pos(index+1);
		if ( npos != -1 ) {			
			std::string str = _content.substr(idx,npos-idx-1);		
			strcpy(dest,str.c_str());
		}
		else {
			std::string str = _content.substr(idx);		
			strcpy(dest,str.c_str());
		}
	}
}

// ------------------------------------------------------
// get char
// ------------------------------------------------------
const char CSVLine::get_char(int index) const {
	int idx = find_pos(index);
	if ( idx != -1 ) {
		return _content[idx];
	}
	return '-';
}

// ------------------------------------------------------
// get bool
// ------------------------------------------------------
const bool CSVLine::get_bool(int index) const {
	int idx = find_pos(index);
	if ( idx != -1 ) {
		char c = _content[idx];
		if ( c == 'N' || c == 'n' ) {
			return false;
		}
		return true;		
	}
	return false;
}

// ------------------------------------------------------
// get sign
// ------------------------------------------------------
const Sign CSVLine::get_sign(int index) const {
	int idx = find_pos(index);
	Sign v;
	if ( idx != -1 ) {		
		int npos = find_pos(index+1);
		if ( npos != -1 ) {
			std::string str = _content.substr(idx,npos-idx-1);		
			v = Sign(str[0],str[1]);
		}
		else {
			std::string str = _content.substr(idx);		
			v = Sign(str[0],str[1]);
		}
	}
	return v;
}

// ------------------------------------------------------
// CSVFile
// ------------------------------------------------------
CSVFile::CSVFile() {}


CSVFile::~CSVFile() {}

// ------------------------------------------------------
// load
// ------------------------------------------------------
bool CSVFile::load(const char* fileName,const char* directory) {
	char buffer[256];
		sprintf(buffer,"%s\\%s",directory,fileName);
		std::string line;
		std::ifstream myfile(buffer);
		_lines.clear();
		if (myfile.is_open()) {
			while (std::getline(myfile, line)) {
				if (line.find("#") == std::string::npos && line.find(",") != std::string::npos) {
					_lines.push_back(CSVLine(line));					
				}
			}
			myfile.close();
			return true;
		}
		return false;
}

// ------------------------------------------------------
// get line
// ------------------------------------------------------
const CSVLine& CSVFile::get(int index) const {
	assert(index >= 0 && index < _lines.size());
	return _lines[index];
}

// ------------------------------------------------------
// size
// ------------------------------------------------------
const size_t CSVFile::size() const {
	return _lines.size();
}