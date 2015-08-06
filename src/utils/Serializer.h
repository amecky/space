#pragma once
#include <stdio.h>

enum BinaryMode {
	BM_READ,
	BM_WRITE
};

class Serializer {

public:
	Serializer();
	~Serializer();
	bool open(const char* fileName, const char* dir,BinaryMode mode);
	void write(int v);
	void write(void* buffer, int size);
	bool read(int* v);
	bool read(void* buffer, int size);
private:
	FILE* _stream;
};

