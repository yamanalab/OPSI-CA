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

#ifndef OPSICA_BLOOMFILTER_DATA_HEADER_HPP
#define OPSICA_BLOOMFILTER_DATA_HEADER_HPP

#include <cstdint>

namespace opsica_share
{

/**
 * @brief This class is used to hold the data header to transmit bloomfilter.
 * The structure size is fixed 16 byte.
 */
struct BloomFilterDataHeader
{
    int32_t num_elements;
    int32_t npacks;
    char pad[8];
};

} /* namespace opsica_share */

#endif /* OPSICA_BLOOMFILTER_DATA_HEADER_HPP */
