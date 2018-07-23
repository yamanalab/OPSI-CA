#include "dataowner.h"


DataOwner::DataOwner(long nThreads):
	nThreads(nThreads)
{
	
	toCloud=new DataOwnerSocket(10000);
	toCloud->connect();	

}
DataOwner::~DataOwner(){

	toCloud->disconnect();
	delete toCloud;

}

void DataOwner::runServer(){

	//run server
	//receive fpmax and publickey from querier

	Server server(10001);

	while(DataOwner::isRunning){
		std::string header="";
		std::string data="";
		std::string packet="";
		int fd=0;
		int type=-1;
		server.receive(fd,type,packet,header,data);
		if(type!=-1){
		
			if(type==PUBKEYSTORE_MSG){
				LOG("PUBKEY STORE");
				std::cout<<"storing public key...."<<std::flush;
				int echeck=storePubkey(header,data);
				if(echeck!=-1)
					sendAccept(fd);
				else
					sendReject(fd);
				std::cout<<"done"<<std::endl;
			}else if(type==FPMAX_MSG){
				LOG("FPMAX STORE");
				std::cout<<"storing false positive rate...."<<std::flush;
				try{
					fpmax=std::stod(data);
					sendAccept(fd);
					isRunning=false;
				}catch (const std::invalid_argument& e) {
				
					sendReject(fd);	
				}
				std::cout<<"done"<<std::endl;
			}else if(type==EXIT){
				
			}else{
				//reject the request
				LOG("request type error:"+to_string(type));
				sendReject(fd);
			}
		
	
		}
		

	}
}
void DataOwner::getNmaxFromCloud(const std::string filename){

	//get value of nmax from cloud server
	ssize_t size;
	std::string data;
	size=toCloud->downloadNmax(filename,data);
	if(size<0){
		LOG("failed to download the nmax");
		return;
	}
	nmax=std::stol(data);
}

int  DataOwner::storePubkey(const std::string& filename,const std::string& data){
                
       
 	pubkeyFilename=filename;       
        LOG("Store publickey: file name="+filename+",data size="+to_string(data.size()));
        std::fstream receiveFile(filename,std::fstream::out|std::fstream::trunc);
       	assert(receiveFile.is_open());
        const char* str=data.c_str();
        LOG("Writing on file....."+to_string(strlen(str)));
        receiveFile.write(str,strlen(str));
        LOG("Writing done");
        receiveFile.close();
	
 	return 1;
}

void DataOwner::encryptDataset(const std::string& filename){

	
	createBFs(filename);
	encryptBFs(filename);	

}
void DataOwner::createBFs(const std::string& inputFName){
	
	std::vector<std::string> data;
	BloomParams bloomparams(nmax,fpmax);
	LOG("nmax="+to_string(nmax)+", fpmax= "+to_string(fpmax));
	fstream inputFile(inputFName,fstream::in);
        int size;
       	std::string ssize;
	std::getline(inputFile,ssize);
        size=std::stoi(ssize);
	std::string buffer;
        for(int i=0;i<size;i++){
        	std::getline(inputFile,buffer);
                data.push_back(buffer);
         }
         inputFile.close();
         num_elements=size;
	
	BloomFilter bf(bloomparams);
	BloomFilter bfs[num_elements];
	for(int i=0;i<num_elements;i++){
		bfs[i]=bf;
	}
	
	for(int i=0;i<num_elements;i++){
		
		bfs[i].add((uint8_t*)data[i].c_str(),data[i].size());
	//	bfs[i].print();
	}
	
	
	if(!bloomfilters.empty())
		bloomfilters.clear();
	for(int i=0;i<num_elements;i++){
		
		bloomfilters.push_back(bfs[i].bloomfilter);
	}
	
	

}

void DataOwner::encryptBFs(const std::string& uploadFDir){
	
 	fstream pubkeyFile(pubkeyFilename,fstream::in);
        unsigned long m,p,r;
       	vector<long> gens,orgs;
	
        readContextBase(pubkeyFile,m,p,r,gens,orgs);
        FHEcontext context(m,p,r,gens,orgs);
        pubkeyFile >> context;
        FHEPubKey publicKey(context);
        pubkeyFile >> publicKey;
        pubkeyFile.close();
        EncryptedArray ea(context);
        long nslots = ea.size();
      

	BasicThreadPool multiTask(nThreads);
	multiTask.exec_range(num_elements,[&](long first,long last){	
		for(long i=first;i<last;i++){
			std::string re_uploadFDir=uploadFDir;
			re_uploadFDir.insert(re_uploadFDir.size()-4,"-"+to_string(i+1));
			fstream uploadFile(re_uploadFDir,fstream::out|fstream::trunc);
			assert(uploadFile.is_open());
	    
	   		//Output Encrypted bloom filters
		uploadFile << num_elements << std::endl;

		EnBloomFilter dummy;
		int npacks=dummy.calculateNPacks(bloomfilters[0].size(),nslots);
	    	uploadFile << npacks << std::endl;

	    		
		EnBloomFilter enBF(publicKey,nslots,&ea,bloomfilters[i]);

	    	std::vector<Ctxt> enbloomfilter=enBF.enbloomfilter;
	    	for(int n=0;n<npacks;n++){
	    		 uploadFile << enbloomfilter[n] << endl;
	    	}
			uploadFile.close();  
	    }
	});
    LOG("encrypted file creating done");


}
void  DataOwner::uploadToCloud(const std::string& filename, const std::string& uploadFDir){
	
	auto start=std::chrono::system_clock::now();

	ssize_t size=0;

	for(int i=0;i<num_elements;i++){
		std::string re_filename=filename;
        	std::string re_uploadFDir=uploadFDir;
		re_filename.insert(re_filename.size()-4,"-"+to_string(i+1));
		re_uploadFDir.insert(re_uploadFDir.size()-4,"-"+to_string(i+1));
		ssize_t si=toCloud->sendData(re_filename,re_uploadFDir);
		if(si<0){
			LOG("failed to upload data to cloud");
			return;
		}
		size+=si;
	}

	auto end=std::chrono::system_clock::now();

	double time=(double)std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count()/1000;


	LOG("[DATA OWNER] uploading: duration ="+std::to_string(time)+ " sec.");
	LOG("[DATA OWNER] uploaded data = "+std::to_string((double)size/1000000)+" MB");



}
