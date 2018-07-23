#ifndef QUERIER_H
#define QUERIER_H

#include "FHE.h"
#include <time.h>
#include <algorithm>
#include <fstream>
#include <iostream>
#include "function.h"
#include "bloomfilter.h"
#include "EncryptedArray.h"
#include "queriersocket.h"
#include "logger.h"

#define BASIC_PROTOCOL 1
#define QUERIER_FRIENDLY_PROTOCOL 2

#define A 0
#define B 1

class Querier{
	
	long nmax;
	double fpmax;
	int fheM;
	int algorithmType;
	
	QuerierSocket* toServer;
	QuerierSocket *toDataowners[2];

	string pubkeyFileName;
	string seckeyFileName;
	long result;	
	int filterSize;

	bool checkAllOne(std::vector<long> vec,long nslots);

public:	
	Querier(const double fpmax, const int fheM, const int algorithmType);	
	~Querier();
	void getNmax(const std::string filename);
	void generateFHEParams(const std::string publicFName, const std::string secretFName);
	void sendParamsToServer();
	void sendParamsToDataowner(const int name);
	void requestPSICAToServer(const std::string datasetA, const std::string datasetB, const std::string resultFName);
	void downloadResult(const std::string resultFName);
	void decryptResult(const std::string resultFName);
	long getResult();
};

#endif
