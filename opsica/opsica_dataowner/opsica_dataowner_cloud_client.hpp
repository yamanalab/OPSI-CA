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

#ifndef OPSICA_DATAOWNER_CLOUD_CLIENT_HPP
#define OPSICA_DATAOWNER_CLOUD_CLIENT_HPP

#include <memory>
#include <opsica_share/opsica_define.hpp>

namespace stdsc
{
class StateContext;
class Buffer;
}

namespace opsica_dataowner
{

/**
 * @brief Provides client for cloud.
 */
class CloudClient
{
public:
    CloudClient(const opsh::DataOwner_t id, const char* host, const char* port,
                stdsc::StateContext& state);
    virtual ~CloudClient(void) = default;

    void upload(const stdsc::Buffer& buffer);

private:
    struct Impl;
    std::shared_ptr<Impl> pimpl_;
};

} /* namespace opsica_dataowner */

#endif /* OPSICA_DATAOWNER_CLOUD_CLIENT_HPP */
