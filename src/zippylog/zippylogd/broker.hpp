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

#ifndef ZIPPYLOG_ZIPPYLOGD_BROKER_HPP_
#define ZIPPYLOG_ZIPPYLOGD_BROKER_HPP_

#include <zippylog/zippylog.hpp>

#include <zippylog/platform.hpp>
#include <zippylog/store.hpp>
#include <zippylog/store_watcher.hpp>
#include <zippylog/zippylogd/streamer.hpp>
#include <zippylog/zippylogd/watcher.hpp>
#include <zippylog/zippylogd/worker.hpp>

#include <vector>
#include <zmq.hpp>

namespace zippylog {
namespace zippylogd {

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

// The broker is the server class for zippylogd
//
// It has a couple of functions:
//
//   - ZMQ Device - it forwards 0MQ messages to and from the appropriate sockets
//   - Thread Manager - manages threads for request processing, store watching, streaming
//   - Logging coordinator - all process logging (itself using zippylog) flows through this class
//
// SOCKET FLOWS
//
// When a client connects to a configured listening socket, messages will
// be handled as follows:
//
//   client -> <clients_sock> -> <workers_sock> -> worker thread
//
// A worker thread will handle the message in one of the following:
//
//   - It will generate a response itself. It just sends the response
//     back through the workers_sock and it will make its way back to
//     the client.
//   - If a subscription keepalive, will forward the message to
//     worker_streaming_notify_sock. The broker receives messages
//     from all workers and then rebroadcasts the messages to all
//     streamers connected via streaming_streaming_notify_sock.
//   - If a subscription request, will forward the message to
//     worker_subscriptions_sock. The broker receives these messages
//     and sends to one streamer via streaming_subscriptions_sock.
//     The streamer that receives it will likely send a response via
//     streaming_sock and the broker will forward it to the
//     clients_sock.
//
// In the streaming cases, the request response (if there is one) does not
// come back through the workers_sock. This is perfectly fine, as that
// socket is a XREQ socket. This preserves the event-driver architecture
// of the server.
class ZIPPYLOG_EXPORT Broker {
    public:
        // Construct a broker from a Lua config file
        //
        // For a description of what configuration options are read, see
        // ParseConfig()
        Broker(const string config_file_path);

        ~Broker();

        // Run the broker synchronously
        //
        // This will block until a fatal error is encountered or until the
        // Shutdown() function is called.
        void Run();

        // runs the broker asynchronously
        // this creates a new thread, runs the broker in that, then returns
        void RunAsync();

        // Shut down the broker
        //
        // On first call, will trigger the shutdown semaphore which signals all
        // created threads to stop execution. The function call will block
        // until all threads have been joined.
        //
        // On second call, is a no-op.
        // TODO need an API to force shutdown
        void Shutdown();

    protected:
        ::zmq::context_t zctx;

        // fans XREQ that fans out to individual worker threads
        ::zmq::socket_t * workers_sock;

        // binds to listen for client requests on configured interfaces
        ::zmq::socket_t * clients_sock;

        // XREP that receives all streamed envelopes to be sent to clients
        ::zmq::socket_t * streaming_sock;

        // PULL that receives processed client subscription requests
        // messages delivered to one random streamer
        ::zmq::socket_t * worker_subscriptions_sock;

        // PUSH that sends client subscription requests to streamers
        ::zmq::socket_t * streaming_subscriptions_sock;

        // PULL that receives processed client streaming messages
        // messages that need to be forwarded to all streamers
        // we can't send directly from the workers to the streamers
        // because there is potentially a many to many mapping there
        // the broker binds to both endpoints and distributes messages
        // properly
        ::zmq::socket_t * worker_streaming_notify_sock;

        // PUB that sends processed client streaming messages to all streamers
        ::zmq::socket_t * streaming_streaming_notify_sock;

        // PULL that receives logging messages from other threads
        ::zmq::socket_t * logger_sock;

        // PUSH that sends logging messages to main logging sock
        // yes, we have both a client and server in the same object. this is easier
        ::zmq::socket_t * log_client_sock;

        string id;
        ::zippylog::platform::Thread * exec_thread;
        vector< ::zippylog::platform::Thread * > worker_threads;
        vector< ::zippylog::platform::Thread * > streaming_threads;
        ::zippylog::Store * store;
        bool active;
        BrokerConfig config;
        ::zippylog::platform::Thread * store_watcher_thread;

        ::zippylog::zippylogd::WorkerStartParams request_processor_params;
        ::zippylog::zippylogd::StreamerStartParams streamer_params;
        ::zippylog::zippylogd::WatcherStartParams store_watcher_params;

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