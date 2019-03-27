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

#ifndef OPSICA_PROTOCOL_HPP
#define OPSICA_PROTOCOL_HPP

#include <vector>

class EncryptedArray;
class FHEPubKey;
class Ctxt;

namespace opsica_share
{

class EnBloomFilter;

/**
 * @brief Enumeration for algorithm to compute PSICA.
 */
enum Algorithm_t : int32_t
{
    kAlgorithmNil = 0x0,
    kAlgorithmBasic = 0x1,
    kAlgorithmQuerierFriendly = 0x2,
};

/**
 * @brief Provides basic protocol.
 */
class BasicProtocol
{
protected:
    long nThreads;
    long global_nslots;
    int npacks;
    EncryptedArray* global_ea;
    void mergeEncryptedBFs(
      const FHEPubKey& publicKey,
      const std::vector<opsica_share::EnBloomFilter>& enBFs,
      opsica_share::EnBloomFilter& merged_enBF);
    virtual std::vector<Ctxt> inclusionCheck(
      const FHEPubKey& publicKey, const opsica_share::EnBloomFilter& bloomAll,
      const opsica_share::EnBloomFilter& bloom);

public:
    BasicProtocol(size_t nThreads, long global_nslots, int npacks,
       EncryptedArray* global_ea);
    std::vector<std::vector<Ctxt>> execute(
      const FHEPubKey& publicKey,
      const std::vector<opsica_share::EnBloomFilter>& largerEnBFs,
      const std::vector<opsica_share::EnBloomFilter>& smallerEnBFs);
};

/**
 * @brief Provides querier favorite protocol.
 */
class QuerierFavoriteProtocol : public BasicProtocol
{
    std::vector<Ctxt> inclusionCheck(
      const FHEPubKey& publicKey, const opsica_share::EnBloomFilter& bloomAll,
      const opsica_share::EnBloomFilter& bloom) override;
    std::vector<Ctxt> aggregate(const FHEPubKey& publicKey,
                                std::vector<std::vector<Ctxt>> bloom);

public:
    QuerierFavoriteProtocol(size_t nThreads, long global_nslots, int npacks,
        EncryptedArray* global_ea);
    std::vector<Ctxt> execute(
      const FHEPubKey& publicKey,
      const std::vector<opsica_share::EnBloomFilter>& largerEnBFs,
      const std::vector<opsica_share::EnBloomFilter>& smallerEnBFs);
};

} /* namespace opsica_share */

#endif /* OPSICA_PROTOCOL_HPP */
