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

#ifndef OPSICA_ASYNC_HPP
#define OPSICA_ASYNC_HPP

#include <future>
#include <algorithm>
#include <vector>

#define OPSICA_ASYNC_EVAL(func, size, nThreads)                                \
    do                                                                         \
    {                                                                          \
        size_t nTh = (nThreads);                                               \
        int block_size = (size) / (nTh);                                       \
        if ((size) < (nTh))                                                    \
        {                                                                      \
            block_size = 1;                                                    \
            (nTh) = (size);                                                    \
        }                                                                      \
        std::vector<std::future<bool>> futures;                                \
        futures.reserve((nTh));                                                \
                                                                               \
        size_t begin = 0, end = block_size;                                    \
        for (size_t i = 0; i < (nTh); ++i)                                     \
        {                                                                      \
            futures.push_back(                                                 \
              std::async(std::launch::async, (func), begin, end));             \
            begin = end;                                                       \
            end += block_size;                                                 \
        }                                                                      \
        bool b2 = (func)(begin, (size));                                       \
        bool b1 =                                                              \
          std::all_of(futures.begin(), futures.end(), [](std::future<bool>& f) \
        { return f.get(); });                                                  \
        if (!b1 || !b2)                                                        \
        {                                                                      \
            printf("Err: failed to inclusion check.\n");                       \
            exit(0);                                                           \
        }                                                                      \
    } while (0)

#endif /* OPSICA_ASYNC_HPP */
