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

#include <iostream>
#include <opsica_share/opsica_config.hpp>
#include <opsica_share/opsica_define.hpp>
#include <opsica_share/opsica_securekey_filemanager.hpp>
#include <opsica_querier/opsica_querier_common_param.hpp>

namespace opsica_querier
{

struct CommonParam::Impl
{
    Impl(void) = default;
    ~Impl(void) = default;
    std::shared_ptr<opsh::SecureKeyFileManager> skm_ptr;
};

CommonParam::CommonParam(void) : pimpl_(new Impl())
{
}

CommonParam::CommonParam(const std::string& config_filename)
  : pimpl_(new Impl())
{
    load_config(config_filename);
}

void CommonParam::load_config(const std::string& config_filename)
{
    opsh::Config conf;
    conf.load_from_file(config_filename);
#define READ_FROM_CONFIG(key, type) \
    if (conf.is_exist_key(#key))    \
    key = opsh::config_get_value<type>(conf, #key)

    READ_FROM_CONFIG(fpmax, double);
    READ_FROM_CONFIG(nthreads, size_t);
    READ_FROM_CONFIG(fheM, long);
    READ_FROM_CONFIG(fheL, long);
#undef READ_FROM_CONFIG

#define COUT_CONFIG(key)                                                   \
    if (conf.is_exist_key(#key))                                           \
        std::cout << "Read a config file. (" << #key << ": " << key << ")" \
                  << std::endl;

    COUT_CONFIG(fpmax);
    COUT_CONFIG(nthreads);
    COUT_CONFIG(fheM);
    COUT_CONFIG(fheL);
#undef COUT_CONFIG
}

void CommonParam::set_skm(std::shared_ptr<opsh::SecureKeyFileManager> skm_ptr)
{
    pimpl_->skm_ptr = skm_ptr;
}

std::shared_ptr<opsh::SecureKeyFileManager> CommonParam::skm(void)
{
    return pimpl_->skm_ptr;
}

} /* namespace opsica_querier */
