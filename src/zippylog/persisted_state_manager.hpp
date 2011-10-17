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
typedef void (PersistedStateManagerPathAddedCallback)(
    SubscriptionInfo const &,
    ::std::string const &,
    void *);

/// Callback for individual subscription notification of a path deleted
typedef void (PersistedStateManagerPathDeletedCallback)(
    SubscriptionInfo const &,
    ::std::string const &,
    void *);

/// Callback for individual subscription notification of a stream append
typedef void (PersistedStateManagerStreamAppendedCallback)(
    SubscriptionInfo const &,
    EnvelopeSubscriptionResponseState &,
    void *);

/// An individual subscription inside a subscription manager
///
/// This class is used internally by PersistedStateManager and should not be
/// used elsewhere.
class PersistedStateManagerSubscriptionRecord {
public:
    ~PersistedStateManagerSubscriptionRecord();

protected:
    /// Construct a new, empty record
    PersistedStateManagerSubscriptionRecord();

    /// Construct a record from subscription info
    ///
    /// @param subscription Subscription to create record from
    /// @param ttl TTL of subscription, in milliseconds
    PersistedStateManagerSubscriptionRecord(SubscriptionInfo const &subscription, uint32 ttl);

    /// The original subscription record
    SubscriptionInfo si;

    /// Timer to keep track of TTL
    platform::Timer expiration_timer;

    /// Lua state attached to subscription
    lua::LuaState *l;

    /// Give PersistedStateManager rights to internals
    friend class PersistedStateManager;

    /// Give tests rights to internals
    FRIEND_TEST(PersistedStateManagerTest, IsPathSubscribed);

private:
    ///@{
    /// Disable copy constructor and assignment operator
    PersistedStateManagerSubscriptionRecord(PersistedStateManagerSubscriptionRecord const &orig);
    PersistedStateManagerSubscriptionRecord & operator=(PersistedStateManagerSubscriptionRecord const &orig);
    ///@}
};

/// Represents info about a plugin
/// @todo Implement
class ZIPPYLOG_EXPORT PluginInfo {

};

/// Constructor arguments for PersistedStateManager
class ZIPPYLOG_EXPORT PersistedStateManagerStartParams {
public:
    PersistedStateManagerStartParams() :
        subscription_ttl(::zippylog::server_default_subscription_ttl),
        subscription_lua_allow(::zippylog::server_default_lua_allow),
        subscription_lua_memory_ceiling(lua_default_subscription_memory_ceiling),
        /// @todo these should come from constants elsewhere
        plugin_allow(true),
        plugin_lua_memory_ceiling(128000)
    { }

    /// URI of store to bind to
    ::std::string store_uri;

    /// Time to live, in milliseconds, of subscriptions
    uint32 subscription_ttl;

    /// Whether to allow Lua on subscriptions
    bool subscription_lua_allow;

    /// Lua memory ceiling for subscriptions, in bytes
    uint32 subscription_lua_memory_ceiling;

    /// Whether plugins are allowed
    bool plugin_allow;

    /// Lua memory ceiling for plugins, in bytes
    uint32 plugin_lua_memory_ceiling;
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
/// instances and distribute subscriptions and plugins across instances.
///
/// The general usage flow of this class is to instantiate an instance, add
/// plugins and/or subscriptions to it, then call the various Process*
/// functions after an event of interest has occurred. These functions will
/// invoke the necessary functions in the subscriptions or plugins. If
/// additional action is required, the callback supplied to the function will
/// be invoked with the necessary metadata. Then, the callback does its thing.
/// Eventually, all the plugins and subscriptions are processed, and the
/// function returns.
///
/// @todo Add API to retrieve state metadata
class ZIPPYLOG_EXPORT PersistedStateManager {
public:
    /// Construct an instance from parameters
    PersistedStateManager(PersistedStateManagerStartParams const &params);

    ~PersistedStateManager();

    /// Whether we have a subscription with the specified id
    ///
    /// @param id Subscription ID to check presence of
    /// @return true if subscription with specified is present, false otherwise
    bool HasSubscription(::std::string const &id) const;

    /// Whether we have any store change subscriptions
    ///
    /// @return true if there are any store change subscriptions registered
    bool HaveStoreChangeSubscriptions() const;

