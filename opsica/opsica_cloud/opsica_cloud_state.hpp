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

#ifndef OPSICA_CLOUD_STATE_HPP
#define OPSICA_CLOUD_STATE_HPP

#include <cstdbool>
#include <memory>
#include <stdsc/stdsc_state.hpp>

namespace opsica_cloud
{

/**
 * @brief Enumeration for state of cloud.
 */
enum StateId_t : uint64_t
{
    kStateNil = 0,
    kStateInit = 1,
    kStateConnected = 2,
    kStateReady = 3,
    kStateComputed = 4,
    kStateExit = 5,
};

/**
 * @brief Enumeration for events of data owner.
 */
enum Event_t : uint64_t
{
    kEventConnectSocket = 0,
    kEventDisconnectSocket = 1,
    kEventPubKeyStore = 2,
    kEventStoreRequestA = 3,
    kEventStoreRequestB = 4,
    kEventJoinRequest = 5,
    kEventResultRequest = 6,
    kEventInitialize = 7,
};

/**
 * @brief Provides 'Init' state.
 */
struct StateInit : public stdsc::State
{
    static std::shared_ptr<State> create(size_t initial_connection_count = 0);
    StateInit(size_t initial_connection_count = 0);
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
                                                bool is_stored_dataA = false,
                                                bool is_stored_dataB = false);

    StateConnected(bool is_stored_pubkey = false, bool is_stored_dataA = false,
                   bool is_stored_dataB = false);
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
 * @brief Provides 'Computed' state.
 */
struct StateComputed : public stdsc::State
{
    static std::shared_ptr<State> create();
    StateComputed(void);
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

} /* opsica_cloud */

#endif /* OPSICA_CLOUD_STATE_HPP */
