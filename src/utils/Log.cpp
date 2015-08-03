#include "Log.h"
#include <windows.h>
#include <stdio.h>

void ConsoleOutputHandler::write(const std::string& message) {
	OutputDebugStringA(message.c_str());
}

FILE*& FileOutputHandler::Stream() {
    static FILE* pStream = stderr;
    return pStream;
}

void FileOutputHandler::write(const std::string& message) {
	OutputDebugStringA(message.c_str());
	FILE* pStream = Stream();
	if ( pStream ) {
		fprintf(pStream, "%s", message.c_str());
		fflush(pStream);
	}
}

Log::Log() {}

Log::Log(const Log& orig) {}

std::ostringstream& Log::get() {
    os << NowTime();
    os << " : ";
    return os;
}

std::ostringstream& Log::get(const char *file, const unsigned long line) {
	os << NowTime();
	os << " : [";
	os << file;
	os << " : ";
	os << line;
	os << "] ";
	return os;
}

std::ostringstream& Log::error(const char *file, const unsigned long line) {
	os << NowTime();
	os << " [ERROR] : ";
	os << " : [";
	os << file;
	os << " : ";
	os << line;
	os << "] ";	
	return os;
}

std::ostringstream& Log::error(const char *file, const unsigned long line,const char* message) {
	os << NowTime();
	os << " [ERROR] : ";
	os << " : [";
	os << file;
	os << " : ";
	os << line;
	os << "] ";
	os << message;
	return os;
}

std::ostringstream& Log::error(const char *file, const unsigned long line, char* format, va_list args) {
	os << NowTime();
	os << " [ERROR] : ";
	os << " : [";
	os << file;
	os << " : ";
	os << line;
	os << "] ";
	char buffer[1024];
	memset(buffer, 0, sizeof(buffer));
	int written = vsnprintf_s(buffer, sizeof(buffer), _TRUNCATE, format, args);
	os << buffer;
	return os;
}

std::ostringstream& Log::error(const char *file, const unsigned long line, char* format, ...) {
	va_list args;
	va_start(args, format);
	error(file, line, format, args);
	va_end(args);
	return os;
}

std::ostringstream& Log::error() {
	os << NowTime();
	os << " [ERROR] : ";
	return os;
}

std::ostringstream& Log::error(const std::string& category) {
	os << NowTime();	
	os << " [" << category << "] : ";
	os << "[ERROR] ";
	return os;
}

std::ostringstream& Log::get(const std::string& category) {
	os << NowTime();
	os << " [" << category << "] : ";
	return os;
}

Log::~Log() {
    os << std::endl;
	OutputDebugStringA(os.str().c_str());
	//handler().write(os.str());
}

LogOutputHandler& Log::handler() {
	static ConsoleOutputHandler coh;
	return coh;	
}

std::string Log::NowTime() {
    const int MAX_LEN = 200;
    char buffer[MAX_LEN];
    if (GetTimeFormatA(LOCALE_USER_DEFAULT, 0, 0,
            "HH':'mm':'ss", buffer, MAX_LEN) == 0)
        return "Error in NowTime()";

    char result[100] = {0};
    static DWORD first = GetTickCount();
    std::sprintf(result, "%s.%03ld", buffer, (long)(GetTickCount() - first) % 1000);
    return result;
}