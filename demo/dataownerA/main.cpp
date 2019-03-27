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
#include <stdsc/stdsc_callback_function_container.hpp>
#include <stdsc/stdsc_callback_function.hpp>
#include <stdsc/stdsc_state.hpp>
#include <stdsc/stdsc_packet.hpp>
#include <stdsc/stdsc_log.hpp>
#include <stdsc/stdsc_exception.hpp>
#include <opsica_share/opsica_packet.hpp>
#include <opsica_share/opsica_define.hpp>
#include <opsica_dataowner/opsica_dataowner_thread.hpp>
#include <opsica_dataowner/opsica_dataowner_state.hpp>
#include <opsica_dataowner/opsica_dataowner_callback_function.hpp>
#include <opsica_dataowner/opsica_dataowner_callback_param.hpp>
#include <opsica_dataowner/opsica_dataowner_cloud_client.hpp>
#include <share/define.hpp>

struct Option
{
    std::string input_filename = "datasetA2_1.txt";
    std::string encfile_dir = "encrypteddatasets";
};

void init(Option& option, int argc, char* argv[])
{
    int opt;
    opterr = 0;
    while ((opt = getopt(argc, argv, "i:o:h")) != -1)
    {
        switch (opt)
        {
            case 'i':
                option.input_filename = optarg;
                break;
            case 'o':
                option.encfile_dir = optarg;
                break;
            case 'h':
            default:
                printf("Usage: %s [-i input_filename] [-o output_dir]\n",
                       argv[0]);
                exit(1);
        }
    }
}

void create_querier_server(opdo::CallbackParam& param,
                           stdsc::StateContext& state,
                           const char* port = DATAA_PORT_FOR_QUERIER)
{
    stdsc::CallbackFunctionContainer callback;

    std::shared_ptr<stdsc::CallbackFunction> cb_conn(
      new opdo::CallbackFunctionRequestConnect(param));
    callback.set(opsh::kControlCodeRequestConnect, cb_conn);

    std::shared_ptr<stdsc::CallbackFunction> cb_disconn(
      new opdo::CallbackFunctionRequestDisconnect(param));
    callback.set(opsh::kControlCodeRequestDisconnect, cb_disconn);

    std::shared_ptr<stdsc::CallbackFunction> cb_pubkey(
      new opdo::CallbackFunctionDataPubkey(param));
    callback.set(opsh::kControlCodeDataPubKey, cb_pubkey);

    std::shared_ptr<stdsc::CallbackFunction> cb_fpmax(
      new opdo::CallbackFunctionDataFpmax(param));
    callback.set(opsh::kControlCodeDataFpmax, cb_fpmax);

    std::shared_ptr<stdsc::CallbackFunction> cb_upload(
      new opdo::CallbackFunctionRequestUpload(param));

    callback.set(opsh::kControlCodeRequestUpload, cb_upload);

    opdo::DataownerThread dataowner(port, callback, state);
    dataowner.start();
    dataowner.join();
}

int main(int argc, char* argv[])
{
    STDSC_INIT_LOG();
    try
    {
        Option option;
        init(option, argc, argv);
        STDSC_LOG_INFO("Launched dataownerA demo app");
        stdsc::StateContext state(std::make_shared<opdo::StateInit>());
        opdo::CloudClient client(opsh::kDataOwnerA, "localhost",
                                 CLOUD_PORT_FOR_DATAOWA, state);
        opdo::CallbackParam param(client);
        param.input_filename = option.input_filename;
        param.encfile_dir = option.encfile_dir;
        create_querier_server(param, state);
    }
    catch (stdsc::AbstractException& e)
    {
        STDSC_LOG_ERR("Err: %s", e.what());
    }
    catch (...)
    {
        STDSC_LOG_ERR("Catch unknown exception");
    }
    return 0;
}
