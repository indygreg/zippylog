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

#ifndef ZIPPYLOG_DEVICE_SERVER_HPP_
#define ZIPPYLOG_DEVICE_SERVER_HPP_

#include <zippylog/zippylog.hpp>

#include <zippylog/platform.hpp>
#include <zippylog/request_processor.hpp>
#include <zippylog/store.hpp>
#include <zippylog/store_watcher.hpp>
#include <zippylog/device/device.hpp>
#include <zippylog/device/persisted_state_reactor.hpp>
#include <zippylog/device/store_writer.hpp>
#include <zippylog/device/store_writer_sender.hpp>

#include <vector>
#include <zmq.hpp>

namespace zippylog {
namespace device {

/// contains classes used by server device
namespace server {

/// Used to construct a ServerRequestProcessor
class ServerRequestProcessorStartParams {
public:
    // where to send client subscription messages
    ::std::string streaming_subscriptions_endpoint;

    // where to send updates for existing subscriptions
    ::std::string streaming_updates_endpoint;

    /// 0MQ endpoint for store writer's envelope PULL socket
    ::std::string store_writer_envelope_pull_endpoint;

    /// 0MQ endpoint for store writer's envelope REP socket
    ::std::string store_writer_envelope_rep_endpoint;

    ::zippylog::RequestProcessorStartParams request_processor_params;
};

/// RequestProcessorImplementation for the server device
///
/// When this request processor receives a request related to streaming, it
/// forwards it to the stream processors, via 1 of 2 sockets. The built-in
/// subscriptions sock load balances among all active streamers in the
/// server device. The subscription updates sock fans out to all instances.
/// The former is used when a message only needs to go to 1 streamer and the
/// latter when all streamers need to see it (e.g. a keepalive message since
/// the server doesn't know which streamers have which subscriptions).
class ServerRequestProcessor : public ::zippylog::RequestProcessorImplementation {
    public:
        ServerRequestProcessor(ServerRequestProcessorStartParams &params);
        ~ServerRequestProcessor();

        RequestProcessorHandlerResult HandleSubscriptionRequest(
            SubscriptionInfo subscription);

        RequestProcessorHandlerResult HandleSubscribeKeepalive(
            Envelope &request,
            ::std::vector<Envelope> &output);

        RequestProcessorHandlerResult HandleWriteEnvelopes(
            ::std::string const &path,
            ::std::vector<Envelope> &to_write,
            bool synchronous);

        RequestProcessorHandlerResult HandleRegisterPlugin(
            PluginRegistrationRequest const &r);

        RequestProcessorHandlerResult HandleUnregisterPlugin(
            ::std::string const &name);

        RequestProcessorHandlerResult HandleGetPluginStatus(
            ::std::vector< ::std::string > const &names);

    protected:
        ::zmq::context_t *ctx;

        ::std::string streaming_subscriptions_endpoint;
        ::std::string streaming_updates_endpoint;

        ::zmq::socket_t *subscriptions_sock;
        ::zmq::socket_t *subscription_updates_sock;

        ::zippylog::device::StoreWriterSender * store_sender;
};

/// Create store watchers tailored for the server device
class WatcherStartParams {
public:
    ::zippylog::StoreWatcherStartParams params;

    // 0MQ socket endpoint on which to connect a PUSH socket
    ::std::string socket_endpoint;
};

/// Store watcher implementation for the server device
///
/// Whenever changes are seen, forwards store change events on a 0MQ PUSH socket
/// whose endpoint is defined at construction time.
class Watcher : public ::zippylog::StoreWatcher {
public:
    // Construct a watcher that sends events to a 0MQ PUSH socket
    Watcher(WatcherStartParams &params);
    ~Watcher();

protected:
    // implement the interface
    void HandleAdded(::std::string path, platform::FileStat &stat);
    void HandleDeleted(::std::string path);
    void HandleModified(::std::string path, platform::FileStat &stat);

    // sends the change to all interested parties
    void SendChangeMessage(Envelope &e);

