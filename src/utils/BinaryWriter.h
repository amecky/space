#pragma once
#include <stdio.h>
#include <Windows.h>

enum BinaryMode {
	BM_READ,
	BM_WRITE
};

class BinaryWriter {

public:
	BinaryWriter();
	~BinaryWriter();
	bool open(const char* fileName, const char* dir,BinaryMode mode);
	void write(DWORD d);
	void write(int v);
	void write(void* buffer, int size);
	bool read(int* v);
	bool read(DWORD* v);
	bool read(void* buffer, int size);
private:
	FILE* _stream;
};

