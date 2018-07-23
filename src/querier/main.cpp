#include <vector>
#include <string>
#include <sstream>
#include "querier.h"
#include "logger.h"


#define LOGFILE "log/log.txt"


void execute(Querier *querier, const std::string datasetsAFName, const std::string datasetsBFName, const std::string resultFName){

	std::string resultFDir="./result/"+resultFName;
	std::cout<<"requesting to cloud server....."<<std::endl;
	auto total_start=std::chrono::system_clock::now();
	querier->requestPSICAToServer(datasetsAFName,datasetsBFName,resultFDir);

	//download results
	std::cout<<"downloading the result...."<<std::flush;
	auto start1=std::chrono::system_clock::now();
	querier->downloadResult(resultFDir);
	auto end1=std::chrono::system_clock::now();
	double time1=(double)std::chrono::duration_cast<std::chrono::milliseconds>(end1-start1).count()/1000;
	LOG("[Querier] downloading PSICA: duration ="+std::to_string(time1)+" sec.");
	std::cout<<time1<<" sec."<<std::endl;

	//decrypt results
	std::cout<<"decrypting the result...."<<std::flush;
	auto start2=std::chrono::system_clock::now();
	querier->decryptResult(resultFDir);
	auto end2=std::chrono::system_clock::now();
	double time2=(double)std::chrono::duration_cast<std::chrono::milliseconds>(end2-start2).count()/1000;
	LOG("[Querier] decrypting PSICA: duration ="+std::to_string(time2)+" sec.");
	std::cout<<time2<<" sec."<<std::endl;

	double total_time=(double)std::chrono::duration_cast<std::chrono::milliseconds>(end2-total_start).count()/1000;	
	LOG("[Querier] total time:duration ="+std::to_string(total_time)+ " sec.");

	LOG("RESULT OPSI-CA="+std::to_string(querier->getResult()));	
	std::cout<<"**The result of OPSI-CA: "<<querier->getResult()<<"**"<<std::endl;;
	std::cout<<"**Total time:"<<total_time<<"**"<<std::endl;
}

int main(int argc, char **argv){
	
	
	double fpmax=0.001;//-p
	int fheM=30269;//-m
	int algorithmType=QUERIER_FRIENDLY_PROTOCOL;//-t
	//int algorithmType=BASIC_PROTOCOL;	
	
	for(int i=0;i<argc-1;i++){
		if(strcmp(argv[i],"-p")==0){
			fpmax=std::stod(argv[i+1]);		
		}else if(strcmp(argv[i],"-m")==0){
			fheM=std::stoi(argv[i+1]);
		}else if(strcmp(argv[i],"-t")==0){
			int t=std::stoi(argv[i+1]);
			if(t==BASIC_PROTOCOL || t==QUERIER_FRIENDLY_PROTOCOL)
				algorithmType=t;
		}

	}



	Logger::Initialize(LOGFILE);

	LOG(" fpmax="+std::to_string(fpmax)+" fheM="+std::to_string(fheM)+" algorithmType="+std::to_string(algorithmType));	
	std::cout<<"Initialized parameters..."<<std::endl;
	std::cout<<"false positive rate fp:"<<fpmax<<std::endl;
	std::cout<<"ring modulus m:"<<fheM<<std::endl;
	std::cout<<"protocol: "<<algorithmType<<std::endl;
	
	std::fstream fpmaxFile("fpmax.txt",std::fstream::out|std::fstream::trunc);
        assert(fpmaxFile.is_open());
        fpmaxFile<<std::to_string(fpmax);
        fpmaxFile.close();


	Querier querier(fpmax,fheM,algorithmType);
	//setup
	std::cout<<"set up......"<<std::flush;
	querier.getNmax("nmax.txt");
	querier.generateFHEParams("./pubkey/pubkey.txt","./seckey/seckey.txt");
	querier.sendParamsToServer();
	querier.sendParamsToDataowner(A);
	querier.sendParamsToDataowner(B);
	std::cout<<"done"<<std::endl;


	//demo
	std::cout<<std::endl<<"Demo:"<<std::endl;
	std::cout<<"Press enter to compute OPS-CA operation toward datasetA50_1.txt and datasetB50_1.txt";
	getchar();
	execute(&querier,"datasetA50_1.txt","datasetB50_1.txt","result.txt");

	/*
	int run_i=0;
	std::string line;
	while (true){
    	run_i++;
    	std::cout<<"Run"<<run_i<<":";
    	if(!std::getline(std::cin, line))
    		break;
    	//line=datasetA100_1 datasetsB100_1 result-A100_1-B100_1"
    	std::vector<std::string> filenames;
    	filenames=split(line,' ');
    	if(filenames.size()!=3)
    		break;
    	execute(&querier,filenames[0],filenames[1],filenames[2]);
	}
	*/	

	return 0;
}
