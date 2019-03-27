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

#ifndef OPSICA_PSICA_HPP
#define OPSICA_PSICA_HPP

#include <string>
#include <vector>
#include <memory>

namespace opsica_share
{

/**
 * @brief Provides computing of PSI-CA.
 */
class Psica
{
public:
    Psica(const int32_t algorithm_type, const std::string& pubkey_filename);
    ~Psica(void) = default;

    void calculate(const std::vector<std::string>& filenamesA,
                   const std::vector<std::string>& filenamesB);

    void save_to_stream(std::ostream& os) const;
    void load_from_stream(std::istream& is);

    void save_to_file(const std::string& filename) const;
    void load_from_file(const std::string& filename);

    size_t stream_size(void) const;

    long get_result(const std::string& seckeyFilename);

private:
    struct Impl;
    std::shared_ptr<Impl> pimpl_;
};

} /* namespace opsica_share */

#endif /* OPSICA_PSICA_HPP */
