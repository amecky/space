#pragma once
#include <vector>

class Messages {

typedef std::vector<std::string> MessageList;

public:
	Messages(void);
	~Messages(void);
	void clear();
	void add(char* format,...);
	void report_error(char* format,...);
	void add(const char* msg);
	void report_error(const char* error);
	const bool contains_errors() const;
	const bool contains_messages() const;
	const size_t num_errors() const;
	const size_t num_messages() const;
	const std::string& get_message(int index) const;
	const std::string& get_error(int index) const;
private:
	MessageList _messages;
	MessageList _errors;
};

