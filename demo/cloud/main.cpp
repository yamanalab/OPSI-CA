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

#include <stdsc/stdsc_log.hpp>
#include <stdsc/stdsc_exception.hpp>
#include <opsica_share/opsica_define.hpp>
#include <opsica_cloud/opsica_cloud_state.hpp>
#include <share/define.hpp>
#include "server_querier.hpp"
#include "server_dataowner.hpp"

using namespace opsica_demo;

int main()
{
    try
    {
        STDSC_INIT_LOG();
        STDSC_LOG_INFO("Launched cloud demo app");
        opcl::CallbackParam param;
        stdsc::StateContext state(std::make_shared<opcl::StateInit>());

        ServerQuerier server_querier(param, state);
        server_querier.start();
        ServerDataowner<opcl::CallbackFunctionDataDataA,
                        opsh::kControlCodeDataDataA>
          server_dataA(param, state, CLOUD_PORT_FOR_DATAOWA);
        server_dataA.start();
        ServerDataowner<opcl::CallbackFunctionDataDataB,
                        opsh::kControlCodeDataDataB>
          server_dataB(param, state, CLOUD_PORT_FOR_DATAOWB);
        server_dataB.start();

        server_querier.join();
        server_dataA.join();
        server_dataB.join();
    }
    catch (stdsc::AbstractException &e)
    {
        STDSC_LOG_ERR("Err: %s", e.what());
    }
    catch (...)
    {
        STDSC_LOG_ERR("Catch unknown exception");
    }
    return 0;
}
