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

#ifndef OPSICA_CLOUD_DATA_MANAGER_HPP
#define OPSICA_CLOUD_DATA_MANAGER_HPP

#include <memory>

namespace opsica_cloud
{

/**
 * @brief Manges data received from data owner A/B.
 */
class DataManager
{
public:
    DataManager(void);
    virtual ~DataManager(void) = default;

    void init(const size_t size);

    void save(const size_t index, const void* data, const size_t size,
              const std::string filename);

    std::string filename(const size_t index) const;

    bool is_saved(const size_t index) const;

    bool is_all_saved(void) const;

    bool is_initialized(void) const;

    size_t size(void) const;

private:
    struct Impl;
    std::shared_ptr<Impl> pimpl_;
};

} /* namespace opsica_cloud */

#endif /* OPSICA_CLOUD_DATA_MANAGER_HPP */
