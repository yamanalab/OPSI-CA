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
#include <vector>
#include <fstream>
#include <stdsc/stdsc_log.hpp>
#include <opsica_cloud/opsica_cloud_data_manager.hpp>

namespace opsica_cloud
{

struct DataManager::Impl
{
    Impl(void) = default;
    ~Impl(void) = default;

    void init(const size_t size)
    {
        std::vector<Info>().swap(record_);
        record_.resize(size, Info());
    }

    void save(const size_t index, const void* data, const size_t size,
              const std::string filename)
    {
        if (is_initialized() && index < record_.size())
        {
            if (std::ofstream ofs{filename,
                                  std::fstream::trunc | std::fstream::binary})
            {
                ofs.write(reinterpret_cast<const char*>(data), size);
                ofs.close();
                record_[index].is_saved = true;
                record_[index].filename = filename;
            }
        }
    }

    std::string filename(const size_t index) const
    {
        std::string filename;
        if (is_saved(index))
        {
            filename = record_[index].filename;
        }
        return filename;
    }

    bool is_saved(const size_t index) const
    {
        bool res = false;
        if (is_initialized() && index < record_.size())
        {
            res = record_[index].is_saved;
        }
        return res;
    }

    bool is_all_saved(void) const
    {
        bool res = true;
        if (!is_initialized())
        {
            res = false;
        }
        else
        {
            for (size_t i = 0; i < record_.size(); ++i)
            {
                STDSC_LOG_TRACE("is_saved(%u): %d", i, this->is_saved(i));
                if (!this->is_saved(i))
                {
                    res = false;
                    break;
                }
            }
        }
        return res;
    }

    bool is_initialized(void) const
    {
        return !record_.empty();
    }

    size_t size(void) const
    {
        return record_.size();
    }

private:
    struct Info
    {
        Info(void) : is_saved(false), filename()
        {
        }
        bool is_saved;
        std::string filename;
    };

    std::vector<Info> record_;
};

DataManager::DataManager(void) : pimpl_(new Impl())
{
}

void DataManager::init(const size_t size)
{
    pimpl_->init(size);
}

void DataManager::save(const size_t index, const void* data, const size_t size,
                       const std::string filename)
{
    pimpl_->save(index, data, size, filename);
}

std::string DataManager::filename(const size_t index) const
{
    return pimpl_->filename(index);
}

bool DataManager::is_saved(const size_t index) const
{
    return pimpl_->is_saved(index);
}

bool DataManager::is_all_saved(void) const
{
    return pimpl_->is_all_saved();
}

bool DataManager::is_initialized(void) const
{
    return pimpl_->is_initialized();
}

size_t DataManager::size(void) const
{
    return pimpl_->size();
}

} /* namespace opsica_cloud */
