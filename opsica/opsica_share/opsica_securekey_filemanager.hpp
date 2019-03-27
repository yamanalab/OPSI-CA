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

#ifndef OPSICA_SECUREKEY_FILEMANAGER
#define OPSICA_SECUREKEY_FILEMANAGER

#include <memory>

namespace opsica_share
{

/**
 * @brief Manages secure key files. (public / secret key files)
 */
class SecureKeyFileManager
{
    static constexpr long DefaultFheM = 30269;
    static constexpr long DefaultFheL = 34;
    
public:
    SecureKeyFileManager(const std::string& pubkey_filename,
                         const std::string& seckey_filename,
                         const long fheM = DefaultFheM, const long fheL = DefaultFheL);
    ~SecureKeyFileManager(void) = default;

    void initialize(void);

    size_t pubkey_size(void) const;
    size_t seckey_size(void) const;

    void pubkey_data(void* buffer);
    void seckey_data(void* buffer);

    bool is_exist_pubkey(void) const;
    bool is_exist_seckey(void) const;

    std::string pubkey_filename(void) const;
    std::string seckey_filename(void) const;

private:
    struct Impl;
    std::shared_ptr<Impl> pimpl_;
};

} /* namespace opsica_share */

#endif /* OPSICA_SECUREKEY_FILEMANAGER */
