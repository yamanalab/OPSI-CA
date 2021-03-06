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

#include <stdsc/stdsc_server.hpp>
#include <stdsc/stdsc_log.hpp>
#include <opsica_dataowner/opsica_dataowner_thread.hpp>
#include <opsica_dataowner/opsica_dataowner_state.hpp>

namespace opsica_dataowner
{

struct DataownerThread::Impl
{
    Impl(const char* port, stdsc::CallbackFunctionContainer& callback,
         stdsc::StateContext& state)
      : server_(new stdsc::Server<>(port, state)), state_(state)
    {
        server_->set_callback(callback);
        //state_.set(kEventConnectSocketFromQuerier);
        STDSC_LOG_INFO("Lanched dataowner thread (%s)", port);
    }

    ~Impl(void) = default;

    void start(void)
    {
        server_->start();
    }

    void join(void)
    {
        server_->wait_for_finish();
    }

private:
    std::shared_ptr<stdsc::Server<>> server_;
    stdsc::StateContext& state_;
};

DataownerThread::DataownerThread(const char* port,
                                 stdsc::CallbackFunctionContainer& callback,
                                 stdsc::StateContext& state)
  : pimpl_(new Impl(port, callback, state))
{
}

void DataownerThread::start(void)
{
    pimpl_->start();
}

void DataownerThread::join(void)
{
    pimpl_->join();
}

} /* namespace opsica_dataowner */
