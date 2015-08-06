#include "Serializer.h"


Serializer::Serializer() : _stream(0) {
}


Serializer::~Serializer() {
	if (_stream != 0) {
		fclose(_stream);
	}
}

bool Serializer::open(const char* fileName, const char* dir, BinaryMode mode) {
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

void Serializer::write(int v) {
	if (_stream != 0) {
		fwrite(&v, sizeof(int), 1, _stream);
	}
}

void Serializer::write(void* buffer,int size) {
	if (_stream != 0) {
		fwrite(buffer, size, 1, _stream);
	}
}

bool Serializer::read(int* v) {
	if (_stream != 0) {
		fread(v, sizeof(int), 1, _stream);
		return true;
	}
	return false;
}

bool Serializer::read(void* buffer, int size) {
	if (_stream != 0) {
		fread(buffer, size, 1, _stream);
		return true;
	}
	return false;
}