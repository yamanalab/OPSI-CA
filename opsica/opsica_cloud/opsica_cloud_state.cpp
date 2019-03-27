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
#include <opsica_cloud/opsica_cloud_state.hpp>

namespace opsica_cloud
{

/* querier, dataownerA, B */
static constexpr std::size_t MAX_CLIENT_NUM = 3;

struct StateInit::Impl
{
    Impl(size_t initial_connection_count)
      : id_(kStateInit), count_(initial_connection_count)
    {
    }

    void set(stdsc::StateContext& sc, uint64_t event)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        STDSC_LOG_TRACE("StateInit(%lu): event#%lu, conn_count:%ld", id_, event,
                        count_);
        switch (static_cast<Event_t>(event))
        {
            case kEventConnectSocket:
                if (++count_ >= MAX_CLIENT_NUM)
                {
                    sc.next_state(StateConnected::create());
                }
                break;
            case kEventDisconnectSocket:
                --count_;
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
    std::size_t count_;
    std::mutex mutex_;
};

struct StateConnected::Impl
{
    Impl(bool is_stored_pubkey = false, bool is_stored_dataA = false,
         bool is_stored_dataB = false)
      : id_(kStateConnected),
        is_stored_pubkey_(is_stored_pubkey),
        is_stored_dataA_(is_stored_dataA),
        is_stored_dataB_(is_stored_dataB)
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
            case kEventStoreRequestA:
                is_stored_dataA_ = true;
                break;
            case kEventStoreRequestB:
                is_stored_dataB_ = true;
                break;
            case kEventDisconnectSocket:
                sc.next_state(StateInit::create(MAX_CLIENT_NUM - 1));
                break;
            default:
                break;
        }

        if (is_stored_pubkey_ && is_stored_dataA_ && is_stored_dataB_)
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
    bool is_stored_dataA_;
    bool is_stored_dataB_;
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
            case kEventJoinRequest:
                sc.next_state(StateComputed::create());
                break;
            case kEventDisconnectSocket:
                sc.next_state(StateInit::create(MAX_CLIENT_NUM - 1));
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

struct StateComputed::Impl
{
    Impl(void) : id_(kStateComputed)
    {
    }

    void set(stdsc::StateContext& sc, uint64_t event)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        STDSC_LOG_TRACE("StateComputed(%lu): event#%lu", id_, event);
        switch (static_cast<Event_t>(event))
        {
            case kEventPubKeyStore:
                sc.next_state(StateReady::create());
                break;
            case kEventStoreRequestA:
                sc.next_state(StateReady::create());
                break;
            case kEventStoreRequestB:
                sc.next_state(StateReady::create());
                break;
            case kEventJoinRequest:
                sc.next_state(StateReady::create());
                break;
            case kEventDisconnectSocket:
                sc.next_state(StateInit::create(MAX_CLIENT_NUM - 1));
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

std::shared_ptr<stdsc::State> StateInit::create(size_t initial_connection_count)
{
    auto s = std::shared_ptr<stdsc::State>(new StateInit(initial_connection_count));
    return s;
}

StateInit::StateInit(size_t initial_connection_count) : pimpl_(new Impl(initial_connection_count))
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
                                                     bool is_stored_dataA,
                                                     bool is_stored_dataB)
{
    auto s = std::shared_ptr<stdsc::State>(
      new StateConnected(is_stored_pubkey, is_stored_dataA, is_stored_dataB));
    return s;
}

StateConnected::StateConnected(bool is_stored_pubkey, bool is_stored_dataA,
                               bool is_stored_dataB)
  : pimpl_(new Impl(is_stored_pubkey, is_stored_dataA, is_stored_dataB))
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

// Computed

std::shared_ptr<stdsc::State> StateComputed::create()
{
    auto s = std::shared_ptr<stdsc::State>(new StateComputed());
    return s;
}

StateComputed::StateComputed(void) : pimpl_(new Impl())
{
}

void StateComputed::set(stdsc::StateContext& sc, uint64_t event)
{
    pimpl_->set(sc, event);
}

uint64_t StateComputed::id(void) const
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

} /* opsica_cloud */
