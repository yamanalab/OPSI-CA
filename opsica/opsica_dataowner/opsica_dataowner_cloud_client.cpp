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

#include <stdsc/stdsc_state.hpp>
#include <stdsc/stdsc_packet.hpp>
#include <stdsc/stdsc_client.hpp>
#include <stdsc/stdsc_log.hpp>
#include <opsica_share/opsica_packet.hpp>
#include <opsica_dataowner/opsica_dataowner_cloud_client.hpp>
#include <opsica_dataowner/opsica_dataowner_state.hpp>

namespace opsica_dataowner
{

struct CloudClient::Impl
{
    Impl(const opsh::DataOwner_t id, const char* host, const char* port,
         stdsc::StateContext& state)
      : state_(state), id_(id)
    {
        STDSC_LOG_TRACE("Connecting %s@%s", host, port);
        client_.connect(host, port);
        state_.set(kEventConnectSocketToCloud);
        STDSC_LOG_INFO("Connected %s@%s", host, port);
        
        STDSC_LOG_INFO("Requesting connect to cloud.");
        client_.send_request_blocking(opsh::kControlCodeRequestConnect);
    }

    ~Impl(void) = default;

    void upload(const stdsc::Buffer& buffer)
    {
        opsh::ControlCode_t code = (id_ == opsh::kDataOwnerA)
                                     ? opsh::kControlCodeDataDataA
                                     : opsh::kControlCodeDataDataB;
        client_.send_data_blocking(code, buffer);
    }

private:
    stdsc::StateContext& state_;
    const opsh::DataOwner_t id_;
    stdsc::Client client_;
};

CloudClient::CloudClient(const opsh::DataOwner_t id, const char* host,
                         const char* port, stdsc::StateContext& state)
  : pimpl_(new Impl(id, host, port, state))
{
}

void CloudClient::upload(const stdsc::Buffer& buffer)
{
    pimpl_->upload(buffer);
}

} /* opsica_dataowner */
