/*
 * Copyright 2018 Yamana Laboratory, Waseda University
 * Supported by JST CREST Grant Number JPMJCR1503, Japan.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE‐2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef BLOOM_FILTER_H
#define BLOOM_FILTER_H

#include <vector>
#include <array>

class EncryptedArray;
class Ctxt;
class FHEPubKey;

namespace opsica_share
{

/**
 * @brief This class is used to hold the parameters for bloomfilter.
 */
class BloomParams
{
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

/**
 * @brief This class is used to hold the bloomfilter.
 */
class BloomFilter
{
    uint64_t num_elements;
    uint8_t num_hashes;
    uint64_t filter_size;
    double false_positive_probability;

public:
    std::vector<bool> bloomfilter;
    BloomFilter();
    BloomFilter(const BloomParams& params);
    BloomFilter(const BloomFilter& filter);
    std::array<uint64_t, 2> hash(const uint8_t* data, std::size_t len);
    uint64_t nthHash(uint8_t n, uint64_t hashA, uint64_t hashB);
    void add(const uint8_t* data, std::size_t len);
    bool constains(const uint8_t* data, std::size_t len);
    bool contains(const BloomFilter& filter);
    void bloomJoin(const BloomFilter& filter);
    void clear();
    void print();
};

/**
 * @brief This class is used to hold the encrypted bloomfilter.
 */
class EnBloomFilter
{
public:
    long nslots;
    EncryptedArray* ea;
    int npacks;
    std::vector<Ctxt> enbloomfilter;
    std::vector<long> convertToVector(const std::vector<bool> bf);
    EnBloomFilter();
    EnBloomFilter(const FHEPubKey& publicKey, long nslots, EncryptedArray* ea,
                  const std::vector<bool> bloomfilter);
    EnBloomFilter(long nslots, EncryptedArray* ea, int npacks,
                  const std::vector<Ctxt> enbf);
    int calculateNPacks(int size, long nslots);
    EnBloomFilter& operator=(const EnBloomFilter& other);
};

} /* namespace opsica_share */

#endif
