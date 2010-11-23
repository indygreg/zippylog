//  Copyright 2010 Gregory Szorc
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

#ifndef ZIPPYLOG_DEVICE_STREAMER_HPP_
#define ZIPPYLOG_DEVICE_STREAMER_HPP_

#include <zippylog/zippylog.hpp>

#include <zippylog/lua.hpp>
#include <zippylog/platform.hpp>
#include <zippylog/store.hpp>
#include <zippylog/stream.hpp>

#include <zmq.hpp>

#include <map>
#include <string>
#include <vector>

namespace zippylog {
namespace device {

/// records a subscription to an envelope
class EnvelopeSubscription {
public:
    EnvelopeSubscription();

    ::std::string id;
    ::std::vector< ::std::string > paths;
    ::std::vector< ::std::string > socket_identifiers;
};

/// records details about an individual subscription
class SubscriptionInfo {
public:
    SubscriptionInfo();
    SubscriptionInfo(uint32 expiration_ttl);
    ~SubscriptionInfo();

    ::zippylog::platform::Timer expiration_timer;

    enum SubscriptionType {
        ENVELOPE = 1,
        STORE_CHANGE = 2,
    } type;

    ::std::vector< ::std::string > paths;
    ::std::vector< ::std::string > socket_identifiers;

    EnvelopeSubscription envelope_subscription;

    ::zippylog::lua::LuaState *l;

private:
    SubscriptionInfo(const SubscriptionInfo &orig);
    SubscriptionInfo & operator=(const SubscriptionInfo &orig);
};

/// Used to construct a streamer device
class ZIPPYLOG_EXPORT StreamerStartParams {
public:
    StreamerStartParams();

    ::zmq::context_t *ctx;
    ::zippylog::Store *store;

    /// 0MQ endpoint for SUB socket that receives store changes
    ::std::string store_changes_endpoint;

    /// 0MQ endpoint for PUSH socket that will send messages to clients
    ::std::string client_endpoint;

    /// 0MQ endpoint for PULL socket that will receive subscription messages
    ::std::string subscriptions_endpoint;

    /// 0MQ endpoint for SUB socket that will receive subscription updates
    ::std::string subscription_updates_endpoint;

    /// 0MQ endpoint for PUSH socket that will send logging messages
    ::std::string logging_endpoint;

    /// Default expiration TTL of new subscriptions, in milliseconds
    uint32 subscription_ttl;

    /// Whether to allow Lua code to be executed
    bool lua_allow;

    /// Memory memory a Lua interpreter can grow to
    uint32 lua_max_memory;

    /// Semaphore stating whether streamer should remain active
    bool *active;
};

/// Holds stream subscriptions and notifies interested clients
///
/// The streamer handles the streaming aspect of zippylog. It processes
/// new subscriptions. When it receives details about events in a store, it
/// looks at the registered subscriptions and if any are interested, performs
/// optional data processing and sends messages to interested subscribers.
class ZIPPYLOG_EXPORT Streamer {
    public:
        Streamer(StreamerStartParams params);
        ~Streamer();

        void Run();

        // returns whether the streamer knows of a subscription with the specified id
        bool HasSubscription(const string &id);

        // renews a subscription for the specified id
        bool RenewSubscription(const string &id);

    protected:

        Store * store;
        ::zmq::context_t * zctx;
        ::std::string id;

        ::std::string store_changes_endpoint;
        ::std::string client_endpoint;
        ::std::string subscriptions_endpoint;
        ::std::string subscription_updates_endpoint;
        ::std::string logging_endpoint;

        uint32 subscription_ttl;

        bool lua_allow;
        uint32 lua_max_memory;

        ::zmq::socket_t * changes_sock;
        ::zmq::socket_t * client_sock;
        ::zmq::socket_t * subscriptions_sock;
        ::zmq::socket_t * subscription_updates_sock;
        ::zmq::socket_t * logging_sock;

        ::std::map< ::std::string, SubscriptionInfo * > subscriptions;

        // maps read offsets in streams, for envelope streaming
        ::std::map< ::std::string, uint64 > stream_read_offsets;

        bool * active;

        void ProcessStoreChangeEnvelope(Envelope &e);

        void ProcessSubscribeStoreChanges(Envelope &e, ::std::vector< ::std::string > &identities, ::std::vector<message_t *> &msgs);
        void ProcessSubscribeEnvelopes(Envelope &e, ::std::vector< ::std::string > &identities, ::std::vector<message_t *> &msgs);

        void SendSubscriptionAck(const ::std::string &id, Envelope &req, ::std::vector< ::std::string > &identities);

        // returns whether we have a subscription for envelopes in the given stream path
        bool HaveEnvelopeSubscription(const ::std::string &path);

        // whether we have any store change subscriptions
        bool HaveStoreChangeSubscriptions();

        // returns whether we have store change subscriptions for the given path
        bool HaveStoreChangeSubscriptions(const ::std::string &path);

    private:
        Streamer(const Streamer &orig);
        Streamer & operator=(const Streamer &orig);
};

}} // namespaces

#endif