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

#ifndef OPSICA_DATAOWNER_PARAM_HPP
#define OPSICA_DATAOWNER_PARAM_HPP

#include <string>
#include <cstdint>

namespace opsica_querier
{

/**
 * @brief This class is used to hold the parameter for dataowner.
 * The structure size is fixed 16 byte.
 */
struct DataOwnerParam
{
    double fpmax = 0.001;
    uint32_t nmax = 100;
    char pad[4];
};

} /* namespace opsica_dataowner */

#endif /* OPSICA_DATAOWNER_PARAM_HPP */
