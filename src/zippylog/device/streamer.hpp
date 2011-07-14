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

#ifndef ZIPPYLOG_DEVICE_STREAMER_HPP_
#define ZIPPYLOG_DEVICE_STREAMER_HPP_

#include <zippylog/zippylog.hpp>

#include <zippylog/lua.hpp>
#include <zippylog/platform.hpp>
#include <zippylog/request_processor.hpp>
#include <zippylog/store.hpp>
#include <zippylog/stream.hpp>

#include <zmq.hpp>

#include <map>
#include <string>
#include <vector>

namespace zippylog {
namespace device {

/// Used to construct a streamer device
class ZIPPYLOG_EXPORT StreamerStartParams {
public:
    StreamerStartParams() : ctx(NULL), active(NULL) { }

    /// 0MQ context to use.
    ///
    /// Must be defined.
    ::zmq::context_t *ctx;

    /// Store URI to use
    ::std::string store_path;

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

    /// Memory (in bytes) a Lua interpreter can grow to
    uint32 lua_max_memory;

    /// Semaphore stating whether streamer should remain active
    bool *active;
};

/// Manages streaming of data to interested parties
///
/// A streamer performs the following actions:
///
///   - Receives client subscriptions for interested events
///   - Receives notifications of changes to a store
///   - Notifies subscribers to subscribed events
///
/// The streamer currently operates by receiving and sending data over a set
/// of 0MQ sockets. The endpoints are controlled by StreamerStartParams.
///
///  - store_changes - SUB socket connected to a store watcher's PUB socket
///  - client        - PUSH socket that sends messages to clients
///  - subscriptions - PULL that receives new subscription requests.
///  - subscription_updates - SUB that receives messages concerning existing
///                           subscriptions
///
/// The streamer, like the RequestProcessor, should probably be abstracted. As
/// it currently stands, it is tightly coupled with the implementation of the
/// server device. We'll get there.
/// @todo make base streamer abstract
class ZIPPYLOG_EXPORT Streamer {
    public:
        Streamer(StreamerStartParams params);
        ~Streamer();

        /// Runs the streamer
        ///
        /// This function will run forever until the boolean pointed to
        /// in its start parameters goes to false.
        void Run();

        /// Returns whether the streamer knows of a subscription with the specified id
        bool HasSubscription(const ::std::string &id);

        /// Renews a subscription for the specified id
        bool RenewSubscription(const ::std::string &id);

    protected:
        /// Processes an envelope received on the subscription updates socket
        bool ProcessSubscriptionUpdate(Envelope &e);

        /// Removes expired subscriptions from the streamer
        ///
        /// Returns the number of subscriptions removed
        int RemoveExpiredSubscriptions();

        bool ProcessStoreChangeMessage(::zmq::message_t &m);

        void ProcessStoreChangeEnvelope(Envelope &e);

        // returns whether we have a subscription for envelopes in the given stream path
        bool HaveEnvelopeSubscription(const ::std::string &path);

        // whether we have any store change subscriptions
        bool HaveStoreChangeSubscriptions();

        // returns whether we have store change subscriptions for the given path
        bool HaveStoreChangeSubscriptions(const ::std::string &path);

        /// Store we are bound to.
        ///
        /// It is assumed the streamer receives events from the same store to
        /// which it is bound. If not, there will likely be large explosions.
        Store * store;

        ::zmq::context_t * zctx;
        ::std::string id;

        ::std::string store_changes_endpoint;
        ::std::string client_endpoint;
        ::std::string subscriptions_endpoint;
        ::std::string subscription_updates_endpoint;
        ::std::string logging_endpoint;

        /// Subscription expiration time, in milliseconds
        uint32 subscription_ttl;

        /// Do we allow subscriptions with Lua?
        bool lua_allow;

        /// Max memory per Lua interpreter
        uint32 lua_max_memory;

        ::zmq::socket_t * changes_sock;
        ::zmq::socket_t * client_sock;
        ::zmq::socket_t * subscriptions_sock;
        ::zmq::socket_t * subscription_updates_sock;
        ::zmq::socket_t * logging_sock;

        /// Maps subscription id to details about the subscription
        ::std::map< ::std::string, ::zippylog::SubscriptionInfo * > subscriptions;

        // maps read offsets in streams, for envelope streaming
        ::std::map< ::std::string, uint64 > stream_read_offsets;

        bool * active;

    private:
        Streamer(const Streamer &orig);
        Streamer & operator=(const Streamer &orig);
};

}} // namespaces

#endif