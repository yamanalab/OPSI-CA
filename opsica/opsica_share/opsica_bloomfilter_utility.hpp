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

#ifndef OPSICA_BLOOMFILTER_UTILITY_HPP
#define OPSICA_BLOOMFILTER_UTILITY_HPP

#include <vector>
#include <string>

namespace opsica_share
{
namespace bfutility
{

void createBFs(const std::string& inputFName, const long nmax,
               const double fpmax, std::vector<std::vector<bool>>& bloomfilters,
               int& num_elements);

void encryptBFs(const std::string& dir, const std::string& basename,
                const std::string& pubkeyFilename,
                const std::vector<std::vector<bool>>& bloomfilters,
                const size_t num_elements, const size_t num_threads = 2);

std::string enfilename(const int index, const std::string& basename);

} /* namespace bfutility */
} /* namespace opsica_share */

#endif /* OPSICA_BLOOMFILTER_UTILITY_HPP */
