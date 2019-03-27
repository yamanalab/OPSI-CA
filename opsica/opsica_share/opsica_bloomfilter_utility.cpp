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

#include <cstdint>
#include <stdsc/stdsc_log.hpp>
#include <stdsc/stdsc_exception.hpp>
#include <opsica_share/opsica_bloomfilter.hpp>
#include <opsica_share/opsica_define.hpp>
#include <opsica_share/opsica_utility.hpp>
#include <opsica_share/opsica_bloomfilter_utility.hpp>
#include <opsica_share/opsica_bloomfilter_data_header.hpp>

#include "FHE.h"
#include "EncryptedArray.h"

/*define ENABLE_PARALLEL*/

namespace opsica_share
{
namespace bfutility
{

void createBFs(const std::string& inputFName, const long nmax,
               const double fpmax, std::vector<std::vector<bool>>& bloomfilters,
               int& num_elements)
{
    STDSC_THROW_FILE_IF_CHECK(opsh::utility::file_exist(inputFName),
                              "inputFName not found.");

    std::vector<std::string> data;
    opsh::BloomParams bloomparams(nmax, fpmax);
    // printf("nmax=%d, fpmax=%lf\n", nmax,fpmax);
    std::fstream inputFile(inputFName, std::fstream::in);
    int size;
    std::string ssize;
    std::getline(inputFile, ssize);
    size = std::stoi(ssize);
    std::string buffer;
    for (int i = 0; i < size; i++)
    {
        std::getline(inputFile, buffer);
        data.push_back(buffer);
    }
    inputFile.close();
    num_elements = size;

    opsh::BloomFilter bf(bloomparams);
    opsh::BloomFilter bfs[num_elements];
    for (int i = 0; i < num_elements; i++)
    {
        bfs[i] = bf;
    }

    for (int i = 0; i < num_elements; i++)
    {
        bfs[i].add((uint8_t*)data[i].c_str(), data[i].size());
    }

    if (!bloomfilters.empty())
    {
        bloomfilters.clear();
    }
    for (int i = 0; i < num_elements; i++)
    {
        bloomfilters.push_back(bfs[i].bloomfilter);
    }
}

static void generate_encfile(const int index, const std::string& dir,
                             const std::string& basename, const long nslots,
                             const FHEPubKey& publicKey,
                             const EncryptedArray& ea,
                             const std::vector<std::vector<bool>>& bloomfilters,
                             const int num_elements)
{
    std::string re_uploadFDir = dir + "/" + enfilename(index, basename);
    STDSC_LOG_TRACE("Generate encrypt file. (%s)", re_uploadFDir.c_str());
    std::ofstream uploadFile(re_uploadFDir,
                             std::fstream::trunc | std::fstream::binary);
    assert(uploadFile.is_open());

    BloomFilterDataHeader header;
    std::memset(reinterpret_cast<void*>(&header), sizeof(BloomFilterDataHeader),
                0);
    header.num_elements = num_elements;

    opsh::EnBloomFilter dummy;
    int npacks = dummy.calculateNPacks(bloomfilters[0].size(), nslots);
    header.npacks = npacks;

    uploadFile.write(reinterpret_cast<char*>(&header),
                     sizeof(BloomFilterDataHeader));

    opsh::EnBloomFilter enBF(
      publicKey, nslots, const_cast<EncryptedArray*>(&ea), bloomfilters[index]);

    std::vector<Ctxt> enbloomfilter = enBF.enbloomfilter;
    for (int n = 0; n < npacks; n++)
    {
        enbloomfilter[n].write(uploadFile);
    }
    uploadFile.close();
}

#ifdef ENABLE_PARALLEL
static void thread_func(const size_t begin_index, const size_t end_index,
                        const std::string& dir, const std::string& basename,
                        const long nslots, const FHEPubKey& publicKey,
                        const EncryptedArray& ea,
                        const std::vector<std::vector<bool>>& bloomfilters,
                        const int num_elements)
{
    for (size_t i = begin_index; i < end_index; ++i)
    {
        generate_encfile(i, dir, basename, nslots, publicKey, ea, bloomfilters,
                         num_elements);
    }
}
#endif

void encryptBFs(const std::string& dir, const std::string& basename,
                const std::string& pubkeyFilename,
                const std::vector<std::vector<bool>>& bloomfilters,
                const size_t num_elements, const size_t num_threads)
{
    STDSC_THROW_FILE_IF_CHECK(opsh::utility::dir_exist(dir),
                              "Err: Output directory not found.");

    std::fstream pubkeyFile(pubkeyFilename, std::fstream::in);
    unsigned long m, p, r;
    std::vector<long> gens, orgs;

    readContextBase(pubkeyFile, m, p, r, gens, orgs);
    FHEcontext context(m, p, r, gens, orgs);
    pubkeyFile >> context;
    FHEPubKey publicKey(context);
    pubkeyFile >> publicKey;
    pubkeyFile.close();
    EncryptedArray ea(context);
    long nslots = ea.size();

#ifndef ENABLE_PARALLEL
    for (size_t i = 0; i < num_elements; ++i)
    {
        generate_encfile(i, dir, basename, nslots, publicKey, ea, bloomfilters,
                         num_elements);
    }
#else
    int block_size = num_elements / num_threads;
    size_t begin = 0, end = block_size;
    for (size_t th = 0; th < num_threads; ++th)
    {
        std::async(std::launch::async, thread_func, begin, end, dir, basename,
                   nslots, publicKey, ea, bloomfilters, num_elements);
        begin = end;
        end += block_size;
    }
#endif
}

std::string enfilename(const int index, const std::string& basename)
{
    std::string result = basename;
    return result.insert(basename.size() - 4, "-" + std::to_string(index));
}

} /* namespace bfutility */
} /* namespace opsica_share */
