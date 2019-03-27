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

#include <unordered_map>
#include <opsica_share/opsica_define.hpp>
#include <opsica_cloud/opsica_cloud_callback_param.hpp>
#include <opsica_cloud/opsica_cloud_data_manager.hpp>

static constexpr char* DEFAULT_PUBKEY_FILENAME = (char*)"pubkey.txt";
static constexpr char* DEFAULT_RESULT_FILENAME = (char*)"result.txt";

namespace opsica_cloud
{

struct CallbackParam::Impl
{
    Impl(void) = default;
    ~Impl(void) = default;
    DataManager datamgrA_, datamgrB_;
};

CallbackParam::CallbackParam(void)
  : pubkey_filename(DEFAULT_PUBKEY_FILENAME),
    result_filename(DEFAULT_RESULT_FILENAME),
    pimpl_(new Impl())
{
}

#define DEFILE_DATAMGR_GETTER_METHOD(T)                           \
    DataManager& CallbackParam::data_manager##T(const size_t num) \
    {                                                             \
        static bool initialized = false;                          \
        if (!initialized)                                         \
        {                                                         \
            pimpl_->datamgr##T##_.init(num);                      \
            initialized = true;                                   \
        }                                                         \
        return pimpl_->datamgr##T##_;                             \
    }

DEFILE_DATAMGR_GETTER_METHOD(A);
DEFILE_DATAMGR_GETTER_METHOD(B);

#undef DEFILE_DATAMGR_GETTER_METHOD

} /* namespace opsica_cloud */
