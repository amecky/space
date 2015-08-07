#include "Messages.h"
#include <stdarg.h>
#include "Log.h"

Messages::Messages(void) {
}


Messages::~Messages(void) {
}

void Messages::clear() {
	_errors.clear();
	_messages.clear();
}

void Messages::add(char* format,...) {
	char buffer[256];
	va_list args;
	va_start (args, format);
	vsnprintf (buffer, 255, format, args);
	_messages.push_back(buffer);
	va_end (args);
}

void Messages::report_error(char* format,...) {
	char buffer[256];
	va_list args;
	va_start (args, format);
	vsnprintf (buffer, 255, format, args);
	LOGE << buffer;
	_errors.push_back(buffer);
	va_end (args);
}

void Messages::add(const char* msg) {
	_messages.push_back(msg);
}

void Messages::report_error(const char* error) {
	LOGE << error;
	_errors.push_back(error);
}

const bool Messages::contains_errors() const {
	return !_errors.empty();
}

const bool Messages::contains_messages() const {
	return !_messages.empty();
}

const size_t Messages::num_errors() const {
	return _errors.size();
}

const size_t Messages::num_messages() const {
	return _messages.size();
}

const std::string& Messages::get_message(int index) const {
	return _messages[index];
}

const std::string& Messages::get_error(int index) const {
	return _errors[index];
}