    ::std::string endpoint;
    ::zmq::socket_t * socket;

private:
    Watcher(Watcher const &orig);
    Watcher & operator=(Watcher const &orig);
};

} // end of server namespace

/// Holds the config for a server device
///
/// Typically this is populated by parsing a Lua file. However, it could also
/// be created manually and passed into a server's constructor.
class ZIPPYLOG_EXPORT ServerStartParams {
public:
    ServerStartParams() :
        active(NULL),
        ctx(NULL),
        worker_threads(::zippylog::server_default_worker_threads),
        persisted_state_reactor_threads(::zippylog::server_default_persisted_state_reactor_threads),
        subscription_ttl(::zippylog::server_default_subscription_ttl),
        log_bucket(::zippylog::server_default_log_bucket),
        log_stream_set(::zippylog::server_default_log_stream_set),
        stream_flush_interval(::zippylog::server_default_stream_flush_interval),
        lua_execute_client_code(::zippylog::server_default_lua_allow),
        lua_streaming_max_memory(::zippylog::server_default_lua_streaming_max_memory)
    { }

    /// Semaphore used to control if device should keep running
    ::zippylog::platform::ConditionalWait *active;

    /// 0MQ context to use
    ///
    /// If not defined, a new context will be created automatically
    ::zmq::context_t *ctx;

    /// the path to the store the server operates against
    ::std::string store_path;

    /// 0MQ endpoints to bind XREP sockets to listen for client messages
    ::std::vector< ::std::string > listen_endpoints;

    /// The number of worker threads to run
    uint32 worker_threads;

    /// The number of persisted state reactor threads to run
    uint32 persisted_state_reactor_threads;

    /// The default subscription expiration TTL, in milliseconds
    uint32 subscription_ttl;

    /// Bucket to log server's own log messages to
    ::std::string log_bucket;

    /// Stream set to log server's own log messages to
    ::std::string log_stream_set;

    /// How often to flush written streams, in milliseconds
    int32 stream_flush_interval;

    /// whether client-supplied Lua code can be executed
    bool lua_execute_client_code;

    /// max memory size of Lua interpreters attached to streaming in bytes
    uint32 lua_streaming_max_memory;
};

/// The server is an uber device that provides server functionality
///
/// It has the following functions:
///
///   - ZMQ Device - it forwards 0MQ messages to and from the appropriate sockets
///   - Thread Manager - manages threads for request processing, store watching, streaming
///   - Logging coordinator - all process logging (itself using zippylog) flows through this class
///
/// The server device maintains the following:
///
///   - A thread pool of RequestProcessor instances that service incoming requests
///   - A thread pool of PeristedStateManager instances that react to events
///   - A store watcher that notices changes to the store and notifies
///     interested parties
///   - Logging sink for all attached components
///
/// SOCKET FLOWS
///
/// When a client connects to a configured listening socket, messages will
/// be handled as follows:
///
///   client -> <clients_sock> -> <workers_sock> -> worker thread
///
/// A worker thread will handle the message in one of the following:
///
///   - It will generate a response itself. It just sends the response
///     back through the workers_sock and it will make its way back to
///     the client.
///   - If a subscription keepalive, will forward the message to
///     worker_streaming_notify_sock. The broker receives messages
///     from all workers and then rebroadcasts the messages to all
///     streamers connected via streaming_streaming_notify_sock.
///   - If a subscription request, will forward a subscription record to
///     worker_subscriptions_sock. The broker receives these messages
///     and sends to one streamer via streaming_subscriptions_sock.
///
/// It is possible for messages to flow back to clients not via the
/// workers_sock. This is the case for subscriptions and other asynchronous
/// notifications.
///
/// @msc
///   client [label="Client"],broker [label="Server Broker"],rp [label="Request Processor"], sr [label="Persisted State Reactor"];
///
///   --- [label="Simple Request Flow"];
///   client => broker [label="clients_sock"];
///   broker => rp [label="workers_sock"];
///   rp => broker [label="workers_sock"];
///   broker => client [label="clients_sock"];
///
///   --- [label="Persisted State Requests"];
///   client => broker [label="clients_sock"];
///   broker => rp [label="workers_sock"];
///   rp => sr [label="worker_subscriptions_sock"];
///   sr => broker [label="worker_subscriptions_sock"];
///   broker => client [label="clients_sock"];
///
///   --- [label="Persisted State Update"];
///   client => broker [label="clients_sock"];
///   broker => rp [label="workers_sock"];
///   rp => sr [label="worker_streaming_notify_sock"];
///   sr => broker [label="worker_subscriptions_sock"];
///   broker => client [label="clients_sock"];
///
///   --- [label="Asynchronous Persisted State Response"];
///   sr => broker [label="worker_subscriptions_sock"];
///   broker => client [label="clients_sock"];
/// @endmsc
class ZIPPYLOG_EXPORT Server : public ::zippylog::device::Device {
    public:
        /// Construct a server from a server config object
        Server(ServerStartParams &params);

