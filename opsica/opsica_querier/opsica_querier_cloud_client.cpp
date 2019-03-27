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

#include <memory>
#include <stdsc/stdsc_client.hpp>
#include <stdsc/stdsc_buffer.hpp>
#include <stdsc/stdsc_packet.hpp>
#include <stdsc/stdsc_log.hpp>
#include <stdsc/stdsc_exception.hpp>
#include <opsica_share/opsica_packet.hpp>
#include <opsica_share/opsica_define.hpp>
#include <opsica_share/opsica_psica.hpp>
#include <opsica_share/opsica_securekey_filemanager.hpp>
#include <opsica_share/opsica_protocol.hpp>
#include <opsica_querier/opsica_querier_cloud_client.hpp>
#include <opsica_querier/opsica_querier_common_param.hpp>

namespace opsica_querier
{
template <class T>
struct CloudClient<T>::Impl
{
    std::shared_ptr<stdsc::ThreadException> te_;

    Impl(const char* host, const char* port) : host_(host), port_(port)
    {
        te_ = stdsc::ThreadException::create();
    }

    long get_result(void)
    {
        return result_;
    }

    void exec(T& args, std::shared_ptr<stdsc::ThreadException> te)
    {
        try
        {
            auto skm_ptr = args.skm();
            auto algo_type = opsh::kAlgorithmQuerierFriendly;
            constexpr uint32_t retry_interval_usec = OPSICA_RETRY_INTERVAL_USEC;
            constexpr uint32_t retry_interval_usec_to_send_compute_request =
              2000000;
            constexpr uint32_t timeout_sec = OPSICA_TIMEOUT_SEC;

            STDSC_THROW_FILE_IF_CHECK(skm_ptr->is_exist_pubkey(),
                                      "Public key not found.");
            STDSC_THROW_FILE_IF_CHECK(skm_ptr->is_exist_seckey(),
                                      "Secret key not found.");

            STDSC_LOG_INFO("Connecting to cloud.");
            client_.connect(host_, port_, retry_interval_usec, timeout_sec);
            STDSC_LOG_INFO("Connected to cloud.");

            STDSC_LOG_INFO("Requesting connect to cloud.");
            client_.send_request_blocking(opsh::kControlCodeRequestConnect,
                                          retry_interval_usec, timeout_sec);

            stdsc::Buffer pubkey(skm_ptr->pubkey_size());
            skm_ptr->pubkey_data(pubkey.data());
            STDSC_LOG_INFO("Sending public key to cloud.");
            client_.send_data_blocking(opsh::kControlCodeDataPubKey, pubkey,
                                       retry_interval_usec, timeout_sec);

            STDSC_LOG_INFO("Requesting computing OPSI-CA to cloud.");
            stdsc::Buffer compute_params(sizeof(int32_t));
            auto compute_params_ptr =
              reinterpret_cast<int32_t*>(compute_params.data());
            *compute_params_ptr = algo_type;
            client_.send_data_blocking(
              opsh::kControlCodeDataCompute, compute_params,
              retry_interval_usec_to_send_compute_request);

            STDSC_LOG_INFO("Downloading results from cloud.");
            stdsc::Buffer result;
            client_.recv_data_blocking(opsh::kControlCodeDownloadResult,
                                       result);

            STDSC_LOG_INFO("Requesting disconnect to cloud.");
            client_.send_request_blocking(opsh::kControlCodeRequestDisconnect,
                                          retry_interval_usec, timeout_sec);
            client_.close();

            STDSC_LOG_INFO("Decrypting results.");
            stdsc::BufferStream buffstream(result);
            std::iostream stream(&buffstream);
            opsh::Psica psica(algo_type, skm_ptr->pubkey_filename());
            psica.load_from_stream(stream);
            psica.save_to_file("result.txt");
            result_ = psica.get_result(skm_ptr->seckey_filename());
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
    long result_;
};

template <class T>
CloudClient<T>::CloudClient(const char* host, const char* port)
  : pimpl_(new Impl(host, port))
{
}

template <class T>
CloudClient<T>::~CloudClient(void)
{
    super::join();
}

template <class T>
void CloudClient<T>::start(T& param)
{
    super::start(param, pimpl_->te_);
}

template <class T>
void CloudClient<T>::wait_for_finish(void)
{
    super::join();
    pimpl_->te_->rethrow_if_has_exception();
}

template <class T>
long CloudClient<T>::get_result(void)
{
    this->wait_for_finish();
    return pimpl_->get_result();
}

template <class T>
void CloudClient<T>::exec(T& args,
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

template class CloudClient<CommonParam>;

} /* namespace opsica_querier */
