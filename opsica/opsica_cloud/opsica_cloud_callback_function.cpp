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
#include <stdsc/stdsc_buffer.hpp>
#include <stdsc/stdsc_socket.hpp>
#include <stdsc/stdsc_packet.hpp>
#include <stdsc/stdsc_state.hpp>
#include <stdsc/stdsc_exception.hpp>
#include <stdsc/stdsc_log.hpp>
#include <opsica_share/opsica_packet.hpp>
#include <opsica_share/opsica_psica.hpp>
#include <opsica_share/opsica_protocol.hpp>
#include <opsica_cloud/opsica_cloud_callback_function.hpp>
#include <opsica_cloud/opsica_cloud_callback_param.hpp>
#include <opsica_cloud/opsica_cloud_state.hpp>
#include <opsica_cloud/opsica_cloud_data_manager.hpp>
#include <opsica_dataowner/opsica_dataowner_data_info.hpp>

namespace opsica_cloud
{

// CallbackFunctionRequestConnect

void CallbackFunctionRequestConnect::request_function(
  uint64_t code, stdsc::StateContext& state)
{
    STDSC_LOG_INFO("Received connect request. (current state : %lu)",
                   state.current_state());

    state.set(kEventConnectSocket);
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

    state.set(kEventDisconnectSocket);
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
      kStateConnected <= state.current_state(),
      "Warn: must be connected state to store pubkey.");

    auto data = reinterpret_cast<const char*>(buffer.data());
    auto size = buffer.size();
    std::ofstream(param_.pubkey_filename, std::ios::binary).write(data, size);
    STDSC_LOG_INFO("Created a public key file. (%s)",
                   param_.pubkey_filename.c_str());
    state.set(kEventPubKeyStore);
}
DEFINE_REQUEST_FUNC(CallbackFunctionDataPubkey);
DEFINE_DOWNLOAD_FUNC(CallbackFunctionDataPubkey);

// CallbackFunctionDataCompute

void CallbackFunctionDataCompute::data_function(uint64_t code,
                                                const stdsc::Buffer& buffer,
                                                stdsc::StateContext& state)
{
    STDSC_LOG_INFO(
      "Received compute request from Querier. (current state : %lu)",
      state.current_state());
    STDSC_THROW_CALLBACK_IF_CHECK(kStateReady <= state.current_state(),
                                  "Warn: must be ready state to compute.");

    auto algorithm_type = *static_cast<const int32_t*>(buffer.data());

    auto datamgrA = param_.data_managerA();
    auto datamgrB = param_.data_managerB();

    std::vector<std::string> filenamesA, filenamesB;
    for (size_t i = 0; i < datamgrA.size(); ++i)
    {
        filenamesA.push_back(datamgrA.filename(i));
    }
    for (size_t i = 0; i < datamgrB.size(); ++i)
    {
        filenamesB.push_back(datamgrB.filename(i));
    }

    STDSC_LOG_INFO("Computing OPSI-CA. (algorithm_type: %d)", algorithm_type);

    opsh::Psica psica(algorithm_type, param_.pubkey_filename);
    psica.calculate(filenamesA, filenamesB);
    psica.save_to_file(param_.result_filename);

    state.set(kEventJoinRequest);
}
DEFINE_REQUEST_FUNC(CallbackFunctionDataCompute);
DEFINE_DOWNLOAD_FUNC(CallbackFunctionDataCompute);

// CallbackFunctionDownloadResult

void CallbackFunctionDownloadResult::download_function(
  uint64_t code, const stdsc::Socket& sock, stdsc::StateContext& state)
{
    STDSC_LOG_INFO(
      "Received result request from Querier. (current state : %lu)",
      state.current_state());
    STDSC_THROW_CALLBACK_IF_CHECK(
      kStateComputed <= state.current_state(),
      "Warn: must be computed state to download results.");

    opsh::Psica psica(opsh::kAlgorithmNil, param_.pubkey_filename);
    psica.load_from_file(param_.result_filename);

    stdsc::BufferStream buffstream(psica.stream_size());
    std::iostream stream(&buffstream);
    psica.save_to_stream(stream);

    stdsc::Buffer* buff_ptr = &buffstream;
    auto size = static_cast<uint64_t>(buff_ptr->size());
    STDSC_LOG_INFO("Sending results to Querier.");
    sock.send_packet(
      stdsc::make_data_packet(opsh::kControlCodeDataResult, size));
    sock.send_buffer(*buff_ptr);
}
DEFINE_REQUEST_FUNC(CallbackFunctionDownloadResult);
DEFINE_DATA_FUNC(CallbackFunctionDownloadResult);

// CallbackFunctionDataDataA

static void parse_buffer(opdo::DataInfo& info, std::vector<char>& data,
                         const stdsc::Buffer& buffer)
{
    stdsc::BufferStream buffstream(buffer);
    std::iostream stream(&buffstream);

    stream.read(reinterpret_cast<char*>(&info), sizeof(opdo::DataInfo));
    STDSC_LOG_INFO("Data (%u/%u): filename: %s", info.index + 1, info.num,
                   info.filename);
    auto datasz = buffer.size() - sizeof(opdo::DataInfo);
    data.resize(datasz);
    std::fill(data.begin(), data.end(), 0);
    stream.read(&data[0], datasz);
}

#define DEFINE_CALLBACK_FUNC_DATA(T)                                          \
    void CallbackFunctionDataData##T::data_function(                          \
      uint64_t code, const stdsc::Buffer& buffer, stdsc::StateContext& state) \
    {                                                                         \
        STDSC_LOG_INFO("Received data from Dataowner" #T                      \
                       ". (current state : %lu)",                             \
                       state.current_state());                                \
        STDSC_THROW_CALLBACK_IF_CHECK(                                        \
          kStateConnected <= state.current_state(),                           \
          "Warn: must be connected state to store data" #T ".");              \
                                                                              \
        opdo::DataInfo info;                                                  \
        std::vector<char> data;                                               \
        parse_buffer(info, data, buffer);                                     \
                                                                              \
        auto datamgr = param_.data_manager##T(info.num);                      \
        datamgr.save(info.index, reinterpret_cast<void*>(&data[0]),           \
                     data.size(), info.filename);                             \
                                                                              \
        if (datamgr.is_all_saved())                                           \
        {                                                                     \
            state.set(kEventStoreRequest##T);                                 \
        }                                                                     \
    }

DEFINE_CALLBACK_FUNC_DATA(A);
DEFINE_REQUEST_FUNC(CallbackFunctionDataDataA);
DEFINE_DOWNLOAD_FUNC(CallbackFunctionDataDataA);

// CallbackFunctionDataDataB
DEFINE_CALLBACK_FUNC_DATA(B);
DEFINE_REQUEST_FUNC(CallbackFunctionDataDataB);
DEFINE_DOWNLOAD_FUNC(CallbackFunctionDataDataB);

} /* namespace opsica_cloud */
