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

#include <mutex>
#include <stdsc/stdsc_state.hpp>
#include <stdsc/stdsc_log.hpp>
#include <opsica_dataowner/opsica_dataowner_state.hpp>

namespace opsica_dataowner
{

struct StateInit::Impl
{
    Impl(bool is_connected_from_querier, bool is_connected_to_cloud)
      : id_(kStateInit),
        is_connected_from_querier_(is_connected_from_querier),
        is_connected_to_cloud_(is_connected_to_cloud)
    {
    }

    void set(stdsc::StateContext& sc, uint64_t event)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        STDSC_LOG_TRACE("StateInit(%lu): event#%lu", id_, event);
        switch (static_cast<Event_t>(event))
        {
            case kEventConnectSocketFromQuerier:
                is_connected_from_querier_ = true;
                break;
            case kEventConnectSocketToCloud:
                is_connected_to_cloud_ = true;
                break;
            case kEventDisconnectSocketFromQuerier:
                is_connected_from_querier_ = false;
                break;
            default:
                break;
        }
        if (is_connected_from_querier_ && is_connected_to_cloud_)
        {
            sc.next_state(StateConnected::create());
        }
    }

    uint64_t id(void) const
    {
        return id_;
    }

private:
    uint64_t id_;
    bool is_connected_from_querier_;
    bool is_connected_to_cloud_;
    std::mutex mutex_;
};

struct StateConnected::Impl
{
    Impl(bool is_stored_pubkey = false, bool is_stored_fpmax = false)
      : id_(kStateConnected),
        is_stored_pubkey_(is_stored_pubkey),
        is_stored_fpmax_(is_stored_fpmax)
    {
    }

    void set(stdsc::StateContext& sc, uint64_t event)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        STDSC_LOG_TRACE("StateConnected: event#%lu", event);
        switch (static_cast<Event_t>(event))
        {
            case kEventPubKeyStore:
                is_stored_pubkey_ = true;
                break;
            case kEventFpmaxStore:
                is_stored_fpmax_ = true;
                break;
            case kEventDisconnectSocketFromQuerier:
                sc.next_state(StateInit::create(false, true));
                break;
            default:
                break;
        }

        if (is_stored_pubkey_ && is_stored_fpmax_)
        {
            sc.next_state(StateReady::create());
        }
    }

    uint64_t id(void) const
    {
        return id_;
    }

private:
    uint64_t id_;
    bool is_stored_pubkey_;
    bool is_stored_fpmax_;
    std::mutex mutex_;
};

struct StateReady::Impl
{
    Impl(void) : id_(kStateReady)
    {
    }

    void set(stdsc::StateContext& sc, uint64_t event)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        STDSC_LOG_TRACE("StateReady(%lu): event#%lu", id_, event);
        switch (static_cast<Event_t>(event))
        {
            case kEventPubKeyStore:
                sc.next_state(StateConnected::create(false, true));
                break;
            case kEventFpmaxStore:
                sc.next_state(StateConnected::create(true, false));
                break;
            case kEventStoreRequest:
                sc.next_state(StateUploaded::create());
                break;
            case kEventDisconnectSocketFromQuerier:
                sc.next_state(StateInit::create(false, true));
                break;
            default:
                break;
        }
    }

    uint64_t id(void) const
    {
        return id_;
    }

private:
    uint64_t id_;
    std::mutex mutex_;
};

struct StateUploaded::Impl
{
    Impl(void) : id_(kStateUploaded)
    {
    }

    void set(stdsc::StateContext& sc, uint64_t event)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        STDSC_LOG_TRACE("StateUploaded(%lu): event#%lu", id_, event);
        switch (static_cast<Event_t>(event))
        {
            case kEventPubKeyStore:
                sc.next_state(StateConnected::create(false, true));
                break;
            case kEventFpmaxStore:
                sc.next_state(StateConnected::create(true, false));
                break;
            case kEventStoreRequest:
                sc.next_state(StateReady::create());
                break;
            case kEventDisconnectSocketFromQuerier:
                sc.next_state(StateInit::create(false, true));
                break;
            default:
                break;
        }
    }

    uint64_t id(void) const
    {
        return id_;
    }

private:
    uint64_t id_;
    std::mutex mutex_;
};

struct StateExit::Impl
{
    Impl(void) : id_(kStateExit)
    {
    }

    void set(stdsc::StateContext& sc, uint64_t event)
    {
    }

    uint64_t id(void) const
    {
        return id_;
    }

private:
    uint64_t id_;
    std::mutex mutex_;
};

// Init

std::shared_ptr<stdsc::State> StateInit::create(bool is_connected_from_querier,
                                                bool is_connected_to_cloud)
{
    auto s = std::shared_ptr<stdsc::State>(
      new StateInit(is_connected_from_querier, is_connected_to_cloud));
    return s;
}

StateInit::StateInit(bool is_connected_from_querier, bool is_connected_to_cloud)
  : pimpl_(new Impl(is_connected_from_querier, is_connected_to_cloud))
{
}

void StateInit::set(stdsc::StateContext& sc, uint64_t event)
{
    pimpl_->set(sc, event);
}

uint64_t StateInit::id(void) const
{
    return pimpl_->id();
}

// Connected

std::shared_ptr<stdsc::State> StateConnected::create(bool is_stored_pubkey,
                                                     bool is_stored_fpmax)
{
    auto s = std::shared_ptr<stdsc::State>(
      new StateConnected(is_stored_pubkey, is_stored_fpmax));
    return s;
}

StateConnected::StateConnected(bool is_stored_pubkey, bool is_stored_fpmax)
  : pimpl_(new Impl(is_stored_pubkey, is_stored_fpmax))
{
}

void StateConnected::set(stdsc::StateContext& sc, uint64_t event)
{
    pimpl_->set(sc, event);
}

uint64_t StateConnected::id(void) const
{
    return pimpl_->id();
}

// Ready

std::shared_ptr<stdsc::State> StateReady::create()
{
    auto s = std::shared_ptr<stdsc::State>(new StateReady());
    return s;
}

StateReady::StateReady(void) : pimpl_(new Impl())
{
}

void StateReady::set(stdsc::StateContext& sc, uint64_t event)
{
    pimpl_->set(sc, event);
}

uint64_t StateReady::id(void) const
{
    return pimpl_->id();
}

// Uploaded

std::shared_ptr<stdsc::State> StateUploaded::create()
{
    auto s = std::shared_ptr<stdsc::State>(new StateUploaded());
    return s;
}

StateUploaded::StateUploaded(void) : pimpl_(new Impl())
{
}

void StateUploaded::set(stdsc::StateContext& sc, uint64_t event)
{
    pimpl_->set(sc, event);
}

uint64_t StateUploaded::id(void) const
{
    return pimpl_->id();
}

// Exit

std::shared_ptr<stdsc::State> StateExit::create()
{
    auto s = std::shared_ptr<stdsc::State>(new StateExit());
    return s;
}

StateExit::StateExit(void) : pimpl_(new Impl())
{
}

void StateExit::set(stdsc::StateContext& sc, uint64_t event)
{
    pimpl_->set(sc, event);
}

uint64_t StateExit::id(void) const
{
    return pimpl_->id();
}

} /* opsica_dataowner */
