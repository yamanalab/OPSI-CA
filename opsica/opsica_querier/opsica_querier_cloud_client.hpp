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

#ifndef OPSICA_QUERIER_CLOUD_CLIENT_HPP
#define OPSICA_QUERIER_CLOUD_CLIENT_HPP

#include <memory>
#include <stdsc/stdsc_thread.hpp>

namespace opsica_querier
{
class CommonParam;

/**
 * @brief Provides client for cloud.
 */
template <class T = CommonParam>
class CloudClient : public stdsc::Thread<T>
{
    using super = stdsc::Thread<T>;

public:
    CloudClient(const char* host, const char* port);
    virtual ~CloudClient(void);

    void start(T& param);
    void wait_for_finish(void);
    long get_result(void);

private:
    virtual void exec(T& args,
                      std::shared_ptr<stdsc::ThreadException> te) const;

    struct Impl;
    std::shared_ptr<Impl> pimpl_;
};

} /* namespace opsica_querier */

#endif /* OPSICA_QUERIER_CLOUD_CLIENT_HPP */
