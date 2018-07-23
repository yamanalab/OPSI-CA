#ifndef PROTOCOL_H
#define PROTOCOL_H

#include "FHE.h"
#include "function.h"
#include "EncryptedArray.h"
#include "bloomfilter.h"
#include "logger.h"
#include "NTL/BasicThreadPool.h"
#include <iostream>
#include <cstring>
#include <chrono>

#define BASIC_PROTOCOL 1
#define QUERIER_FRIENDLY_PROTOCOL 2

class BP{
protected:	
	long nThreads;	
	long global_nslots;
	int npacks;
	EncryptedArray* global_ea;
	EnBloomFilter mergeEncryptedBFs(const FHEPubKey& publicKey,std::vector<EnBloomFilter> enBFs);
	virtual std::vector<Ctxt> inclusionCheck(const FHEPubKey& publicKey,EnBloomFilter bloomAll, EnBloomFilter bloom);
public:
	BP(long nThreads,  long global_nslots, int npacks,EncryptedArray* global_ea );
	std::vector<std::vector<Ctxt>> execute(const FHEPubKey& publicKey,  std::vector<EnBloomFilter> largerEnBFs, std::vector<EnBloomFilter> smallerEnBFs );
			
};

class QFP:public BP{

	std::vector<Ctxt> inclusionCheck(const FHEPubKey& publicKey,EnBloomFilter bloomAll, EnBloomFilter bloom)override;
	std::vector<Ctxt> aggregate(const FHEPubKey& publicKey, std::vector<std::vector<Ctxt>> bloom);

public:
	QFP(long nThreads, long global_nslots, int npacks,EncryptedArray* global_ea);
	std::vector<Ctxt> execute(const FHEPubKey& publicKey,  std::vector<EnBloomFilter> largerEnBFs, std::vector<EnBloomFilter> smallerEnBFs );
};
#endif
