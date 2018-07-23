#ifndef LOGGER_H
#define LOGGER_H

#include <time.h> /*time_t*/
#include <stdio.h> /*localtime strftime*/
#include <stdlib.h> /*exit*/
#include <string>
#define LOG(message) {Logger::Write(message);}

class Logger{
public:
	static void Initialize(const std::string& filename);
	static void Write(const std::string&log);

protected:
	static std::string filename;
	static FILE* file;

};

#endif
