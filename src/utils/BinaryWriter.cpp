#include "BinaryWriter.h"


BinaryWriter::BinaryWriter() : _stream(0) {
}


BinaryWriter::~BinaryWriter() {
	if (_stream != 0) {
		fclose(_stream);
	}
}

bool BinaryWriter::open(const char* fileName, const char* dir, BinaryMode mode) {
	char buffer[256];
	sprintf(buffer, "%s/%s", dir, fileName);
	if (mode == BM_WRITE) {
		_stream = fopen(buffer, "wb");
	}
	else {
		_stream = fopen(buffer, "rb");
	}
	if (_stream != 0) {
		return true;
	}
	return false;
}

void BinaryWriter::write(DWORD d) {
	if (_stream != 0) {
		fwrite(&d, sizeof(DWORD), 1, _stream);
	}
}

void BinaryWriter::write(int v) {
	if (_stream != 0) {
		fwrite(&v, sizeof(int), 1, _stream);
	}
}

void BinaryWriter::write(void* buffer,int size) {
	if (_stream != 0) {
		fwrite(buffer, size, 1, _stream);
	}
}

bool BinaryWriter::read(int* v) {
	if (_stream != 0) {
		fread(v, sizeof(int), 1, _stream);
		return true;
	}
	return false;
}

bool BinaryWriter::read(DWORD* v) {
	if (_stream != 0) {
		fread(v, sizeof(DWORD), 1, _stream);
		return true;
	}
	return false;
}

bool BinaryWriter::read(void* buffer, int size) {
	if (_stream != 0) {
		fread(buffer, size, 1, _stream);
		return true;
	}
	return false;
}