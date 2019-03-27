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

#include <unistd.h>
#include <memory>
#include <stdsc/stdsc_client.hpp>
#include <stdsc/stdsc_buffer.hpp>
#include <stdsc/stdsc_packet.hpp>
#include <stdsc/stdsc_log.hpp>
#include <stdsc/stdsc_exception.hpp>
#include <opsica_share/opsica_packet.hpp>
#include <opsica_share/opsica_define.hpp>
#include <opsica_share/opsica_securekey_filemanager.hpp>
#include <opsica_querier/opsica_querier_dataowner_client.hpp>
#include <opsica_querier/opsica_querier_common_param.hpp>
#include <opsica_querier/opsica_querier_dataowner_param.hpp>

namespace opsica_querier
{
template <class T>
struct DataownerClient<T>::Impl
{
    std::shared_ptr<stdsc::ThreadException> te_;

    Impl(const char* host, const char* port) : host_(host), port_(port)
    {
        te_ = stdsc::ThreadException::create();
    }

    void exec(T& args, std::shared_ptr<stdsc::ThreadException> te)
    {
        try
        {
            auto skm_ptr = args.skm();
            constexpr uint32_t retry_interval_usec = OPSICA_RETRY_INTERVAL_USEC;
            constexpr uint32_t timeout_sec = OPSICA_TIMEOUT_SEC;

            STDSC_THROW_FILE_IF_CHECK(skm_ptr->is_exist_pubkey(),
                                      "Public key not found.");

            STDSC_LOG_INFO("Connecting to dataowner.");
            client_.connect(host_, port_, retry_interval_usec, timeout_sec);
            STDSC_LOG_INFO("Connected to dataowner.");

            STDSC_LOG_INFO("Requesting connect to dataowner.");
            client_.send_request_blocking(opsh::kControlCodeRequestConnect,
                                          retry_interval_usec, timeout_sec);

            stdsc::Buffer pubkey(skm_ptr->pubkey_size());
            skm_ptr->pubkey_data(pubkey.data());
            STDSC_LOG_INFO("Sending public key to dataowner.");
            client_.send_data_blocking(opsh::kControlCodeDataPubKey, pubkey,
                                       retry_interval_usec, timeout_sec);

            stdsc::Buffer doparam(sizeof(DataOwnerParam));
            auto doparam_ptr =
              reinterpret_cast<DataOwnerParam*>(doparam.data());
            doparam_ptr->fpmax = args.fpmax;
            doparam_ptr->nmax = args.nmax;
            STDSC_LOG_INFO("Sending parameters to dataowner.");
            client_.send_data_blocking(opsh::kControlCodeDataFpmax, doparam,
                                       retry_interval_usec, timeout_sec);

            STDSC_LOG_INFO("Requesting uploading data to dataowner.");
            client_.send_request_blocking(opsh::kControlCodeRequestUpload,
                                          retry_interval_usec, timeout_sec);

            STDSC_LOG_INFO("Requesting disconnect to dataowner.");
            client_.send_request_blocking(opsh::kControlCodeRequestDisconnect,
                                          retry_interval_usec, timeout_sec);
            client_.close();
        }
        catch (const stdsc::AbstractException& e)
        {
            STDSC_LOG_TRACE("Failed to client process (%s)", e.what());
            te->set_current_exception();
        }
    }

private:
    const char* host_;
    const char* port_;
    stdsc::Client client_;
};

template <class T>
DataownerClient<T>::DataownerClient(const char* host, const char* port)
  : pimpl_(new Impl(host, port))
{
}

template <class T>
DataownerClient<T>::~DataownerClient(void)
{
    super::join();
}

template <class T>
void DataownerClient<T>::start(T& param)
{
    super::start(param, pimpl_->te_);
}

template <class T>
void DataownerClient<T>::wait_for_finish(void)
{
    super::join();
    pimpl_->te_->rethrow_if_has_exception();
}

template <class T>
void DataownerClient<T>::exec(T& args,
                              std::shared_ptr<stdsc::ThreadException> te) const
{
    try
    {
        pimpl_->exec(args, te);
    }
    catch (...)
    {
        te->set_current_exception();
    }
}

template class DataownerClient<CommonParam>;

} /* namespace opsica_querier */
