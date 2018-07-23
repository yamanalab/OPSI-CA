
#include <vector>
#include <string>
#include <sstream>
#include "dataowner.h"
#include <thread>
#define LOGFILE "log/log.txt"
void execute(DataOwner* dataowner, const std::string filename){

	LOG("Filename:"+filename);

	auto total_start=std::chrono::system_clock::now(); 

	//create and encrypt bloom filters
	std::cout<<"creating encrypted bloom filters......."<<std::flush;
	auto start1=std::chrono::system_clock::now();
	dataowner->createBFs("./datasets/"+filename);
	dataowner->encryptBFs("./encrypteddatasets/"+filename); 
	auto end1=std::chrono::system_clock::now();
	double time1= (double)std::chrono::duration_cast<std::chrono::milliseconds>(end1-start1).count()/1000;
	LOG("[Data Owner] encrypting datasets: duration ="+std::to_string(time1)+" sec.");
	std::cout<<time1<<" sec."<<std::endl;	

	//upload encrypted bloom filters
	std::cout<<"uploading..........."<<std::flush;
	auto start2=std::chrono::system_clock::now();
	dataowner->uploadToCloud(filename,"./encrypteddatasets/"+filename);
	auto end2=std::chrono::system_clock::now();
	double time2=(double)std::chrono::duration_cast<std::chrono::milliseconds>(end2-start2).count()/1000;
	LOG("[Data owner] uploading datasets: duration ="+std::to_string(time2)+" sec."); 
	std::cout<<time2<<" sec."<<std::endl;

	double total_time=(double)std::chrono::duration_cast<std::chrono::milliseconds>(end2-total_start).count()/1000;

	LOG("[Data owner] total preprocessing time:duration ="+std::to_string(total_time)+ " sec.");  
	std::cout<<"**total pre-processing time:"<<total_time<<" sec."<<std::endl;
}

int main(int argc, char **argv){

	SetNumThreads(1);	
	
	long nThreads=10;
	
	for(int i=0;i<argc-1;i++){
		if(strcmp(argv[i],"-th")==0){
			nThreads=std::stol(argv[i+1]);
		}
	}

	Logger::Initialize(LOGFILE);
	
	LOG("arguments: nthreads="+std::to_string(nThreads));
	std::cout<<"Initialized parameter..."<<std::endl;
	std::cout<<"# of threads:"<<nThreads<<std::endl;

	DataOwner dataowner(nThreads);
	dataowner.isRunning=true;
	std::thread th(&DataOwner::runServer,&dataowner);	
	dataowner.getNmaxFromCloud("nmax.txt");

	th.join();	
	//Demo
	std::cout<<std::endl<<"Demo:"<<std::endl;
	std::cout<<"Press enter to upload datasetA50_1.txt";
	getchar();	
	execute(&dataowner,"datasetA50_1.txt");

	/*
	while(1){
		std::cout<<"Enter a file name you want to upload:";
		std::string filename;
		if(!std::getline(std::cin,filename))
			break;
		execute(&dataowner,filename);	
	}
	*/

	
	return 0;
}
