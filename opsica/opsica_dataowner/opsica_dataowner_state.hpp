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

#ifndef OPSICA_DATAOWNER_STATE_HPP
#define OPSICA_DATAOWNER_STATE_HPP

#include <cstdbool>

#include <memory>
#include <stdsc/stdsc_state.hpp>
#include <opsica_share/opsica_define.hpp>

namespace opsica_dataowner
{

/**
 * @brief Enumeration for state of data owner.
 */
enum StateId_t : uint64_t
{
    kStateNil = 0,
    kStateInit = 1,
    kStateConnected = 2,
    kStateReady = 3,
    kStateUploaded = 4,
    kStateExit = 5,
};

/**
 * @brief Enumeration for events of data owner.
 */
enum Event_t : uint64_t
{
    kEventConnectSocketFromQuerier = 0,
    kEventConnectSocketToCloud = 1,
    kEventDisconnectSocketFromQuerier = 2,
    kEventPubKeyStore = 3,
    kEventFpmaxStore = 4,
    kEventStoreRequest = 5,
};

/**
 * @brief Provides 'Init' state.
 */
struct StateInit : public stdsc::State
{
    static std::shared_ptr<State> create(bool is_connected_from_querier = false,
                                         bool is_connected_to_cloud = false);
    StateInit(bool is_connected_from_querier = false,
              bool is_connected_to_cloud = false);
    virtual void set(stdsc::StateContext& sc, uint64_t event) override;
    virtual uint64_t id(void) const override;

private:
    struct Impl;
    std::shared_ptr<Impl> pimpl_;
};

/**
 * @brief Provides 'Connected' state.
 */
struct StateConnected : public stdsc::State
{
    static std::shared_ptr<stdsc::State> create(bool is_stored_pubkey = false,
                                                bool is_stored_fpmax = false);

    StateConnected(bool is_stored_pubkey = false, bool is_stored_fpmax = false);
    virtual void set(stdsc::StateContext& sc, uint64_t event) override;
    virtual uint64_t id(void) const override;

private:
    struct Impl;
    std::shared_ptr<Impl> pimpl_;
};

/**
 * @brief Provides 'Ready' state.
 */
struct StateReady : public stdsc::State
{
    static std::shared_ptr<State> create();
    StateReady(void);
    virtual void set(stdsc::StateContext& sc, uint64_t event) override;
    virtual uint64_t id(void) const override;

private:
    struct Impl;
    std::shared_ptr<Impl> pimpl_;
};

/**
 * @brief Provides 'Uploaded' state.
 */
struct StateUploaded : public stdsc::State
{
    static std::shared_ptr<State> create();
    StateUploaded(void);
    virtual void set(stdsc::StateContext& sc, uint64_t event) override;
    virtual uint64_t id(void) const override;

private:
    struct Impl;
    std::shared_ptr<Impl> pimpl_;
};

/**
 * @brief Provides 'Exit' state.
 */
struct StateExit : public stdsc::State
{
    static std::shared_ptr<State> create();
    StateExit(void);
    virtual void set(stdsc::StateContext& sc, uint64_t event) override;
    virtual uint64_t id(void) const override;

private:
    struct Impl;
    std::shared_ptr<Impl> pimpl_;
};

} /* opsica_dataowner */

#endif /* OPSICA_DATAOWNER_STATE_HPP */
