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
#include <vector>
#include <cstring> // std::strcpy
#include <stdsc/stdsc_buffer.hpp>
#include <stdsc/stdsc_socket.hpp>
#include <stdsc/stdsc_packet.hpp>
#include <stdsc/stdsc_state.hpp>
#include <stdsc/stdsc_exception.hpp>
#include <stdsc/stdsc_log.hpp>
#include <stdsc/stdsc_exception.hpp>
#include <opsica_share/opsica_utility.hpp>
#include <opsica_share/opsica_bloomfilter_utility.hpp>
#include <opsica_dataowner/opsica_dataowner_cloud_client.hpp>
#include <opsica_dataowner/opsica_dataowner_callback_function.hpp>
#include <opsica_dataowner/opsica_dataowner_callback_param.hpp>
#include <opsica_dataowner/opsica_dataowner_state.hpp>
#include <opsica_dataowner/opsica_dataowner_data_info.hpp>
#include <opsica_querier/opsica_querier_dataowner_param.hpp>

namespace opsica_dataowner
{

// CallbackFunctionRequestConnect

void CallbackFunctionRequestConnect::request_function(
  uint64_t code, stdsc::StateContext& state)
{
    STDSC_LOG_INFO("Received connect request. (current state : %lu)",
                   state.current_state());

    state.set(kEventConnectSocketFromQuerier);
    STDSC_LOG_TRACE("End callback.");
}
DEFINE_DATA_FUNC(CallbackFunctionRequestConnect);
DEFINE_DOWNLOAD_FUNC(CallbackFunctionRequestConnect);

// CallbackFunctionRequestDisconnect

void CallbackFunctionRequestDisconnect::request_function(
  uint64_t code, stdsc::StateContext& state)
{
    STDSC_LOG_INFO("Received disconnect request. (current state : %lu)",
                   state.current_state());

    state.set(kEventDisconnectSocketFromQuerier);
    STDSC_LOG_TRACE("End callback.");
}
DEFINE_DATA_FUNC(CallbackFunctionRequestDisconnect);
DEFINE_DOWNLOAD_FUNC(CallbackFunctionRequestDisconnect);

// CallbackFunctionDataPubkey

void CallbackFunctionDataPubkey::data_function(uint64_t code,
                                               const stdsc::Buffer& buffer,
                                               stdsc::StateContext& state)
{
    STDSC_LOG_INFO("Received public key from Querier. (current state : %lu)",
                   state.current_state());
    STDSC_THROW_CALLBACK_IF_CHECK(
      kStateConnected == state.current_state(),
      "Warn: must be connected state to store pubkey.");

    auto data = reinterpret_cast<const char*>(buffer.data());
    auto size = buffer.size();
    STDSC_LOG_INFO("Saved public key to file. (%s)",
                   param_.pubkey_filename.c_str());
    std::ofstream(param_.pubkey_filename, std::ios::binary).write(data, size);

    state.set(kEventPubKeyStore);
}
DEFINE_REQUEST_FUNC(CallbackFunctionDataPubkey);
DEFINE_DOWNLOAD_FUNC(CallbackFunctionDataPubkey);

// CallbackFunctionDataFpmax

void CallbackFunctionDataFpmax::data_function(uint64_t code,
                                              const stdsc::Buffer& buffer,
                                              stdsc::StateContext& state)
{
    STDSC_LOG_INFO("Received parameters from Querier. (current state : %lu)",
                   state.current_state());
    STDSC_THROW_CALLBACK_IF_CHECK(
      kStateConnected == state.current_state(),
      "Warn: must be connected state to store pubkey.");

    auto param_ptr = static_cast<const opqu::DataOwnerParam*>(buffer.data());
    param_.fpmax = param_ptr->fpmax;
    param_.nmax = static_cast<long>(param_ptr->nmax);
    STDSC_LOG_INFO("Received fpmax: %lf, nmax: %ld", param_.fpmax, param_.nmax);
    state.set(kEventFpmaxStore);
}
DEFINE_REQUEST_FUNC(CallbackFunctionDataFpmax);
DEFINE_DOWNLOAD_FUNC(CallbackFunctionDataFpmax);

// CallbackFunctionRequestUpload

void CallbackFunctionRequestUpload::request_function(uint64_t code,
                                                     stdsc::StateContext& state)
{
    STDSC_LOG_INFO(
      "Received upload request from Querier. (current state : %lu)",
      state.current_state());
    STDSC_THROW_CALLBACK_IF_CHECK(
      kStateReady == state.current_state(),
      "Warn: must be ready state to request upload.");

    auto& client = param_.client_;

    int num_elements;
    std::string basename = opsh::utility::basename(param_.input_filename);

    STDSC_LOG_INFO("Creating bloomfilters.");
    std::vector<std::vector<bool>> bloomfilters;
    opsh::bfutility::createBFs(param_.input_filename, param_.nmax, param_.fpmax,
                               bloomfilters, num_elements);
    STDSC_LOG_INFO("Encrypting bloomfilters.");
    opsh::bfutility::encryptBFs(param_.encfile_dir, basename,
                                param_.pubkey_filename, bloomfilters,
                                num_elements);

    DataInfo info{num_elements, 0, ""};
    for (int i = 0; i < num_elements; ++i)
    {
        info.index = i;
        auto enfilename = opsh::bfutility::enfilename(i, basename);
        auto enfilepath = param_.encfile_dir + "/" + enfilename;
        std::strcpy(info.filename, enfilename.c_str());
        if (!opsh::utility::file_exist(enfilepath))
        {
            std::ostringstream ss;
            ss << "Err: encryped file not found (" << enfilepath << ")";
            STDSC_THROW_FILE(ss.str());
        }

        auto filesz = opsh::utility::file_size(enfilepath);
        std::shared_ptr<char> data(new char[filesz]);
        if (std::ifstream ifs{enfilepath, std::fstream::binary})
        {
            ifs.read(data.get(), filesz);
        }

        stdsc::BufferStream buffstream(sizeof(DataInfo) + filesz);
        std::iostream ostream(&buffstream);
        ostream.write(reinterpret_cast<char*>(&info), sizeof(DataInfo));
        ostream.write(reinterpret_cast<char*>(data.get()), filesz);

        STDSC_LOG_INFO("Sending dataset. (%s)", enfilename.c_str());
        stdsc::Buffer* buff = &buffstream;
        client.upload(*buff);
    }

    state.set(kEventStoreRequest);
    STDSC_LOG_TRACE("End callback.");
}
DEFINE_DATA_FUNC(CallbackFunctionRequestUpload);
DEFINE_DOWNLOAD_FUNC(CallbackFunctionRequestUpload);

} /* namespace opsica_dataowner */
