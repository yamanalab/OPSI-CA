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

#include <fstream>
#include <iostream>
#include <vector>
#include <sstream>
#include <cstring>
#include <chrono>
#include <algorithm>
#include <sstream>

#include "FHE.h"
#include "EncryptedArray.h"

#include <stdsc/stdsc_exception.hpp>
#include <stdsc/stdsc_log.hpp>
#include <opsica_share/opsica_bloomfilter.hpp>
#include <opsica_share/opsica_bloomfilter_data_header.hpp>
#include <opsica_share/opsica_define.hpp>
#include <opsica_share/opsica_function.hpp>
#include <opsica_share/opsica_protocol.hpp>
#include <opsica_share/opsica_psica.hpp>

namespace opsica_share
{

struct Psica::Impl
{
    Impl(const int32_t algorithm_type, const std::string& pubkey_filename)
      : algorithm_type_(static_cast<Algorithm_t>(algorithm_type)),
        pubkey_filename_(pubkey_filename),
        re_npacks_(),
        re_numelements_()
    {
    }
    ~Impl(void) = default;

    void calculate(const std::vector<std::string>& filenamesA,
                   const std::vector<std::string>& filenamesB);

    void save_to_stream(std::ostream& os) const;
    void load_from_stream(std::istream& is);

    void save_to_file(const std::string& filename) const;
    void load_from_file(const std::string& filename);

    size_t stream_size(void) const;

