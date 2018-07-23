#include "cloud.h"


Cloud::Cloud()
	
{
	
}
Cloud::Cloud(long nThreads, long nmax):
	nThreads(nThreads), nmax(nmax)
{
	this->server=new Server(10000);
}
void Cloud::start(){
	//set up server
	int fd=0;
	int type=-1;
	

	while(true){
		std::string header="";
		std::string data="";
		std::string packet="";	
		server->receive(fd,type,packet,header,data);
	
		if(type!=-1){

			if(type==GETNMAX){
				std::cout<<"returning Nmax....."<<std::flush;
				LOG("NMAX REQUEST");
				std::string packet=createPacket(GETNMAX,header,header);
				Send(fd,packet); 
				std::cout<<"done"<<std::endl;
			}
			else if(type==PUBKEYSTORE_MSG){
				std::cout<<"storing public key......"<<std::flush;
				LOG("PUBKEY STORE");
				int echeck=storeData(header,data);
				if(echeck!=-1)
					sendAccept(fd);
				else
					sendReject(fd);
				std::cout<<"done"<<std::endl;

			}else if(type==JOINREQUEST_MSG){

				//Calculate PSICA
				std::cout<<"computing OPSICA......."<<std::endl;
				LOG("OPSI-CA REQUEST");
		
				//[inputdataA1_1.txt inputdataB1_1.txt resultA1_1-B1_1.txt encrypter.txt BLOOMIDEA1] 	
				//-> [inputdataA1_1.txt][inputdataB1_1.txt][resultA1_1-B1_1.txt][encrypter.txt][BLOOMIDEA1]
				std::vector<std::string> filenames=split(std::string(header),' ');
				
				int echeck=calculatePSICA(STOREDATAFOLDER+filenames[0],STOREDATAFOLDER+filenames[1],filenames[2], filenames[3],std::stoi(filenames[4]));
				
				if(echeck!=-1)
					sendAccept(fd);
				else
					sendReject(fd);

				std::cout<<"done"<<std::endl;
			}else if(type==STOREREQUEST_MSG){

				//store data
				LOG("STORE REQUEST");
				std::cout<<"storing data......"<<std::flush;
				int echeck=storeData(STOREDATAFOLDER+header,data);
				if(echeck!=-1)
					sendAccept(fd);
				else
					sendReject(fd);
				std::cout<<"done"<<std::endl;


			}else if(type==RESULTREQUEST_MSG){

				//Send result to client
				LOG("RESULT REQUEST");
				std::cout<<"returning result......"<<std::flush;
				std::string packet=createPacket(ENRESULT,header,header);
				Send(fd,packet);
				std::cout<<"done"<<std::endl;
			}else if(type==EXIT){

			}else{

				//reject the request because the type is not matched 
				LOG("request type error:"+to_string(type));
				sendReject(fd);

			}
			type=-1;
		}
	

	}


}
Cloud::~Cloud(){
	delete server;
}
long Cloud::storeData(const std::string& filename,const std::string& data){


	long size=data.size();

	LOG("Store data: file name="+filename+",data size="+to_string(data.size()));
	std::fstream receiveFile(filename,std::fstream::out|std::fstream::trunc);
	assert(receiveFile.is_open());
	const char* str=data.c_str();
	LOG("Writing data on file....."+to_string(strlen(str)));
	receiveFile.write(str,strlen(str));
	LOG("Writing done");
	receiveFile.close();
	
	return size;

}


