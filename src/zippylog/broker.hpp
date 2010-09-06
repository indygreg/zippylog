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

#ifndef ZIPPYLOG_SERVER_BROKER_HPP_
#define ZIPPYLOG_SERVER_BROKER_HPP_

#include <zippylog/zippylog.h>
#include <zippylog/server.hpp>
#include <zippylog/store.hpp>
#include <zippylog/store_watcher.hpp>

#include <vector>
#include <zmq.hpp>

namespace zippylog {
namespace server {

using ::zippylog::Store;
using ::std::string;
using ::std::vector;
using ::zmq::context_t;
using ::zmq::socket_t;

typedef struct broker_config {
    broker_config();
    string store_path;
    vector<string> listen_endpoints;
    uint32 worker_threads;
} broker_config;

typedef struct store_watcher_start_data {
    context_t *zctx;
    Store *store;
    char *endpoint;
} store_watcher_start_data;

// the broker is a ZMQ device that provides the core message routing component
// of zippylogd. it binds to a number of sockets and coordinates all the workers
// in the system
class ZIPPYLOG_EXPORT Broker {
    public:
        Broker(const string config_file_path);
        Broker(Store *store);
        Broker(Store *store, context_t *ctx);

        ~Broker();

        void run();

    protected:
        context_t * zctx;
        socket_t * workers_sock;
        socket_t * clients_sock;
        vector<void *> worker_threads;
        Store * store;
        bool active;
        request_processor_start_data * worker_start_data;
        broker_config config;
        void * store_watcher_thread;
        store_watcher_start_data * store_watcher_start;

        static bool ParseConfig(const string path, broker_config &config, string &error);
        static void * __stdcall StoreWatcherStart(void *data);

        void init();
        void create_worker_threads();
        void create_store_watcher();
        void setup_internal_sockets();
        void setup_listener_sockets();


};

}} // namespaces

#endif