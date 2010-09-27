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

#include <zippylog/store.hpp>

#include <zmq.hpp>

#include <map>
#include <string>
#include <vector>

namespace zippylog {
namespace server {

using ::std::map;
using ::std::string;
using ::std::vector;
using ::zippylog::Envelope;
using ::zippylog::Store;
using ::zmq::context_t;
using ::zmq::socket_t;

// represents a streaming subscription for store changes
class StoreChangeSubscription {
public:
    StoreChangeSubscription();

    string id;
    vector<string> paths;
    vector<string> socket_identifiers;
};

class SubscriptionInfo {
public:
    SubscriptionInfo();

};

// the streamer streams information to subscribed clients
class ZIPPYLOG_EXPORT Streamer {
    public:
        Streamer(
            Store *store, context_t *zctx,
            const string store_changes_endpoint,
            const string client_endpoint,
            const string subscriptions_endpoint,
            const string subscription_updates_endpoint,
            uint32 subscription_ttl
        );
        ~Streamer();

        void Run();

        // sets the shutdown semaphore for the streamer
        // if the boolean pointed to by the parameter goes to 0, Run() will
        // finish, likely shutting down the calling thread
        void SetShutdownSemaphore(bool *active);

    protected:
        Store * store;
        context_t * zctx;

        string store_changes_endpoint;
        string client_endpoint;
        string subscriptions_endpoint;
        string subscription_updates_endpoint;

        uint32 subscription_ttl;

        socket_t * changes_sock;
        socket_t * client_sock;
        socket_t * subscriptions_sock;
        socket_t * subscription_updates_sock;

        vector<StoreChangeSubscription> store_change_subscriptions;

        map<string, SubscriptionInfo> subscriptions;

        bool * active;

        void ProcessStoreChangeEnvelope(Envelope &e);
};

}} // namespaces

#endif