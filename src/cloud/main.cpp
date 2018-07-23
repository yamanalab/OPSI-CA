#include <vector>
#include <string>
#include <sstream>
#include "cloud.h"
#include "logger.h"


#define LOGFILE "log/log.txt"


int main(int argc, char **argv){

	long  nmax=100;//-n
	long nthreads=50;//-th
	
			
	for(int i=0;i<argc-1;i++){
		if(strcmp(argv[i],"-n")==0){
			nmax=std::stol(argv[i+1]);
		}else if(strcmp(argv[i],"-th")==0){
			nthreads=std::stol(argv[i+1]);
		}
	}

	Logger::Initialize(LOGFILE);

	LOG("arguments: nmax="+std::to_string(nmax)+" nthread="+std::to_string(nthreads));
	std::cout<<"Initialized parameters..."<<std::endl;
	std::cout<<"maximum data size Nmax:"<<nmax<<std::endl;
	std::cout<<"# of threads:"<<nthreads<<std::endl;

	//store nmax to file
	std::fstream nmaxFile("nmax.txt", std::fstream::out|std::fstream::trunc);
	assert(nmaxFile.is_open());
	nmaxFile<<std::to_string(nmax);
	nmaxFile.close();

	Cloud cloud(nthreads,nmax);
	cloud.start();

	return 0;
}
