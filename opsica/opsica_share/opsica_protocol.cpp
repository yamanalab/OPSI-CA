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

#include <future>
#include <algorithm>

#include "FHE.h"
#include "EncryptedArray.h"

#include <stdsc/stdsc_log.hpp>
#include <opsica_share/opsica_define.hpp>
#include <opsica_share/opsica_bloomfilter.hpp>
#include <opsica_share/opsica_async.hpp>
#include <opsica_share/opsica_function.hpp>
#include <opsica_share/opsica_protocol.hpp>

namespace opsica_share
{

BasicProtocol::BasicProtocol(size_t nThreads, long global_nslots, int npacks,
                             EncryptedArray* global_ea)
  : nThreads(nThreads),
    global_nslots(global_nslots),
    npacks(npacks),
    global_ea(global_ea)
{
}

std::vector<std::vector<Ctxt>> BasicProtocol::execute(
  const FHEPubKey& publicKey,
  const std::vector<opsh::EnBloomFilter>& largerEnBFs,
  const std::vector<opsh::EnBloomFilter>& smallerEnBFs)
{
    STDSC_LOG_DEBUG("merging bloom filters.....\n");
    opsh::EnBloomFilter enbloomSum;
    mergeEncryptedBFs(publicKey, largerEnBFs, enbloomSum);
    STDSC_LOG_DEBUG("finished merging bloom filters\n");

    size_t small_size = smallerEnBFs.size();
    Ctxt ctemp(publicKey);
    std::vector<std::vector<Ctxt>> results(small_size,
                                           std::vector<Ctxt>(npacks, ctemp));

    STDSC_LOG_DEBUG("inclusion check....\n");

    STDSC_LOG_DEBUG("small_size: %lu, nThreads: %lu\n", small_size, nThreads);
    std::function<bool(long, long)> fn = [&](long first, long last)
    {
        for (long i = first; i < last; i++)
        {
            std::vector<Ctxt> Cresult =
              inclusionCheck(publicKey, enbloomSum, smallerEnBFs[i]);
            for (long n = 0; n < npacks; n++)
            {
                results[i][n] = Cresult[n];
            }
        }
        return true;
    };
    OPSICA_ASYNC_EVAL(fn, small_size, nThreads);

    STDSC_LOG_DEBUG("finished inclusion check\n");
    return results;
}

void BasicProtocol::mergeEncryptedBFs(
  const FHEPubKey& publicKey, const std::vector<opsh::EnBloomFilter>& enBFs,
  opsh::EnBloomFilter& merged_enBF)
{
    if (enBFs.size() == 1)
    {
        merged_enBF = enBFs[0];
    }

    std::vector<opsh::EnBloomFilter> bfs;
    std::copy(enBFs.begin(), enBFs.end(), std::back_inserter(bfs));

    for (int n = 0; n < npacks; n++)
    {
        for (uint64_t i = 1; i < bfs.size(); i *= 2)
        {
            for (uint64_t j = 0; j < bfs.size(); j += i * 2)
            {
                uint64_t k = j + i;
                if (k >= bfs.size())
                {
                    break;
                }
                bfs[j].enbloomfilter[n] =
                  opsh::myOR(bfs[j].enbloomfilter[n], bfs[k].enbloomfilter[n]);
            }
        }
    }
    merged_enBF = bfs[0];
}

std::vector<Ctxt> BasicProtocol::inclusionCheck(
  const FHEPubKey& publicKey, const opsh::EnBloomFilter& bloomAll,
  const opsh::EnBloomFilter& bloom)
{
    Ctxt Cresult(publicKey);
    std::vector<Ctxt> Cresults(npacks, Cresult);
    NTL::ZZX zzxOne = opsh::getZZXOne(global_nslots, global_ea);
    for (int n = 0; n < npacks; n++)
    {
        Ctxt Cresult1 =
          opsh::myAND(bloomAll.enbloomfilter[n], bloom.enbloomfilter[n]);

        Ctxt Cresult2 =
          opsh::myNOT(opsh::myXOR(Cresult1, bloom.enbloomfilter[n]),
                      zzxOne); // 2.equality check
        Cresults[n] = Cresult2;
    }

    return Cresults;
}
    
QuerierFavoriteProtocol::QuerierFavoriteProtocol(size_t nThreads,
                                                 long global_nslots, int npacks,
                                                 EncryptedArray* global_ea)
  : BasicProtocol(nThreads, global_nslots, npacks, global_ea)
{
}
    
std::vector<Ctxt> QuerierFavoriteProtocol::execute(
  const FHEPubKey& publicKey,
  const std::vector<opsh::EnBloomFilter>& largerEnBFs,
  const std::vector<opsh::EnBloomFilter>& smallerEnBFs)
{
    std::vector<Ctxt> results;
    STDSC_LOG_DEBUG("merging bloom filters.....\n");
    opsh::EnBloomFilter enbloomSum;
    mergeEncryptedBFs(publicKey, largerEnBFs, enbloomSum);
    STDSC_LOG_DEBUG("finished merging bloom filters\n");

    size_t small_size = smallerEnBFs.size();
    long re_npacks = small_size / global_nslots;
    if ((small_size % global_nslots) != 0)
        re_npacks++;

    Ctxt ctemp(publicKey);

    std::vector<std::vector<Ctxt>> tournamentORs(
      re_npacks, std::vector<Ctxt>(global_nslots, ctemp));

    STDSC_LOG_DEBUG("inclusion check....\n");

    STDSC_LOG_DEBUG("small_size: %lu, nThreads: %lu\n", small_size, nThreads);
    std::function<bool(long, long)> fn = [&](long first, long last)
    {
        for (long i = first; i < last; i++)
        {
            std::vector<Ctxt> in_resultCtxts =
              inclusionCheck(publicKey, enbloomSum, smallerEnBFs[i]);
            //[001][000][000] -> [000][000][000]
            Ctxt in_resultCtxt = opsh::tournamentProductArray(in_resultCtxts);
            global_ea->rotate(in_resultCtxt, -1 * (i % global_nslots));
            tournamentORs[i / global_nslots][i % global_nslots] = in_resultCtxt;
        }
        return true;
    };
    OPSICA_ASYNC_EVAL(fn, small_size, nThreads);
    
    STDSC_LOG_DEBUG("finished inclusion check\n");
    STDSC_LOG_DEBUG("aggregating....\n");
    results = aggregate(publicKey, tournamentORs);
    STDSC_LOG_DEBUG("finished aggregating\n");

    return results;
}
    
std::vector<Ctxt> QuerierFavoriteProtocol::inclusionCheck(
  const FHEPubKey& publicKey, const opsh::EnBloomFilter& bloomAll,
  const opsh::EnBloomFilter& bloom)
{
    Ctxt Cresult(publicKey);
    std::vector<Ctxt> Cresults(npacks, Cresult);
    NTL::ZZX zzxOne = opsh::getZZXOne(global_nslots, global_ea);
    NTL::ZZX zzxOOne = opsh::getZZXOOne(global_nslots, global_ea);

    for (int n = 0; n < npacks; n++)
    {
        // 1.x=bloomAll & bloom
        Ctxt Cresult1 =
          opsh::myAND(bloomAll.enbloomfilter[n], bloom.enbloomfilter[n]); // 1.

        // 2.if(x==bloom)->1....1
        //	else ->0..1...0
        Ctxt Cresult2 =
          opsh::myNOT(opsh::myXOR(Cresult1, bloom.enbloomfilter[n]),
                      zzxOne); // 2.equality check

        // 3. bitwise and operation
        // 1......1 -> 1......1
        // 0..1...0 -> 0......0
        Ctxt Cresult3 = opsh::totalProduct(Cresult2, global_ea, global_nslots);

        // 4.
        //[000000000]*[0000000001]=[000000000000]
        //[111111111]*[0000000001]=[000000000001]
        Ctxt Cresult4 = opsh::myANDConstant(Cresult3, zzxOOne);

        Cresults[n] = Cresult4;
    }

    return Cresults;
}
    
std::vector<Ctxt> QuerierFavoriteProtocol::aggregate(
  const FHEPubKey& publicKey, std::vector<std::vector<Ctxt>> bloom)
{
    std::vector<Ctxt> Cresults;
    long re_npacks = bloom.size();
    for (int i = 0; i < re_npacks; i++)
    {
        Ctxt result = opsh::tournamentORope(bloom[i]);
        Cresults.push_back(result);
    }

    return Cresults;
}

} /* namespace opsica_share */
