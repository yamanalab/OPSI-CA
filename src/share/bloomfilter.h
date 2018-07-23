#ifndef BLOOM_FILTER_H
#define BLOOM_FILTER_H

#include <iostream>
#include <vector>
#include <cmath>
#include <string>
#include <array>
#include "FHE.h"
#include "EncryptedArray.h"
#include "MurmurHash3.h"

class BloomParams{
public:
	uint64_t num_elements;
	uint8_t num_hashes;
	uint64_t filter_size;
	double false_positive_probability;
	
	BloomParams(uint64_t num_ele);
	BloomParams(uint64_t num_ele, double error);
	void createParams();
	BloomParams& operator=(const BloomParams& other);
};
class BloomFilter{
	uint64_t num_elements;
	uint8_t num_hashes;
	uint64_t filter_size;
	double false_positive_probability;
public:
	std::vector<bool> bloomfilter;
	BloomFilter();
	BloomFilter(const BloomParams& params);
	BloomFilter(const BloomFilter& filter);
	std::array<uint64_t,2> hash(const uint8_t* data, std::size_t len);
	uint64_t nthHash(uint8_t n, uint64_t hashA, uint64_t hashB);
	void add(const uint8_t *data, std::size_t len);
	bool constains(const uint8_t *data, std::size_t len);
	bool contains(const BloomFilter& filter);
	void bloomJoin(const BloomFilter& filter);
	void clear();
	void print();

};

class EnBloomFilter{
public:
	long nslots;
	EncryptedArray* ea;
	int npacks;
	std::vector<Ctxt> enbloomfilter;
	std::vector<long> convertToVector(const std::vector<bool> bf);
	EnBloomFilter();
	EnBloomFilter(const FHEPubKey& publicKey,long nslots, EncryptedArray* ea, const std::vector<bool> bloomfilter);
	EnBloomFilter(long nslots,EncryptedArray* ea, int npacks, const std::vector<Ctxt> enbf);
	int calculateNPacks(int size, long nslots);
	EnBloomFilter& operator=(const EnBloomFilter& other);

};

#endif
