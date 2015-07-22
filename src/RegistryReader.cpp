#include "RegistryReader.h"
#include <fstream>
#include <assert.h>

// ------------------------------------------------------
// RegistryReader
// ------------------------------------------------------
RegistryReader::RegistryReader(const char** field_names,int num) : _field_names(field_names) , _num_fields(num) {
}


RegistryReader::~RegistryReader() {
}

// ------------------------------------------------------
// find index by name
// ------------------------------------------------------
int RegistryReader::find_index(const char* name) const {
	for ( int i = 0; i < _num_fields; ++i ) {
		if ( strcmp(_field_names[i],name) == 0) {
			return i;
		}
	}
	return -1;
}

// ------------------------------------------------------
// find end
// ------------------------------------------------------
int RegistryReader::find_end(const std::string& str,int start) const {
	bool running = true;
	int current = start;
	while ( running ) {
		++current;
		if ( current >= str.length()) {
			running = false;
		}
		if ( str[current] != ' ' ) {
			running = false;
		}									
	}
	return current;
}

// ------------------------------------------------------
// get int
// ------------------------------------------------------
int RegistryReader::get_int(int line_nr,const char* name) const {
	assert ( line_nr >= 0 && line_nr < _lines.size());
	int idx = find_index(name);
	if ( idx != -1 ) {
		const Entry& e = _lines[line_nr];
		int p = e.positions[idx];
		int end = find_end(e.line,p);
		std::string str = e.line.substr(p,end);		
		int v;
		sscanf(str.c_str(),"%d",&v);
		return v;
	}
	return -1;
}

// ------------------------------------------------------
// get char
// ------------------------------------------------------
char RegistryReader::get_char(int line_nr,const char* name) const {
	assert ( line_nr >= 0 && line_nr < _lines.size());
	int idx = find_index(name);
	if ( idx != -1 ) {
		const Entry& e = _lines[line_nr];
		int p = e.positions[idx];
		return e.line[p];
	}
	return '-';
}

// ------------------------------------------------------
// get bool
// ------------------------------------------------------
bool RegistryReader::get_bool(int line_nr,const char* name) const {
	assert ( line_nr >= 0 && line_nr < _lines.size());
	int idx = find_index(name);
	if ( idx != -1 ) {
		const Entry& e = _lines[line_nr];
		int p = e.positions[idx];
		char c = e.line[p];
		if ( c == 'Y' || c == 'y' ) {
			return true;
		}
		return false;
	}
	return false;
}

// ------------------------------------------------------
// get sign
// ------------------------------------------------------
Sign RegistryReader::get_sign(int line_nr,const char* name) const {
	assert ( line_nr >= 0 && line_nr < _lines.size());
	int idx = find_index(name);
	Sign s('X','X');
	if ( idx != -1 ) {
		const Entry& e = _lines[line_nr];
		int p = e.positions[idx];
		char c = e.line[p];
		char n = e.line[p+1];
		s = Sign(c,n);
	}
	return s;
}

// ------------------------------------------------------
// get string
// ------------------------------------------------------
int RegistryReader::get_string(int line_nr,const char* name,char* dest) const {
	int idx = find_index(name);
	if ( idx != -1 ) {
		const Entry& e = _lines[line_nr];
		int p = e.positions[idx];
		if ( e.line[p] == '"') {
			++p;
		}
		int end = e.line.find("\"",p);
		if ( end == 0 ) {
			std::string str = e.line.substr(p);		
			strcpy(dest,str.c_str());
			return str.length();
		}
		else {
			std::string str = e.line.substr(p,end - p);		
			strcpy(dest,str.c_str());
			return str.length();
		}

	}		
	return 0;
}

// ------------------------------------------------------
// load
// ------------------------------------------------------
bool RegistryReader::load(const char* fileName,const char* directory) {
	char buffer[256];
		sprintf(buffer,"%s\\%s",directory,fileName);
		std::string line;
		std::ifstream myfile(buffer);
		int cnt = 0;
		if (myfile.is_open()) {
			while (std::getline(myfile, line)) {
				++cnt;
				if (line.find("#") == std::string::npos && line.find(",") != std::string::npos) {
					Entry e;
					e.line = line;
					e.count = _num_fields;
					for ( int i = 0; i < _num_fields; ++i ) {
						if ( line.find(_field_names[i]) != std::string::npos) {
							size_t idx =  line.find(_field_names[i]);							
							int nidx = line.find(":",idx);
							if ( nidx != std::string::npos) {	
								nidx = find_end(line,nidx);								
							}
							e.set(i,nidx);
						}
					}
					_lines.push_back(e);
				}
			}
			myfile.close();
			return true;
		}
		return false;
}