#include "logger.h"
#include <iostream>


std::string Logger::filename="";
FILE* Logger::file = NULL;

void Logger::Initialize(const std::string& filename){

	Logger::filename=filename;

	file=fopen(filename.data(), "w");
	if(file==NULL){
		std::cerr<<"failed to open log file"<<std::endl;
		exit(0);
	}

}

void Logger::Write(const std::string& log){
	char date[64];
	time_t t=time(NULL);
	strftime(date,sizeof(date),"%Y/%m/%d %a %H:%M:%S", localtime(&t));	
	fprintf(file,"[ %s ]: %s\n",date, log.c_str());

	fflush(file);

}




/*
int  main(){

        Logger::Initialize("Log.txt");
        LOG("test started, written with LOG Macro");
        return 0;
}
*/