    long get_result(const std::string& seckey_filename);

private:
    Algorithm_t algorithm_type_;
    std::string pubkey_filename_;
    int re_npacks_;
    int re_numelements_;
    std::vector<std::vector<Ctxt>> results_;
};

void Psica::Impl::calculate(const std::vector<std::string>& filenamesA,
                            const std::vector<std::string>& filenamesB)
{
    constexpr long nThreads = 4;

    if (filenamesA.size() != filenamesB.size())
    {
        std::ostringstream oss;
        oss << "Err: Invalid number of data file. ";
        oss << "(dataA: " << filenamesA.size()
            << ", dataB: " << filenamesB.size() << ")";
        STDSC_THROW_FAILURE(oss.str());
    }

    /*---------------READ PUBKEY FILE ------------------*/
    std::fstream pubkeyFile(pubkey_filename_, std::fstream::in);
    if (!pubkeyFile)
    {
        std::ostringstream oss;
        oss << "failed to open. (" << pubkey_filename_ << ")";
        STDSC_THROW_FILE(oss.str());
    }

    unsigned long m, p, r;
    std::vector<long> gens, orgs;
    long global_nslots;
    EncryptedArray* global_ea;
    readContextBase(pubkeyFile, m, p, r, gens, orgs);
    FHEcontext context(m, p, r, gens, orgs);
    pubkeyFile >> context;
    FHEPubKey publicKey(context);
    pubkeyFile >> publicKey;
    pubkeyFile.close();
    EncryptedArray ea(context);
    global_nslots = ea.size();
    global_ea = &ea;

    int num_elementsA;
    int num_elementsB;
    int npacks;

    STDSC_LOG_TRACE("reading A");

    std::string re_filenameA1 = filenamesA[0];
    std::fstream receiveFileA1(re_filenameA1, std::fstream::in);
    {
        opsh::BloomFilterDataHeader header;
        receiveFileA1.read(reinterpret_cast<char*>(&header),
                           sizeof(opsh::BloomFilterDataHeader));
        num_elementsA = header.num_elements;
        npacks = header.npacks;

        if (num_elementsA != static_cast<int>(filenamesA.size()))
        {
            std::ostringstream oss;
            oss << "Err: Invalid number of elements. (" << re_filenameA1 << ")";
            STDSC_THROW_FAILURE(oss.str());
        }
    }

    std::vector<opsh::EnBloomFilter> enBFsA(num_elementsA);
    for (int i = 0; i < num_elementsA; ++i)
    {
        std::string re_filenameA = filenamesA[i];
        std::fstream receiveFileA(re_filenameA, std::fstream::in);

        opsh::BloomFilterDataHeader header;
        receiveFileA.read(reinterpret_cast<char*>(&header),
                          sizeof(opsh::BloomFilterDataHeader));

        std::vector<Ctxt> enbloomfilter;
        for (int n = 0; n < npacks; n++)
        {
            Ctxt ctxt(publicKey);
            ctxt.read(receiveFileA);
            enbloomfilter.push_back(ctxt);
        }

        receiveFileA.close();
        opsh::EnBloomFilter enBFA(global_nslots, global_ea, npacks,
                                  enbloomfilter);
        enBFsA[i] = enBFA;
    }

    STDSC_LOG_TRACE("reading B");

    std::string re_filenameB1 = filenamesB[0];
    std::fstream receiveFileB1(re_filenameB1, std::fstream::in);
    {
        opsh::BloomFilterDataHeader header;
        receiveFileB1.read(reinterpret_cast<char*>(&header),
                           sizeof(opsh::BloomFilterDataHeader));
        num_elementsB = header.num_elements;
        npacks = header.npacks;

        if (num_elementsB != static_cast<int>(filenamesB.size()))
        {
            std::ostringstream oss;
            oss << "Err: Invalid number of elements. (" << re_filenameB1 << ")";
            STDSC_THROW_FAILURE(oss.str());
        }
    }

    std::vector<opsh::EnBloomFilter> enBFsB(num_elementsB);
    for (int i = 0; i < num_elementsB; ++i)
    {
        std::string re_filenameB = filenamesB[i];
        std::fstream receiveFileB(re_filenameB, std::fstream::in);

        opsh::BloomFilterDataHeader header;
        receiveFileB.read(reinterpret_cast<char*>(&header),
                          sizeof(opsh::BloomFilterDataHeader));

        std::vector<Ctxt> enbloomfilter;
        for (int n = 0; n < npacks; n++)
        {
            Ctxt ctxt(publicKey);
            ctxt.read(receiveFileB);
            enbloomfilter.push_back(ctxt);
        }
        receiveFileB.close();
        opsh::EnBloomFilter enBFB(global_nslots, global_ea, npacks,
                                  enbloomfilter);
        enBFsB[i] = enBFB;
    }

    Ctxt cOne = opsh::getCOne(publicKey, global_nslots, global_ea);

    std::vector<opsh::EnBloomFilter> largerEnBFs;
    std::vector<opsh::EnBloomFilter> smallerEnBFs;

    if (enBFsA.size() >= enBFsB.size())
    {
        largerEnBFs = enBFsA;
        smallerEnBFs = enBFsB;
    }
    else
    {
        largerEnBFs = enBFsB;
        smallerEnBFs = enBFsA;
    }

    double time;

    // initialize member variables
    re_npacks_ = 0;
    re_numelements_ = 0;
    Ctxt ctemp(publicKey);
    results_.resize(smallerEnBFs.size(), std::vector<Ctxt>(npacks, ctemp));

    switch (algorithm_type_)
    {
        case(kAlgorithmBasic) :
        {
            BasicProtocol basicProtocol(nThreads, global_nslots, npacks,
                                        global_ea);
            auto start1 = std::chrono::system_clock::now();
            results_ =
              basicProtocol.execute(publicKey, largerEnBFs, smallerEnBFs);
            auto end1 = std::chrono::system_clock::now();
            time =
              (double)std::chrono::duration_cast<std::chrono::milliseconds>(
                end1 - start1).count() /
              1000;
            re_npacks_ = npacks;
            re_numelements_ = smallerEnBFs.size();
            break;
        }
        case(kAlgorithmQuerierFriendly) :
        {
            QuerierFavoriteProtocol querierFriendlyProtocol(
              nThreads, global_nslots, npacks, global_ea);
            auto start2 = std::chrono::system_clock::now();
            results_[0] = querierFriendlyProtocol.execute(
              publicKey, largerEnBFs, smallerEnBFs);
            auto end2 = std::chrono::system_clock::now();
            time =
              (double)std::chrono::duration_cast<std::chrono::milliseconds>(
                end2 - start2).count() /
              1000;
            re_npacks_ = smallerEnBFs.size() / global_nslots;
            if ((smallerEnBFs.size() % global_nslots) != 0)
            {
                re_npacks_++;
            }
            re_numelements_ = 1;
            break;
        }
        default:
            break;
    }

    STDSC_LOG_TRACE("[Cloud] PSI-CA: duration = %s sec.",
                    std::to_string(time).c_str());
    ////std::cout << filenameA << " - " << filenameB << "=>algorithm type:" <<
    /// to_string(type) << std::endl;;
    // std::cout <<  << std::to_string(time) << " sec."
    //<< std::endl;
}

void Psica::Impl::save_to_stream(std::ostream& os) const
{
    os << re_numelements_ << std::endl;
    os << re_npacks_ << std::endl;
    for (int i = 0; i < re_numelements_; i++)
    {
        for (int j = 0; j < re_npacks_; j++)
        {
            os << results_[i][j] << std::endl;
        }
    }
}

void Psica::Impl::load_from_stream(std::istream& is)
{
    std::fstream pubkeyFile(pubkey_filename_, std::fstream::in);
    if (!pubkeyFile)
    {
        std::ostringstream oss;
        oss << "failed to open. (" << pubkey_filename_ << ")";
        STDSC_THROW_FILE(oss.str());
    }

    unsigned long m, p, r;
    std::vector<long> gens, orgs;
    readContextBase(pubkeyFile, m, p, r, gens, orgs);
    FHEcontext context(m, p, r, gens, orgs);
    pubkeyFile >> context;
    FHEPubKey publicKey(context);
    pubkeyFile >> publicKey;
    pubkeyFile.close();

    is >> re_numelements_;
    is >> re_npacks_;

    // clear resutls
    std::vector<std::vector<Ctxt>>().swap(results_);

    for (int i = 0; i < re_numelements_; i++)
    {
        std::vector<Ctxt> bf;
        for (int j = 0; j < re_npacks_; j++)
        {
            Ctxt ctxt(publicKey);
            is >> ctxt;
            bf.push_back(ctxt);
        }
        results_.push_back(bf);
    }
}

void Psica::Impl::save_to_file(const std::string& filename) const
{
    std::fstream ofs(filename, std::fstream::out | std::fstream::trunc);
    save_to_stream(ofs);
    ofs.close();
}

void Psica::Impl::load_from_file(const std::string& filename)
{
    std::fstream ifs(filename, std::fstream::in);
    load_from_stream(ifs);
    ifs.close();
}

size_t Psica::Impl::stream_size(void) const
{
    std::ostringstream oss;
    save_to_stream(oss);
    return oss.str().size();
}

long Psica::Impl::get_result(const std::string& seckey_filename)
{
    std::fstream secretFile(seckey_filename, std::fstream::in);
    unsigned long m, p, r;
    std::vector<long> gens, orgs;

    readContextBase(secretFile, m, p, r, gens, orgs);
    FHEcontext context(m, p, r, gens, orgs);
    secretFile >> context;

    FHESecKey secretKey(context);
    const FHEPubKey& publicKey = secretKey;

    secretFile >> secretKey;

    secretFile.close();

    EncryptedArray ea(context);
    long nslots = ea.size();

    long sum = 0;

    switch (algorithm_type_)
    {
        case kAlgorithmBasic:
            for (auto& bf : results_)
            {
                int allone = 1;
                for (auto& tmp_ctxt : bf)
                {
                    std::stringstream ss;
                    ss << tmp_ctxt;
                    ss.seekg(0);
                    Ctxt ctxt(publicKey);
                    ss >> ctxt;
                    std::vector<long> res;
                    ea.decrypt(ctxt, secretKey, res);
                    if (nslots != static_cast<long>(res.size()))
                    {
                        std::ostringstream oss;
                        oss << "Err: Invalid secret file. (" << seckey_filename
                            << ")";
                        STDSC_THROW_FAILURE(oss.str());
                    }
                    bool is_allone =
                      std::all_of(res.begin(), res.end(), [](long x)
                    { return x != 0; });
                    allone *= is_allone ? 1 : 0;
                }
                sum += allone;
            }
            break;
        case kAlgorithmQuerierFriendly:

            for (auto& bf : results_)
            {
                for (auto& tmp_ctxt : bf)
                {
                    std::stringstream ss;
                    ss << tmp_ctxt;
                    ss.seekg(0);
                    Ctxt ctxt(publicKey);
                    ss >> ctxt;
                    std::vector<long> res;
                    ea.decrypt(ctxt, secretKey, res);
                    if (nslots != static_cast<long>(res.size()))
                    {
                        std::ostringstream oss;
                        oss << "Err: Invalid secret file. (" << seckey_filename
                            << ")";
                        STDSC_THROW_FAILURE(oss.str());
                    }
                    for (auto x : res)
                    {
                        sum += (x == 1) ? 1 : 0;
                    }
                }
            }
            break;
        default:
            STDSC_THROW_FAILURE("Invalid altogrithm type specified.");
    }
    return sum;
}

Psica::Psica(const int32_t algorithm_type, const std::string& pubkey_filename)
  : pimpl_(new Impl(algorithm_type, pubkey_filename))
{
}

void Psica::calculate(const std::vector<std::string>& filenamesA,
                      const std::vector<std::string>& filenamesB)
{
    pimpl_->calculate(filenamesA, filenamesB);
}

void Psica::save_to_stream(std::ostream& os) const
{
    pimpl_->save_to_stream(os);
}

void Psica::load_from_stream(std::istream& is)
{
    pimpl_->load_from_stream(is);
}

void Psica::save_to_file(const std::string& filename) const
{
    pimpl_->save_to_file(filename);
}

void Psica::load_from_file(const std::string& filename)
{
    pimpl_->load_from_file(filename);
}

size_t Psica::stream_size(void) const
{
    return pimpl_->stream_size();
}

long Psica::get_result(const std::string& seckey_filename)
{
    return pimpl_->get_result(seckey_filename);
}
}
