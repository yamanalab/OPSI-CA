#ifndef CLOUD_H
#define CLOUD_H

#include "FHE.h"
#include "function.h"
#include "EncryptedArray.h"
#include "bloomfilter.h"
#include "logger.h"
#include "server.h"
#include "protocol.h"
#include "NTL/BasicThreadPool.h"
#include <iostream>
#include <cstring>
#include <chrono>

#define BASIC_PROTOCOL 1
#define QUERIER_FRIENDLY_PROTOCOL 2
#define PUBKEYFOLDER "./pubkey/"
#define RESULTFOLDER "./result/"
#define STOREDATAFOLDER "./datasets/"
class Cloud{
	long nThreads;	
	long nmax;
	Server *server;
public:
	bool isRunning;
	Cloud();
	Cloud(long nThreads, long nmax);
	~Cloud();
	void start();
	long storeData(const std::string& filename,const std::string& data);
	int calculatePSICA(const std::string filenameA,const std::string filenameB,const std::string resultFilename,const std::string pubkeyFilenamem, const int type);
		
};

#endif
