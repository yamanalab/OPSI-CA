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

#ifndef OPSICA_DATAOWNER_CALLBACK_FUNCTION_HPP
#define OPSICA_DATAOWNER_CALLBACK_FUNCTION_HPP

#include <stdsc/stdsc_callback_function.hpp>
#include <opsica_share/opsica_define.hpp>

namespace opsica_share
{
class StateContext;
}

namespace opsica_dataowner
{
struct CallbackParam;

/**
 * @brief Provides callback function in receiving connect request.
 */
class CallbackFunctionRequestConnect : public stdsc::CallbackFunction
{
public:
    CallbackFunctionRequestConnect(CallbackParam& param) : param_(param)
    {
    }

protected:
    virtual void request_function(uint64_t code,
                                  stdsc::StateContext& state) override;
    virtual void data_function(uint64_t code, const stdsc::Buffer& buffer,
                               stdsc::StateContext& state) override;
    virtual void download_function(uint64_t code, const stdsc::Socket& sock,
                                   stdsc::StateContext& state) override;

private:
    CallbackParam& param_;
};

/**
 * @brief Provides callback function in receiving disconnect request.
 */
class CallbackFunctionRequestDisconnect : public stdsc::CallbackFunction
{
public:
    CallbackFunctionRequestDisconnect(CallbackParam& param) : param_(param)
    {
    }

protected:
    virtual void request_function(uint64_t code,
                                  stdsc::StateContext& state) override;
    virtual void data_function(uint64_t code, const stdsc::Buffer& buffer,
                               stdsc::StateContext& state) override;
    virtual void download_function(uint64_t code, const stdsc::Socket& sock,
                                   stdsc::StateContext& state) override;

private:
    CallbackParam& param_;
};
    
/**
 * @brief Provides callback function in receiving public key.
 */
class CallbackFunctionDataPubkey : public stdsc::CallbackFunction
{
public:
    CallbackFunctionDataPubkey(CallbackParam& param) : param_(param)
    {
    }

protected:
    virtual void request_function(uint64_t code,
                                  stdsc::StateContext& state) override;
    virtual void data_function(uint64_t code, const stdsc::Buffer& buffer,
                               stdsc::StateContext& state) override;
    virtual void download_function(uint64_t code, const stdsc::Socket& sock,
                                   stdsc::StateContext& state) override;

private:
    CallbackParam& param_;
};

/**
 * @brief Provides callback function in receiving parameters.
 */
class CallbackFunctionDataFpmax : public stdsc::CallbackFunction
{
public:
    CallbackFunctionDataFpmax(CallbackParam& param) : param_(param)
    {
    }

protected:
    virtual void request_function(uint64_t code,
                                  stdsc::StateContext& state) override;
    virtual void data_function(uint64_t code, const stdsc::Buffer& buffer,
                               stdsc::StateContext& state) override;
    virtual void download_function(uint64_t code, const stdsc::Socket& sock,
                                   stdsc::StateContext& state) override;

private:
    CallbackParam& param_;
};

/**
 * @brief Provides callback function in receiving upload request.
 */
class CallbackFunctionRequestUpload : public stdsc::CallbackFunction
{
public:
    CallbackFunctionRequestUpload(CallbackParam& param) : param_(param)
    {
    }

protected:
    virtual void request_function(uint64_t code,
                                  stdsc::StateContext& state) override;
    virtual void data_function(uint64_t code, const stdsc::Buffer& buffer,
                               stdsc::StateContext& state) override;
    virtual void download_function(uint64_t code, const stdsc::Socket& sock,
                                   stdsc::StateContext& state) override;

private:
    CallbackParam& param_;
};

} /* namespace opsica_dataowner */

#endif /* OPSICA_DATAOWNER_CALLBACK_FUNCTION_HPP */
