#include "querier.h"


Querier::Querier( const double fpmax, const int fheM, const int algorithmType){

	//initialize parameters
	this->fpmax=fpmax;
	this->fheM=fheM;
	this->algorithmType=algorithmType;

	this->pubkeyFileName="./pubkey/pubkey.txt";
	this->seckeyFileName="./seckey/seckey.txt";

	//connect to cloud server;
	this->toServer=new QuerierSocket(10000);
	toServer->connect();

	//connect to data owner A
	this->toDataowners[A]=new QuerierSocket(10001);
	toDataowners[A]->connect();

	//connect to data owner B
	this->toDataowners[B]=new QuerierSocket(10002);
	toDataowners[B]->connect();
}
Querier::~Querier(){

	toServer->disconnect();
	toDataowners[A]->disconnect();
	toDataowners[B]->disconnect();
	delete toServer;
	delete toDataowners[A];
	delete toDataowners[B];
}



//TODO create FHE class
void Querier::generateFHEParams(const std::string publicFName, const std::string secretFName){

	this->pubkeyFileName=publicFName;
	this->seckeyFileName=secretFName;

	//initialization for FHE
	long m =0;
	long p=2;//p^r
	long r=1;
	//long L=14;//レベル
	long L = 34;
	long c=3;//3
	long w=64;//64
	long d=0;
	long k=128;//90 or 128
	long s=0;

	//LOG("Finding m...\n");
	//m=FindM(k,L,c,p,d,s,0);;
	m=fheM;



	LOG("Initialization context...");

	FHEcontext context(m,p,r);
	buildModChain(context,L,c);
	LOG("OK!");

	LOG("Creating polynomial...");
	ZZX G = context.alMod.getFactorsOverZZ()[0];
	LOG("OK!");


	LOG("Generating keys...");
	FHESecKey secretKey(context);
	const FHEPubKey& publicKey=secretKey;
	
	secretKey.GenSecKey(w);
	addSome1DMatrices(secretKey);
	LOG("OK!");

	
	EncryptedArray ea(context, G);

	//END INITIALIZATION


	//rotate level not increased
	//1:context.zMStar.numOgGens()==1
	//			AND
	//2:context.zMStar.SameOrd(i)==true

	
	if(context.zMStar.numOfGens()!=1){
		LOG("noise increase when it rotates");

		exit(0);
	}
	for(int i=0;i<(int)context.zMStar.numOfGens();i++){
		if(!context.zMStar.SameOrd(i)){

			LOG("noise increase when it rotates");
			exit(0);
		}
	}
	LOG("noise doesn't increase when it rotates");


	long nslots=ea.size();


	LOG("[FHE param] m="+std::to_string(m)+",p="+std::to_string(p)+",r="+std::to_string(r)+",L="+std::to_string(L)+",c="+std::to_string(c)+",w="+std::to_string(w)+",d="+std::to_string(d)+",k="+std::to_string(k)+",nslots="+std::to_string(nslots)+",sec="+std::to_string(context.securityLevel()));
	

    
    /* ----------- Public Key Writing -------*/
    std::fstream publicFile(pubkeyFileName,std::fstream::out|std::fstream::trunc);
    assert(publicFile.is_open());
    // Output ContextBase
    writeContextBase(publicFile,context);
    publicFile << context << endl;
    // Output PublicKey
    publicFile << publicKey << endl;
    publicFile.close();

    /* ---------- Secret Key Writing ------ */

    fstream secretFile(seckeyFileName,fstream::out|fstream::trunc);
    assert(secretFile.is_open());
    // Output ContextBase
    writeContextBase(secretFile,context);
    secretFile << context << endl;
    // Output ScretKey 
    secretFile << secretKey << endl;
    secretFile.close();


}
void Querier::getNmax(const std::string filename){

	ssize_t size;
	std::string data;
        size=toServer->downloadNmax(filename,data);
        if(size<0){
                LOG("failed to download the nmax");
                return;
        }	
	nmax=std::stol(data);
}
void Querier::sendParamsToServer(){

	ssize_t size;
	//send public key;
	size=toServer->sendPubKey(pubkeyFileName,pubkeyFileName);	
	if(size<0)
		LOG("failed to send params to server");

}
void Querier::sendParamsToDataowner(const int name){

	ssize_t size;
	//send public key;
	size=toDataowners[name]->sendPubKey(pubkeyFileName,pubkeyFileName);
		
	if(size<0)
		LOG("failed to send params to data owner"+std::to_string(name));

	//send fpmax;
	size=toDataowners[name]->sendFpmax("fpmax.txt","fpmax.txt");
	if(size<0)
		LOG("failed to send params to data owner"+std::to_string(name));


}
void Querier::requestPSICAToServer(const std::string datasetA, const std::string datasetB,const std::string resultFName){

	ssize_t size;
	//request for PSI-CA to server
	size=toServer->sendPSICARequest(datasetA+" "+datasetB+" "+resultFName+" "+pubkeyFileName+" "+std::to_string(algorithmType));
	if(size<0)
		LOG("failed to send PSICA request");
} 
void Querier::downloadResult(const std::string resultFName){

	double time;

	std::string data;
	
	/*------start timer------*/
	auto start=std::chrono::system_clock::now();
	ssize_t size;
	size=toServer->downloadResult(resultFName,data);
	if(size<0){
		LOG("failed to download the result");
		return;
	}
	auto end=std::chrono::system_clock::now();
	/*-----end timer-------*/


	std::fstream receiveFile(resultFName,std::fstream::out|std::fstream::trunc);
	assert(receiveFile.is_open());
	
	const char* str=data.c_str();
	receiveFile.write(str,strlen(str));
	receiveFile.close();
	

	time=(double)std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count()/1000;


	LOG("[Querier] downloading duration ="+std::to_string(time)+ " sec.");
	LOG("[Querier] downloaded data = "+std::to_string((double)size/1000000)+" MB");



}

