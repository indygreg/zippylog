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

#ifndef ZIPPYLOG_SERVER_STREAMER_HPP_
#define ZIPPYLOG_SERVER_STREAMER_HPP_

#include <zippylog/zippylog.h>

#include <zippylog/lua.hpp>
#include <zippylog/platform.hpp>
#include <zippylog/store.hpp>
#include <zippylog/stream.hpp>

#include <zmq.hpp>

extern "C" {
#include <lua.h>
#include <lauxlib.h>
}

#include <map>
#include <string>
#include <vector>

namespace zippylog {
namespace server {

using ::std::map;
using ::std::string;
using ::std::vector;
using ::zippylog::Envelope;
using ::zippylog::lua::LuaState;
using ::zippylog::Store;
using ::zippylog::platform::Timer;
using ::zmq::context_t;
using ::zmq::socket_t;

class EnvelopeSubscription {
public:
    EnvelopeSubscription();

    string id;
    vector<string> paths;
    vector<string> socket_identifiers;
};

class SubscriptionInfo {
public:
    SubscriptionInfo();
    SubscriptionInfo(uint32 expiration_ttl);
    ~SubscriptionInfo();
    SubscriptionInfo(const SubscriptionInfo &orig);
    SubscriptionInfo & operator=(const SubscriptionInfo &orig);

    Timer expiration_timer;

    enum SubscriptionType {
        ENVELOPE = 1,
        STORE_CHANGE = 2,
    } type;

    vector<string> paths;
    vector<string> socket_identifiers;

    EnvelopeSubscription envelope_subscription;

    LuaState *l;
};

// type passed to constructor to initialize a streamer instance
// it is easier to pass this object than to pass many named arguments
class ZIPPYLOG_EXPORT StreamerStartParams {
public:
    StreamerStartParams();

    context_t *ctx;
    Store *store;
    string store_changes_endpoint;
    string client_endpoint;
    string subscriptions_endpoint;
    string subscription_updates_endpoint;
    string logging_endpoint;
    uint32 subscription_ttl;

    bool lua_allow;
    uint32 lua_max_memory;

    bool *active;
};

// the streamer streams information to subscribed clients
class ZIPPYLOG_EXPORT Streamer {
    public:
        Streamer(StreamerStartParams params);
        Streamer(const Streamer &orig);
        Streamer & operator=(const Streamer &orig);
        ~Streamer();

        void Run();

        // returns whether the streamer knows of a subscription with the specified id
        bool HasSubscription(const string &id);

        // renews a subscription for the specified id
        bool RenewSubscription(const string &id);

    protected:

        Store * store;
        context_t * zctx;
        string id;

        string store_changes_endpoint;
        string client_endpoint;
        string subscriptions_endpoint;
        string subscription_updates_endpoint;
        string logging_endpoint;

        uint32 subscription_ttl;

        bool lua_allow;
        uint32 lua_max_memory;

        socket_t * changes_sock;
        socket_t * client_sock;
        socket_t * subscriptions_sock;
        socket_t * subscription_updates_sock;
        socket_t * logging_sock;

        map<string, SubscriptionInfo *> subscriptions;

        // maps read offsets in streams, for envelope streaming
        map<string, uint64> stream_read_offsets;

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
};

}} // namespaces

#endif