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

#ifndef ZIPPYLOG_ZIPPYLOGD_STREAMER_HPP_
#define ZIPPYLOG_ZIPPYLOGD_STREAMER_HPP_

#include <zippylog/zippylog.h>

#include <zippylog/lua.hpp>
#include <zippylog/platform.hpp>
#include <zippylog/store.hpp>
#include <zippylog/stream.hpp>

#include <zmq.hpp>

#include <map>
#include <string>
#include <vector>

namespace zippylog {
namespace zippylogd {

class EnvelopeSubscription {
public:
    EnvelopeSubscription();

    ::std::string id;
    vector<string> paths;
    vector<string> socket_identifiers;
};

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

    ::std::vector<string> paths;
    ::std::vector<string> socket_identifiers;

    EnvelopeSubscription envelope_subscription;

    ::zippylog::lua::LuaState *l;

private:
    SubscriptionInfo(const SubscriptionInfo &orig);
    SubscriptionInfo & operator=(const SubscriptionInfo &orig);
};

// type passed to constructor to initialize a streamer instance
// it is easier to pass this object than to pass many named arguments
class ZIPPYLOG_EXPORT StreamerStartParams {
public:
    StreamerStartParams();

    ::zmq::context_t *ctx;
    ::zippylog::Store *store;
    ::std::string store_changes_endpoint;
    ::std::string client_endpoint;
    ::std::string subscriptions_endpoint;
    ::std::string subscription_updates_endpoint;
    ::std::string logging_endpoint;
    uint32 subscription_ttl;

    bool lua_allow;
    uint32 lua_max_memory;

    bool *active;
};

// the streamer streams information to subscribed clients
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

        ::std::map<::std::string, SubscriptionInfo *> subscriptions;

        // maps read offsets in streams, for envelope streaming
        ::std::map<::std::string, uint64> stream_read_offsets;

        bool * active;

        void ProcessStoreChangeEnvelope(Envelope &e);

        void ProcessSubscribeStoreChanges(Envelope &e, vector<string> &identities, vector<message_t *> &msgs);
        void ProcessSubscribeEnvelopes(Envelope &e, vector<string> &identities, vector<message_t *> &msgs);

        void SendSubscriptionAck(const string &id, Envelope &req, vector<string> &identities);

        // returns whether we have a subscription for envelopes in the given stream path
        bool HaveEnvelopeSubscription(const string &path);

        // whether we have any store change subscriptions
        bool HaveStoreChangeSubscriptions();

        // returns whether we have store change subscriptions for the given path
        bool HaveStoreChangeSubscriptions(const string &path);

    private:
        Streamer(const Streamer &orig);
        Streamer & operator=(const Streamer &orig);
};

}} // namespaces

#endif