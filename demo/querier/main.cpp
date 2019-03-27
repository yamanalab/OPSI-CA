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
#include <stdsc/stdsc_socket.hpp>
#include <stdsc/stdsc_packet.hpp>
#include <stdsc/stdsc_client.hpp>
#include <stdsc/stdsc_buffer.hpp>
#include <stdsc/stdsc_log.hpp>
#include <stdsc/stdsc_exception.hpp>
#include <opsica_share/opsica_define.hpp>
#include <opsica_share/opsica_securekey_filemanager.hpp>
#include <opsica_querier/opsica_querier_cloud_client.hpp>
#include <opsica_querier/opsica_querier_common_param.hpp>
#include <opsica_querier/opsica_querier_dataowner_client.hpp>
#include <share/define.hpp>

static constexpr const char* PUBKEY_FILENAME = "pubkey.txt";
static constexpr const char* SECKEY_FILENAME = "seckey.txt";

void init(opqu::CommonParam& param, int argc, char* argv[])
{
    std::string config_filename;
    std::string pubkey_filename = PUBKEY_FILENAME;
    std::string seckey_filename = SECKEY_FILENAME;
    bool enable_generate_securekey = false;

    int opt;
    opterr = 0;
    while ((opt = getopt(argc, argv, "c:p:s:gh")) != -1)
    {
        switch (opt)
        {
            case 'c':
                config_filename = optarg;
                break;
            case 'p':
                pubkey_filename = optarg;
                break;
            case 's':
                seckey_filename = optarg;
                break;
            case 'g':
                enable_generate_securekey = true;
                break;
            case 'h':
            default:
                printf(
                  "Usage: %s [-c config_filename] [-p pubkey_filename] [-s "
                  "seckey_filename] [-g]\n",
                  argv[0]);
                exit(1);
        }
    }

    if (!config_filename.empty())
    {
        param.load_config(config_filename);
    }

    std::shared_ptr<opsh::SecureKeyFileManager> skm_ptr(
      new opsh::SecureKeyFileManager(pubkey_filename, seckey_filename,
                                     param.fheM, param.fheL));
    if (enable_generate_securekey)
    {
        skm_ptr->initialize();
    }
    param.set_skm(skm_ptr);
}

void exec(opqu::CommonParam& param)
{
    const char* host = "localhost";

    opqu::CloudClient<> cloud_client(host, CLOUD_PORT_FOR_QUERIER);
    cloud_client.start(param);

    opqu::DataownerClient<> dataownerA_client(host, DATAA_PORT_FOR_QUERIER);
    dataownerA_client.start(param);
    opqu::DataownerClient<> dataownerB_client(host, DATAB_PORT_FOR_QUERIER);
    dataownerB_client.start(param);

    cloud_client.wait_for_finish();
    dataownerA_client.wait_for_finish();
    dataownerB_client.wait_for_finish();

    STDSC_LOG_INFO("Result: %ld\n", cloud_client.get_result());
}

int main(int argc, char* argv[])
{
    STDSC_INIT_LOG();
    try
    {
        opqu::CommonParam param;
        init(param, argc, argv);
        STDSC_LOG_INFO("Launched querier demo app");
        exec(param);
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
