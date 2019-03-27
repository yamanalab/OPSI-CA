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

#ifndef OPSICA_COMMON_PARAM_HPP
#define OPSICA_COMMON_PARAM_HPP

#include <memory>

namespace opsica_share
{
class SecureKeyFileManager;
}

namespace opsica_querier
{

/**
 * @brief This class is used to hold the common parameter for whole system.
 */
struct CommonParam
{
    CommonParam(void);
    explicit CommonParam(const std::string& config_filename);
    virtual ~CommonParam(void) = default;

    void load_config(const std::string& config_filename);

    void set_skm(std::shared_ptr<opsica_share::SecureKeyFileManager> skm_ptr);
    std::shared_ptr<opsica_share::SecureKeyFileManager> skm(void);

    double fpmax = 0.001;
    int32_t nmax = 100;
    size_t nthreads = 2;
    long fheM = 30269;
    long fheL = 34;

private:
    struct Impl;
    std::shared_ptr<Impl> pimpl_;
};

} /* namespace opsica_querier */

#endif /* OPSICA_COMMON_PARAM_HPP */