        virtual ~Server();

        /// Start the server
        ///
        /// This sets up all 0MQ sockets and starts up the processing threads.
        /// It does NOT perform any message processing. If called multiple
        /// times, subsequent times are noops.
        ///
        /// This can throw an exception if an initial call to Start() has not
        /// finished or if it didn't complete. This represents an unstable
        /// object which should be disposed of.
        bool Start();

        /// Perform server functionality
        ///
        /// This function is the heart of the server. When called, it looks
        /// for work to be done (messages ready to be received, background
        /// operations, etc).
        ///
        /// If work is available, it is processed immediately. If not, it can
        /// wait up to the specified number of microseconds for work to become
        /// available. If the configured wait time is 0, the function will
        /// return immediately if no work is ready.
        ///
        /// It is not defined by the API how much work this function actually
        /// performs. Therefore, you should not rely on a call to Pump() to
        /// flush all pending work from the server.
        ///
        /// Returns 1 if work is performed, 0 if no work is performed, or -1 if
        /// there was an error performing work.
        ///
        /// @param wait_microseconds up to how long to wait for work to become
        /// available before returning. In microseconds.
        ::zippylog::device::PumpResult Pump(int32 wait_microseconds = 250000);

        /// Shut down the server
        ///
        /// On first call, will trigger the shutdown semaphore which signals all
        /// created threads to stop execution. The function call will block
        /// until all threads have been joined.
        ///
        /// On second call, is a no-op.
        /// @todo need an API to force shutdown
        void Shutdown();

        /// Obtain a list of client 0MQ endpoints
        ///
        /// The list is the set of 0MQ endpoints the server will accept client
        /// requests from.
        ::std::vector< ::std::string > ClientEndpoints() const { return this->listen_endpoints; }

        /// Parse a config file into a ServerConfig object
        ///
        /// This attempts to load the Lua file referenced by the filesystem
        /// path given. If the file cannot be loaded or if there is an error
        /// with the config, the function returns false and sets error to be
        /// an error message, suitable for printing to the user. The state of
        /// the ServerConfig object after failure is undefined.
        static bool ParseConfig(const ::std::string path, ServerStartParams &params, ::std::string &error);

    protected:
        void OnRunStart();

        /// Spins up a new request processor on a new thread
        bool CreateRequestProcessorDevice();

        /// Spins up a new persisted state reactor on a new thread
        bool CreatePersistedStateReactorDevice();

        /// Checks that all the threads are still kicking
        ///
        /// Kills the server if any have exited.
        void CheckDevices();

        /// the path to the store the server operates against
        ::std::string store_path;

        /// 0MQ endpoints to bind XREP sockets to listen for client messages
        ::std::vector< ::std::string > listen_endpoints;

        /// The number of worker threads to run
        uint32 number_worker_threads;

        /// The number of streaming threads to run
        uint32 number_persisted_reactor_threads;

        /// The default subscription expiration TTL, in milliseconds
        uint32 subscription_ttl;

