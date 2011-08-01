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
#include <zippylog/lua.hpp>
#include <zippylog/platform.hpp>
#include <zippylog/request_processor.hpp>
#include <zippylog/store.hpp>

#include <map>
#include <string>
#include <vector>

namespace zippylog {

/// Callback used for individual subscription notification of a path added
typedef void (PersistedStateManagerPathAddedCallback)(SubscriptionInfo const &, ::std::string const &, void *);

/// Callback for individual subscription notification of a path deleted
typedef void (PersistedStateManagerPathDeletedCallback)(SubscriptionInfo const &, ::std::string const &, void *);

typedef void (PersistedStateManagerStreamAppendedCallback)(SubscriptionInfo const &, EnvelopeSubscriptionResponseState &, void *);

class PersistedStateManagerSubscriptionRecord
{
public:
    PersistedStateManagerSubscriptionRecord();

    PersistedStateManagerSubscriptionRecord(SubscriptionInfo const &subscription, uint32 ttl);
    ~PersistedStateManagerSubscriptionRecord();

protected:
    SubscriptionInfo si;
    platform::Timer expiration_timer;
    lua::LuaState *l;

    friend class PersistedStateManager;
};

/// Represents info about a plugin
/// @todo Implement
class ZIPPYLOG_EXPORT PluginInfo
{

};

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

/// Manages persisted state in a server
///
/// Logic for maintaining subscriptions and plugins and running observed
/// events through these is contained within this class. No other class in
/// the code base should be directly involved in managing subscriptions or
/// plugins.
///
/// The class API will evolve as the zippylog protocol evolves, as there is
/// a strong cohesion between the two.
///
/// Instances of this class are not thread safe. Instances of this class are
/// not in contention with one another. It is recommended to drive an
/// instance of this class from one thread. If you need parallelization of
/// the features provided by this class, one can instantiate multiple
/// instances and distribute subscriptions and plugins acorss instances.
///
/// @todo Add API to retrieve state metadata
class ZIPPYLOG_EXPORT PersistedStateManager {
public:
    /// Construct an instance from parameters
    PersistedStateManager(PersistedStateManagerStartParams const &params);

    ~PersistedStateManager();

    /// Whether we have a subscription with the specified id
    bool HasSubscription(::std::string const &id) const;

    /// Whether we have any store change subscriptions
    bool HaveStoreChangeSubscriptions() const;

    /// Whether we have store change subscriptions for the given path
    bool HaveStoreChangeSubscriptions(::std::string const &path) const;

    /// Whether we have a subscription for envelopes in the given stream path
    bool HaveEnvelopeSubscription(::std::string const &path) const;

    /// Renews a subscription for the specified id
    ///
    /// This is typically called when a server receives a subscription
    /// keepalive request.
    bool RenewSubscription(::std::string const &id);

    /// Renews multiple subscriptions from ids
    bool RenewSubscriptions(::std::vector < ::std::string >  const & ids);

    /// Registers a new subscription from a subscription record
    ///
    /// Ownership of the memory is transferred to the manager. The lifetime
    /// of the object is undefined, so callers should not attempt to access it
    /// after it is transferred to the manager.
    ///
    /// This function will start the subscription TTL timer immediately.
    ///
    /// @todo formalize behavior for duplicate subscriptions
    void RegisterSubscription(SubscriptionInfo const &subscription);

    /// Unregister a subscription with the id specified
    void UnregisterSubscription(::std::string const &id);

    /// Removes expired subscriptions from the manager
    int32 RemoveExpiredSubscriptions();

    /// Registers a plugin from the specified plugin record
    ///
    /// Plugins are effectively a set of persisted hooks. They live and
    /// execute until they are unloaded or encounter a serious error.
    void RegisterPlugin(PluginInfo const &plugin);

    /// Unregister the plugin with the specified id
    void UnregisterPlugin(::std::string const &id);

    /// Processes a path added event
    ///
    /// This is called when a bucket, stream set, or path has been added to
    /// the store.
    ///
    /// The function examines existing subscriptions. If anyone is subscribed,
    /// the passed callback will be invoked, receiving details of the
    /// subscription.
    void ProcessStoreChangePathAdded(::std::string const &path,
                                     PersistedStateManagerPathAddedCallback *cb,
                                     void *data = NULL);

    /// Processes a path deleted event
    ///
    /// This is very similar to ProcessStoreChangePathAdded(). All the
    /// documentation for that method applies.
    void ProcessStoreChangePathDeleted(::std::string const &path,
                                       PersistedStateManagerPathDeletedCallback *cb,
                                       void *data = NULL);

    /// Processes a stream append event
    ///
    /// This should be called when a stream has been appended to.
    ///
    /// The function examines all subscriptions to envelopes. For those
    /// subscribed to this path, the stream is read from the last read point
    /// up to the new stream length. Read envelopes are fed into Lua for
    /// filtering (if configured).
    ///
    /// For each envelope to be sent to the client, the supplied callback is
    /// invoked. The callback receives a EnvelopeSubscriptionResponseState
    /// instance. This will likely be immediately passed to
    /// RequestProcessor::SendSubscriptionEnvelopeResponse(), which will
    /// handle response formulation.
    ///
    /// @todo reduce number of times callback is called (tons of call overhead)
    void ProcessStoreChangeStreamAppended(::std::string const &path,
                                          const uint64 stream_length,
                                          PersistedStateManagerStreamAppendedCallback *cb,
                                          void *data = NULL);

protected:
    /// Returns whether a path is subscribed to by a subscription
    static bool IsPathSubscribed(::std::string const &path, PersistedStateManagerSubscriptionRecord const &subscription);

    // params coming from constructor
    ::std::string store_uri;

    uint32 subscription_ttl;

    bool subscription_lua_allow;

    /// Maximum memory (in bytes) a Lua state for a subscription can allocate
    uint32 subscription_lua_memory_ceiling;

    // instance parameters

    Store * store;

    /// Maps subscription id to details about the subscription
    ::std::map< ::std::string, PersistedStateManagerSubscriptionRecord * > subscriptions;

    // maps read offsets in streams. for envelope streaming
    ::std::map< ::std::string, uint64 > stream_read_offsets;

    FRIEND_TEST(PersistedStateManagerTest, IsPathSubscribed);
    FRIEND_TEST(PersistedStateManagerTest, SubscriptionExpiration);
private:
};

} // namespace

#endif // file define