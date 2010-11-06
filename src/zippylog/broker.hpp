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

#include <zippylog/platform.hpp>
#include <zippylog/request_processor.hpp>
#include <zippylog/store.hpp>
#include <zippylog/store_watcher.hpp>
#include <zippylog/streamer.hpp>

#include <vector>
#include <zmq.hpp>

namespace zippylog {
namespace server {

using ::zippylog::platform::Thread;
using ::zippylog::Store;
using ::std::string;
using ::std::vector;
using ::zmq::context_t;
using ::zmq::socket_t;

class BrokerConfig {
public:
    BrokerConfig();

    string store_path;
    vector<string> listen_endpoints;
    uint32 worker_threads;
    uint32 streaming_threads;
    uint32 subscription_ttl;
    string log_bucket;
    string log_stream_set;
    int32 stream_flush_interval;
    bool lua_execute_client_code;       // whether client-supplied Lua code can be executed
    uint32 lua_streaming_max_memory;    // max memory size of Lua interpreters attached to streaming
};

// the broker is a ZMQ device that provides the core message routing component
// of zippylogd. it binds to a number of sockets and coordinates all the workers
// in the system
// if there was a zippylogd class, this would be it
class ZIPPYLOG_EXPORT Broker {
    public:
        Broker(const string config_file_path);
        ~Broker();

        void run();

        // runs the broker asynchronously
        // this creates a new thread, runs the broker in that, then returns
        void RunAsync();

        void Shutdown();

    protected:
        context_t zctx;

        // fans XREQ that fans out to individual worker threads
        socket_t * workers_sock;

        // binds to listen for client requests on configured interfaces
        socket_t * clients_sock;

        // XREP that receives all streamed envelopes to be sent to clients
        socket_t * streaming_sock;

        // PULL that receives processed client subscription requests
        // messages delivered to one random streamer
        socket_t * worker_subscriptions_sock;

        // PUSH that sends client subscription requests to streamers
        socket_t * streaming_subscriptions_sock;

        // PULL that receives processed client streaming messages
        // messages forwarded to all streamers
        socket_t * worker_streaming_notify_sock;

        // PUB that sends processed client streaming messages to all streamers
        socket_t * streaming_streaming_notify_sock;

        // PULL that receives logging messages from other threads
        socket_t * logger_sock;

        // PUSH that sends logging messages to main logging sock
        // yes, we have both a client and server in the same object. this is easier
        socket_t * log_client_sock;

        string id;
        Thread * exec_thread;
        vector<Thread *> worker_threads;
        vector<Thread *> streaming_threads;
        Store * store;
        bool active;
        BrokerConfig config;
        Thread * store_watcher_thread;

        RequestProcessorStartParams request_processor_params;
        StreamerStartParams streamer_params;
        StoreWatcherStartParams store_watcher_params;

        static const string WORKER_ENDPOINT;
        static const string STORE_CHANGE_ENDPOINT;
        static const string STREAMING_ENDPOINT;
        static const string LOGGER_ENDPOINT;

        static const string WORKER_SUBSCRIPTIONS_ENDPOINT;
        static const string STREAMING_SUBSCRIPTIONS_ENDPOINT;
        static const string WORKER_STREAMING_NOTIFY_ENDPOINT;
        static const string STREAMING_STREAMING_NOTIFY_ENDPOINT;

        static bool ParseConfig(const string path, BrokerConfig &config, string &error);

        // thread start functions
        static void * StoreWatcherStart(void *data);
        static void * StreamingStart(void *data);
        static void * AsyncExecStart(void *data);
        static void * RequestProcessorStart(void *data);

        void init();
        void create_worker_threads();
        void create_store_watcher();
        void create_streaming_threads();
        void setup_internal_sockets();
        void setup_listener_sockets();
    private:
        // copy constructor and assignment operator are not supported
        Broker(const Broker &orig);
        Broker & operator=(const Broker &orig);

};

}} // namespaces

#endif