    /// Whether we have store change subscriptions for the given path
    ///
    /// @param path Path to check for store subscriptions to
    /// @return true if there are path subsriptions to the path specified
    bool HaveStoreChangeSubscriptions(::std::string const &path) const;

    /// Whether we have a subscription for envelopes in the given stream path
    ///
    /// @param path Path to check for envelope subscriptions to
    /// @return true if there are envelope subscriptions for the path specified
    bool HaveEnvelopeSubscription(::std::string const &path) const;

    /// Renews a subscription for the specified id
    ///
    /// This is typically called when a server receives a subscription
    /// keepalive request.
    ///
    /// @param id Subscription to renew
    /// @return true if the subscription was renewed successfully
    bool RenewSubscription(::std::string const &id);

    /// Renews multiple subscriptions from ids
    ///
    /// @param ids Set of subscriptions to renew
    /// @return true if all the subscriptions were renewed sucessfully
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
    /// @param subscription Subscription record to register
    void RegisterSubscription(SubscriptionInfo const &subscription);

    /// Unregister a subscription with the id specified
    ///
    /// @param id Identifier of subscription to remove
    void UnregisterSubscription(::std::string const &id);

    /// Removes expired subscriptions from the manager
    ///
    /// @return The number of subscriptions removed
    int32 RemoveExpiredSubscriptions();

    /// Registers a plugin from the specified plugin record
    ///
    /// Plugins are effectively a set of persisted hooks. They live and
    /// execute until they are unloaded or encounter a serious error.
    ///
    /// @param plugin Plugin to register
    void RegisterPlugin(PluginInfo const &plugin);

    /// Unregister the plugin with the specified id
    ///
    /// @param id Identifier of plugin to remove
    void UnregisterPlugin(::std::string const &id);

    /// Processes a path added event
    ///
    /// This is called when a bucket, stream set, or path has been added to
    /// the store.
    ///
    /// The function examines existing subscriptions. If anyone is subscribed,
    /// the passed callback will be invoked, receiving details of the
    /// subscription.
    ///
    /// @param path The path to process changes for
    /// @param cb The callback to be invoked
    /// @param data User data to pass to callback
    void ProcessStoreChangePathAdded(::std::string const &path,
                                     PersistedStateManagerPathAddedCallback *cb,
                                     void *data = NULL);

    /// Processes a path deleted event
    ///
    /// This is very similar to ProcessStoreChangePathAdded(). All the
    /// documentation for that method applies.
    ///
    /// @param path The path to process a deletion of
    /// @param cb The callback to be invoked
    /// @param data User data to pass to callback
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
    /// @param path Path to process stream append for
    /// @param stream_length New length of stream referenced by path
    /// @param cb Callback to be invoked with change information
    /// @param data User data to pass to callback
    void ProcessStoreChangeStreamAppended(::std::string const &path,
                                          const uint64 stream_length,
                                          PersistedStateManagerStreamAppendedCallback *cb,
                                          void *data = NULL);

protected:
    /// Returns whether a path is subscribed to by a subscription
    static bool IsPathSubscribed(::std::string const &path, PersistedStateManagerSubscriptionRecord const &subscription);

    /// URI of store to bind to
    ::std::string store_uri;

    /// TTL of subscriptions, in milliseconds
    uint32 subscription_ttl;

    /// Whether to allow Lua execution in subscriptions
    bool subscription_lua_allow;

    /// Maximum memory (in bytes) a Lua state for a subscription can allocate
    uint32 subscription_lua_memory_ceiling;

    // instance parameters

    /// Store we are bound to
    Store * store;

    /// Maps subscription id to details about the subscription
    ::std::map< ::std::string, PersistedStateManagerSubscriptionRecord * > subscriptions;

    /// Maps read offsets in streams. for envelope streaming
    ::std::map< ::std::string, uint64 > stream_read_offsets;

    ///@{
    /// Allow tests to poke at internals
    FRIEND_TEST(PersistedStateManagerTest, IsPathSubscribed);
    FRIEND_TEST(PersistedStateManagerTest, SubscriptionExpiration);
    ///@}
private:

    ///@{
    /// Disable copy constructor and assignment operator
    PersistedStateManager(PersistedStateManager const &orig);
    PersistedStateManager & operator=(PersistedStateManager const &orig);
    ///@}
};

} // namespace

#endif // file define
