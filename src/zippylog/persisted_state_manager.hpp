//  Copyright 2011 Gregory Szorc
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.

#ifndef ZIPPYLOG_PERSISTED_STATE_MANAGER_HPP_
#define ZIPPYLOG_PERSISTED_STATE_MANAGER_HPP_

#include <zippylog/zippylog.hpp>
#include <zippylog/request_processor.hpp>
#include <zippylog/store.hpp>

#include <map>
#include <string>

namespace zippylog {

/// Constructor arguments for PersistedStateManager
class ZIPPYLOG_EXPORT PersistedStateManagerStartParams {
public:
    PersistedStateManagerStartParams() :
        subscription_ttl(::zippylog::server_default_subscription_ttl),
        subscription_lua_allow(::zippylog::server_default_lua_allow),
        subscription_lua_memory_ceiling(lua_default_subscription_memory_ceiling)
    { }

    /// URI of store to bind to
    ::std::string store_uri;

    /// Time to live, in milliseconds, of subscriptions
    uint32 subscription_ttl;

    /// Whether to allow Lua on subscriptions
    bool subscription_lua_allow;

    uint32 subscription_lua_memory_ceiling;
};

/// Handles events relevant to persisted state
///
/// Logic for maintaining subscriptions and plugins and running observed
/// events through these is contained within this class.
class ZIPPYLOG_EXPORT PersistedStateManager {
public:
    PersistedStateManager(const PersistedStateManagerStartParams &params);

    ~PersistedStateManager();

    /// Whether we have a subscription with the specified id
    bool HasSubscription(const ::std::string &id);

    /// Renews a subscription for the specified id
    bool RenewSubscription(const ::std::string &id);

    /// Registers a new subscription from a subscription record
    ///
    /// Ownership of the memory is transferred to the manager
    void RegisterSubscription(SubscriptionInfo *subscription);

    /// Removes expired subscriptions from the manager
    int32 RemoveExpiredSubscriptions();

protected:

    /// Whether we have any store change subscriptions
    bool HaveStoreChangeSubscriptions();

    /// Whether we have store change subscriptions for the given path
    bool HaveStoreChangeSubscriptions(const ::std::string &path);

    /// Whether we have a subscription for envelopes in the given stream path
    bool HaveEnvelopeSubscription(const ::std::string &path);

    // params coming from constructor
    ::std::string store_uri;

    uint32 subscription_ttl;

    bool subscription_lua_allow;

    /// Maximum memory (in bytes) a Lua state for a subscription can allocate
    uint32 subscription_lua_memory_ceiling;

    // instance parameters

    Store * store;

    /// Maps subscription id to details about the subscription
    ::std::map< ::std::string, SubscriptionInfo * > subscriptions;

    // maps read offsets in streams. for envelope streaming
    ::std::map< ::std::string, uint64 > stream_read_offsets;

private:
};

} // namespace

#endif // file define