void Querier::decryptResult(const std::string resultFileName){
	
	fstream secretFile(seckeyFileName,fstream::in);
    unsigned long m,p,r;
    vector<long> gens,orgs;
    
    readContextBase(secretFile,m,p,r,gens,orgs);
    FHEcontext context(m,p,r,gens,orgs);
    secretFile >> context;
    
    FHESecKey secretKey(context);
    const FHEPubKey& publicKey=secretKey;
    
    secretFile >> secretKey;
    
    secretFile.close();

    EncryptedArray ea(context);
    long nslots=ea.size();

    std::vector<Ctxt> enResults;

    //read result file and decrypt
	fstream resultFile(resultFileName,fstream::in);
    
    int num;
    int npacks;
    int sum=0;
    resultFile >> num;
    resultFile >> npacks;
	
	switch(algorithmType){
	case BASIC_PROTOCOL:
   		
	//IDEA1
   	for(int i=0;i<num;i++){
		
    		int allone=1;
    		for(int j=0;j<npacks;j++){
    			Ctxt ctxt(publicKey);
	        	resultFile >> ctxt;
	        	std::vector<long> res;
	        	enResults.push_back(ctxt);
	        	ea.decrypt(ctxt,secretKey,res);
	        	//decryptAndPrint(ctxt,secretKey,nslots,&ea);
	       
	        	
      			if(checkAllOne(res,nslots)){
      				allone*=1;
      			}else{
      				allone*=0;
      				
      			}
     
    		}

    		sum+=allone;
    		
    	}
	break;

	case QUERIER_FRIENDLY_PROTOCOL:

    	
    	for(int i=0;i<num;i++){
    		for(int j=0;j<npacks;j++){
    			Ctxt ctxt(publicKey);
	       		resultFile >> ctxt;
	        	std::vector<long> res;
	        	enResults.push_back(ctxt);
	        	ea.decrypt(ctxt,secretKey,res);
	        	//decryptAndPrint(ctxt,secretKey,nslots,&ea);   

      			
 			for(int i=0;i<nslots;i++){
				if(res[i]==1)
					sum++;
			}
    		}

    	}
	break;

	
			
	}	
    	resultFile.close();


	result= sum;

}

long Querier::getResult(){

	return result;

}

bool Querier::checkAllOne(std::vector<long> vec,long nslots){
	for(int i=0 ;i < nslots;i++){
		if(vec[i]==0)
			return false;
	}
	return true;
}