int Cloud::calculatePSICA(const std::string filenameA,const std::string filenameB,const std::string resultFilename,const std::string pubkeyFilename, const int type){

	
	
	/*---------------READ PUBKEY FILE ------------------*/
	fstream pubkeyFile(pubkeyFilename,fstream::in);
	if(!pubkeyFile){
		std::cerr<<"failed to open "<<pubkeyFilename<<std::endl;
		return -1;
	}
	
    unsigned long m,p,r;
    vector<long> gens,orgs;
    long global_nslots;
	EncryptedArray* global_ea;    
    readContextBase(pubkeyFile,m,p,r,gens,orgs);
    FHEcontext context(m,p,r,gens,orgs);
    pubkeyFile >> context;  
    FHEPubKey publicKey(context);
    pubkeyFile >> publicKey;
    pubkeyFile.close();
    EncryptedArray ea(context);
    global_nslots = ea.size();
    global_ea = &ea;

     

    int num_elementsA;
    int num_elementsB;
	int npacks;

	//reading datasetA...
   
  	std::string re_filenameA1=filenameA;
    re_filenameA1.insert(re_filenameA1.size()-4,"-"+to_string(1));
    fstream receiveFileA1(re_filenameA1,fstream::in);
    receiveFileA1 >> num_elementsA;
    receiveFileA1 >> npacks;
    receiveFileA1.close();

	std::vector<EnBloomFilter> enBFsA(num_elementsA);
  
	BasicThreadPool multiTask(nThreads);
	multiTask.exec_range(num_elementsA,[&](long first, long last){
		for(int i=first;i<last;i++){
			std::string re_filenameA=filenameA;
			re_filenameA.insert(re_filenameA.size()-4,"-"+to_string(i+1));
			fstream receiveFileA(re_filenameA,fstream::in);
		
    		int ne,np;
			receiveFileA >> ne;
			receiveFileA >> np;
		
			std::vector<Ctxt> enbloomfilter;
	    	for(int n=0;n<npacks;n++){	
			Ctxt ctxt(publicKey);
	        	receiveFileA >> ctxt;
			enbloomfilter.push_back(ctxt);	
	    	}
		
			receiveFileA.close();    
			EnBloomFilter enBFA(global_nslots,global_ea,npacks,enbloomfilter);
			enBFsA[i]=enBFA;
    	}
	});	
	std::cout<<"reading B"<<std::endl;
	
	std::string re_filenameB1=filenameB;
    re_filenameB1.insert(re_filenameB1.size()-4,"-"+to_string(1));
    fstream receiveFileB1(re_filenameB1,fstream::in);
    receiveFileB1 >> num_elementsB;
    receiveFileB1 >> npacks;
    receiveFileB1.close();

	std::vector<EnBloomFilter> enBFsB(num_elementsB);
    
    multiTask.exec_range(num_elementsB,[&](long first, long last){
        for(int i=first;i<last;i++){
	        std::string re_filenameB=filenameB;
	        re_filenameB.insert(re_filenameB.size()-4,"-"+to_string(i+1));
		 	fstream receiveFileB(re_filenameB,fstream::in);
       		int ne,np;
        	receiveFileB >> ne;
        	receiveFileB >> np;
		std::vector<Ctxt> enbloomfilter;
	        for(int n=0;n<npacks;n++){
	                Ctxt ctxt(publicKey);
	                receiveFileB >> ctxt;
	                enbloomfilter.push_back(ctxt);
	        }
	       receiveFileB.close();
	        EnBloomFilter enBFB(global_nslots,global_ea,npacks,enbloomfilter);
	       enBFsB[i]=enBFB;
	    }
	});      
   
    	
	Ctxt cOne=getCOne(publicKey,global_nslots,global_ea);

	std::vector<EnBloomFilter> largerEnBFs;
	std::vector<EnBloomFilter> smallerEnBFs;
	

	if(enBFsA.size()>=enBFsB.size()){
	
		largerEnBFs=enBFsA;
		smallerEnBFs=enBFsB;
	}else{
		
		largerEnBFs=enBFsB;
		smallerEnBFs=enBFsA;
	}

	int re_npacks=0;
	int re_numelements=0;
	double time;
	Ctxt ctemp(publicKey);
	std::vector<std::vector<Ctxt>> results(smallerEnBFs.size(),std::vector<Ctxt>(npacks,ctemp));

	
	switch(type){
		case (BASIC_PROTOCOL):{

			BP basicProtocol(nThreads,global_nslots,npacks,global_ea);
			auto start1=std::chrono::system_clock::now();
			results=basicProtocol.execute(publicKey,largerEnBFs,smallerEnBFs);
			auto end1=std::chrono::system_clock::now();
			time=(double)std::chrono::duration_cast<std::chrono::milliseconds>(end1-start1).count()/1000;
			re_npacks=npacks;
			re_numelements=smallerEnBFs.size();
			break;
		}
		case(QUERIER_FRIENDLY_PROTOCOL):{
			QFP querierFriendlyProtocol(nThreads,global_nslots,npacks,global_ea);
			auto start2=std::chrono::system_clock::now();
			results[0]=querierFriendlyProtocol.execute(publicKey,largerEnBFs,smallerEnBFs);
			auto end2=std::chrono::system_clock::now();
			time=(double)std::chrono::duration_cast<std::chrono::milliseconds>(end2-start2).count()/1000;
			re_npacks=smallerEnBFs.size()/global_nslots;
			if((smallerEnBFs.size()%global_nslots)!=0)
				re_npacks++;
			re_numelements=1;
			break;
		}
		default:
			break;
	
	}

	/*-----------WRITE RESULT------------*/
	LOG(filenameA+" - "+filenameB+"=>algorithm type:"+to_string(type));
    LOG("[Cloud] PSI-CA: duration ="+to_string(time)+" sec.");

    std::fstream cipherResult(resultFilename, fstream::out|fstream::trunc);
    assert(cipherResult.is_open());
 	 cipherResult << re_numelements<<endl;
    cipherResult << re_npacks <<endl;
    for(int i=0;i<re_numelements;i++){
            for(int j=0;j<re_npacks;j++){
                    cipherResult <<results[i][j]<<endl;
	    }

    }
    cipherResult.close();
	return 0;
	
}