        /// Bucket to log server's own log messages to
        ::std::string log_bucket;

        /// Stream set to log server's own log messages to
        ::std::string log_stream_set;

        /// Whether we write logs
        ///
        /// This is a convenience variable set at construction time.
        bool write_logs;

        /// How often to flush written streams, in milliseconds
        int32 stream_flush_interval;

        /// whether client-supplied Lua code can be executed
        bool lua_execute_client_code;

        /// max memory size of Lua interpreters attached to streaming
        uint32 lua_streaming_max_memory;

        /// The store we are bound to
        ::zippylog::Store * store;

        /// Whether the internal structure is set up and ready for running
        bool start_started;
        bool initialized;

        /// Semaphore to signal child devices whether they should be active
        ::zippylog::platform::ConditionalWait active;

        /// 0MQ context to use
        ::zmq::context_t * zctx;

        /// Whether we own the 0MQ context (whether to delete in dtor)
        bool own_context;

        // fans XREQ that fans out to individual worker threads
        ::zmq::socket_t * workers_sock;
        ::std::string worker_endpoint;

        // binds to listen for client requests on configured interfaces
        ::zmq::socket_t * clients_sock;

        // XREP that receives all streamed envelopes to be sent to clients
        ::zmq::socket_t * streaming_sock;
        ::std::string streaming_endpoint;

        // PULL that receives processed client subscription requests
        // messages delivered to one random streamer
        ::zmq::socket_t * worker_subscriptions_sock;
        ::std::string worker_subscriptions_endpoint;

        // PUSH that sends client subscription requests to streamers
        ::zmq::socket_t * streaming_subscriptions_sock;
        ::std::string streaming_subscriptions_endpoint;

        // PULL that receives processed client streaming messages
        // messages that need to be forwarded to all streamers
        // we can't send directly from the workers to the streamers
        // because there is potentially a many to many mapping there
        // the broker binds to both endpoints and distributes messages
        // properly
        ::zmq::socket_t * worker_streaming_notify_sock;
        ::std::string worker_streaming_notify_endpoint;

        // PUB that sends processed client streaming messages to all streamers
        ::zmq::socket_t * streaming_streaming_notify_sock;
        ::std::string streaming_streaming_notify_endpoint;

        // PULL that receives store changes
        ::zmq::socket_t * store_changes_input_sock;
        ::std::string store_changes_input_endpoint;

        // PUB that sends store changes to streamers
        ::zmq::socket_t * store_changes_output_sock;
        ::std::string store_changes_output_endpoint;

        // PULL that receives logging messages from other threads
        ::zmq::socket_t * logger_sock;
        ::std::string logger_endpoint;

        // PUSH that sends logging messages to main logging sock
        // yes, we have both a client and server in the same object. this is easier
        ::zmq::socket_t * log_client_sock;

        /// socket endpoints used by store writer
        ::std::string store_writer_envelope_pull_endpoint;
        ::std::string store_writer_envelope_rep_endpoint;

        /// poll structure for 0MQ
        ::zmq::pollitem_t pollitems[7];

        /// Timer that signals when we should perform a stream flush
        ::zippylog::platform::Timer stream_flush_timer;

        /// Timer that signals when we should check on thread status
        ///
        /// This is a giant hack until a better solution can be implemented.
        /// @todo unhackify
        ::zippylog::platform::Timer thread_check_timer;

        /// server id
        ///
        /// used for identification purposes in logging
        ::std::string id;

        /// Store writer device
        ::zippylog::device::StoreWriter *store_writer;

        /// Store watcher device
        ::zippylog::StoreWatcher *store_watcher;

        /// Threads running workers/request processors
        ::std::vector< ::zippylog::RequestProcessor * > request_processors;

        /// Threads running persisted state reactors
        ::std::vector< ::zippylog::device::PersistedStateReactor * > persisted_state_reactors;

    private:
        // copy constructor and assignment operator are not available
        Server(Server const &orig);
        Server & operator=(Server const &orig);
};

}} // namespaces

#endif
