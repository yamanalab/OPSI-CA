#ifndef DATAOWNER_H
#define DATAOWNER_H

#include "server.h"
#include "FHE.h"
#include "bloomfilter.h"
#include "logger.h"
#include "EncryptedArray.h"
#include "dataownersocket.h"
#include "NTL/BasicThreadPool.h"

class DataOwner{
	
	long nThreads;
	int  num_elements;
	long nmax;
	double fpmax;
	std::string pubkeyFilename;
	std::vector<std::vector<bool> > bloomfilters;
	std::vector<EnBloomFilter> enBFs;
	
	DataOwnerSocket* toCloud;

	int storePubkey(const std::string& filename,const std::string& data);

public:
	bool isRunning;
	DataOwner(long nThreads);
	~DataOwner();
	void runServer();
	void getNmaxFromCloud(const std::string filename);
	void encryptDataset(const std::string& filename);
	void createBFs(const std::string& inputFName);
	void encryptBFs(const std::string& uploadFDir);
	void uploadToCloud(const std::string& filename, const std::string& uploadFDir);
};
#endif
