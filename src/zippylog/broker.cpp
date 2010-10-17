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

#include <zippylog/broker.hpp>

#include <zippylog/platform.hpp>
#include <zippylog/server.hpp>
#include <zippylog/streamer.hpp>
#include <zippylog/zeromq.hpp>
#include <zippylog/zippylogd.pb.h>

// Lua is compiled as C most of the time, so import as such to avoid
// name mangling
extern "C" {
#include <lua.h>
#include <lauxlib.h>
}

#include <sstream>

namespace zippylog {
namespace server {

using ::std::ostringstream;
using ::zippylog::zippylogd::BrokerStartup;
using ::zippylog::zippylogd::BrokerShutdown;
using ::zippylog::zippylogd::BrokerFlushOutputStreams;

const string Broker::WORKER_ENDPOINT = "inproc://workers";
const string Broker::STORE_CHANGE_ENDPOINT = "inproc://store_changes";
const string Broker::STREAMING_ENDPOINT = "inproc://streaming";
const string Broker::LOGGER_ENDPOINT = "inproc://logger";

const string Broker::WORKER_SUBSCRIPTIONS_ENDPOINT = "inproc://worker_subscriptions";
const string Broker::STREAMING_SUBSCRIPTIONS_ENDPOINT = "inproc://streaming_subscriptions";
const string Broker::WORKER_STREAMING_NOTIFY_ENDPOINT = "inproc://worker_streaming_notify";
const string Broker::STREAMING_STREAMING_NOTIFY_ENDPOINT = "inproc://streaming_notify";


#define CLIENT_INDEX 0
#define WORKER_INDEX 1
#define STREAMING_INDEX 2
#define WORKER_SUBSCRIPTIONS_INDEX 3
#define STREAMING_NOTIFY_INDEX 4
#define LOGGER_INDEX 5

Broker::Broker(const string config_file_path) : zctx(3)
{
    this->init();

    string error;
    if (!ParseConfig(config_file_path, this->config, error)) {
        throw error;
    }

    this->store = new Store(this->config.store_path);
}

Broker::Broker(const Broker & orig) : zctx(1)
{
    throw "can not copy Broker instances";
}

Broker & Broker::operator=(const Broker & orig)
{
    throw "cannot assign Broker instances";
}

Broker::~Broker()
{
    if (this->workers_sock) delete this->workers_sock;
    if (this->clients_sock) delete this->clients_sock;
    if (this->streaming_sock) delete this->streaming_sock;
    if (this->worker_subscriptions_sock) delete this->worker_subscriptions_sock;
    if (this->streaming_subscriptions_sock) delete this->streaming_subscriptions_sock;
    if (this->worker_streaming_notify_sock) delete this->worker_streaming_notify_sock;
    if (this->streaming_streaming_notify_sock) delete this->streaming_streaming_notify_sock;
    if (this->logger_sock) delete this->logger_sock;
    if (this->log_client_sock) delete this->log_client_sock;

    if (this->worker_start_data) delete this->worker_start_data;
    if (this->store_watcher_start) delete this->store_watcher_start;
    if (this->streaming_thread_data) delete this->streaming_thread_data;

    if (this->store) delete this->store;
}

void Broker::init()
{
    this->active = true;
    this->exec_thread = NULL;
    this->worker_start_data = NULL;
    this->workers_sock = NULL;
    this->clients_sock = NULL;
    this->streaming_sock = NULL;
    this->worker_subscriptions_sock = NULL;
    this->streaming_subscriptions_sock = NULL;
    this->worker_streaming_notify_sock = NULL;
    this->streaming_streaming_notify_sock = NULL;
    this->logger_sock = NULL;
    this->log_client_sock = NULL;
    this->store = NULL;
    this->store_watcher_thread = NULL;
    this->store_watcher_start = NULL;
    this->streaming_thread_data = NULL;

    platform::UUID uuid;
    if (!platform::CreateUUID(uuid)) {
        throw "could not create UUID";
    }

    this->id = string((const char *)&uuid, sizeof(uuid));
}

/*
The broker contains a number of 0MQ sockets.

First, we have the client socket. It binds to whatever interfaces were
defined at config time. A single 0MQ socket can bind to multiple endpoints,
which is just plain awesome.

The client socket receives messages and forwards them to a workers socket.
The workers socket distributes messages to a number of worker threads, which
pull messages at their leisure.

If we see a response from the workers, we forward it back to the client.

Some requests like streaming are processed by non-worker threads. In these
cases, the workers deposit a set of messages on some internal sockets. The
broker consumes these messages and forwards them to the appropriate
socket connected to the other thread or thread pool.

The requests processed by non-worker threads deposit messages on their own
inpoc sockets. These sockets are read by the broker and messages are forwarded
to clients, as appropriate.

*/

void Broker::run()
{
    this->setup_internal_sockets();

    {
        BrokerStartup log = BrokerStartup();
        log.set_id(this->id);
        Envelope logenvelope = Envelope();
        log.add_to_envelope(&logenvelope);
        zeromq::send_envelope(this->log_client_sock, logenvelope);
    }

    this->create_worker_threads();
    this->create_store_watcher();
    this->create_streaming_threads();
    this->setup_listener_sockets();

    int number_pollitems = 6;
    zmq::pollitem_t* pollitems = new zmq::pollitem_t[number_pollitems];

    pollitems[CLIENT_INDEX].socket = *this->clients_sock;
    pollitems[CLIENT_INDEX].events = ZMQ_POLLIN;
    pollitems[CLIENT_INDEX].fd = 0;
    pollitems[CLIENT_INDEX].revents = 0;

    pollitems[WORKER_INDEX].socket = *this->workers_sock;
    pollitems[WORKER_INDEX].events = ZMQ_POLLIN;
    pollitems[WORKER_INDEX].fd = 0;
    pollitems[WORKER_INDEX].revents = 0;

    pollitems[STREAMING_INDEX].socket = *this->streaming_sock;
    pollitems[STREAMING_INDEX].events = ZMQ_POLLIN;
    pollitems[STREAMING_INDEX].fd = 0;
    pollitems[STREAMING_INDEX].revents = 0;

    pollitems[WORKER_SUBSCRIPTIONS_INDEX].socket = *this->worker_subscriptions_sock;
    pollitems[WORKER_SUBSCRIPTIONS_INDEX].events = ZMQ_POLLIN;
    pollitems[WORKER_SUBSCRIPTIONS_INDEX].fd = 0;
    pollitems[WORKER_SUBSCRIPTIONS_INDEX].revents = 0;

    pollitems[STREAMING_NOTIFY_INDEX].socket = *this->worker_streaming_notify_sock;
    pollitems[STREAMING_NOTIFY_INDEX].events = ZMQ_POLLIN;
    pollitems[STREAMING_NOTIFY_INDEX].fd = 0;
    pollitems[STREAMING_NOTIFY_INDEX].revents = 0;

    pollitems[LOGGER_INDEX].socket = *this->logger_sock;
    pollitems[LOGGER_INDEX].events = ZMQ_POLLIN;
    pollitems[LOGGER_INDEX].fd = 0;
    pollitems[LOGGER_INDEX].revents = 0;

    zmq::message_t msg;
    int64 more;
    size_t moresz = sizeof(more);

    platform::Timer stream_flush_timer = platform::Timer(this->config.stream_flush_interval * 1000);
    if (!stream_flush_timer.Start()) {
        throw "could not start stream flush timer";
    }

    // TODO so much repetition here. it makes me feel dirty
    // TODO better error handling
    while (this->active) {
        // wait up to 1s for a message to become available
        int rc = zmq::poll(pollitems, number_pollitems, 1000000);

        if (pollitems[LOGGER_INDEX].revents & ZMQ_POLLIN) {
            while (true) {
                if (!this->logger_sock->recv(&msg, 0)) break;

                this->store->WriteEnvelope(this->config.log_bucket, this->config.log_stream_set, msg.data(), msg.size());

                // TODO this is mostly for debugging purposes and should be implemented another way
                // once the project has matured
                /*
#ifdef _DEBUG
                Envelope debugEnvelope = Envelope(&msg);
                ::std::cout << debugEnvelope.ToString();
#endif
                */
                moresz = sizeof(more);
                this->logger_sock->getsockopt(ZMQ_RCVMORE, &more, &moresz);
                if (!more) break;
            }
        }

        // we always favor shipping messages OUT of the server to the clients
        // that way, under heavy load, we try to get smaller before we
        // get bigger

        // move worker responses to client
        else if (pollitems[WORKER_INDEX].revents & ZMQ_POLLIN) {
            while (true) {
                if (!this->workers_sock->recv(&msg, 0)) {
                    break;
                }

                moresz = sizeof(more);
                this->workers_sock->getsockopt(ZMQ_RCVMORE, &more, &moresz);
                if (!this->clients_sock->send(msg, more ? ZMQ_SNDMORE : 0)) {
                    break;
                }

                if (!more) break;
            }
        }

        // move streaming messages to client
        else if (pollitems[STREAMING_INDEX].revents & ZMQ_POLLIN) {
            while (true) {
                if (!this->streaming_sock->recv(&msg, 0)) break;

                moresz = sizeof(more);
                this->streaming_sock->getsockopt(ZMQ_RCVMORE, &more, &moresz);
                if (!this->clients_sock->send(msg, more ? ZMQ_SNDMORE : 0)) break;
                if (!more) break;
            }
        }

        // move subscriptions requests to streamer
        else if (pollitems[WORKER_SUBSCRIPTIONS_INDEX].revents & ZMQ_POLLIN) {
            while (true) {
                if (!this->worker_subscriptions_sock->recv(&msg, 0)) break;

                moresz = sizeof(more);
                this->worker_subscriptions_sock->getsockopt(ZMQ_RCVMORE, &more, &moresz);
                if (!this->streaming_subscriptions_sock->send(msg, more ? ZMQ_SNDMORE : 0)) break;
                if (!more) break;
            }
        }

        // move subscription general messages to all streamers
        else if (pollitems[STREAMING_NOTIFY_INDEX].revents & ZMQ_POLLIN) {
            while (true) {
                if (!this->worker_streaming_notify_sock->recv(&msg, 0)) break;

                moresz = sizeof(more);
                this->worker_streaming_notify_sock->getsockopt(ZMQ_RCVMORE, &more, &moresz);
                if (!this->streaming_streaming_notify_sock->send(msg, more ? ZMQ_SNDMORE : 0)) break;
                if (!more) break;
            }
        }


        // forward client stream requests to streaming thread pool

        // send client requests to workers
        else if (pollitems[CLIENT_INDEX].revents & ZMQ_POLLIN) {
            while (true) {
                if (!this->clients_sock->recv(&msg, 0)) {
                    break;
                }

                moresz = sizeof(more);
                this->clients_sock->getsockopt(ZMQ_RCVMORE, &more, &moresz);
                if (!this->workers_sock->send(msg, more ? ZMQ_SNDMORE : 0)) {
                    break;
                }

                if (!more) break;
            }
        }

        if (stream_flush_timer.Signaled()) {
            BrokerFlushOutputStreams log = BrokerFlushOutputStreams();
            log.set_id(this->id);
            Envelope e;
            log.add_to_envelope(&e);
            zeromq::send_envelope(this->log_client_sock, e);

            this->store->FlushOutputStreams();
            if (!stream_flush_timer.Start()) {
                throw "could not restart stream flush timer";
            }
        }
    }

    // need to do some cleanup
    // tell workers to shut down
    this->worker_start_data->active = false;

    delete pollitems;
}

void Broker::RunAsync()
{
    this->exec_thread = create_thread(Broker::AsyncExecStart, this);
}

void * Broker::AsyncExecStart(void *data)
{
    Broker *broker = (Broker *)data;
    broker->run();

    return NULL;
}


void Broker::create_worker_threads()
{
    this->worker_start_data = new request_processor_start_data;
    this->worker_start_data->ctx = &this->zctx;
    this->worker_start_data->store = this->store;
    this->worker_start_data->broker_endpoint = WORKER_ENDPOINT.c_str();
    this->worker_start_data->streaming_subscriptions_endpoint = WORKER_SUBSCRIPTIONS_ENDPOINT.c_str();
    this->worker_start_data->streaming_updates_endpoint = WORKER_STREAMING_NOTIFY_ENDPOINT.c_str();
    this->worker_start_data->logger_endpoint = LOGGER_ENDPOINT.c_str();
    this->worker_start_data->active = true;

    for (int i = this->config.worker_threads; i; --i) {
        void * thread = create_thread(Request::request_processor, this->worker_start_data);
        if (!thread) {
            throw "error creating worker thread";
        }
        this->worker_threads.push_back(thread);
    }
}

void Broker::create_store_watcher()
{
    this->store_watcher_start = new store_watcher_start_data;
    this->store_watcher_start->endpoint = STORE_CHANGE_ENDPOINT.c_str();
    this->store_watcher_start->logging_endpoint = LOGGER_ENDPOINT.c_str();
    this->store_watcher_start->zctx = &this->zctx;
    this->store_watcher_start->store = this->store;
    this->store_watcher_start->active = &this->active;

    this->store_watcher_thread = create_thread(StoreWatcherStart, this->store_watcher_start);
}

void Broker::create_streaming_threads()
{
    this->streaming_thread_data = new streaming_start_data;
    this->streaming_thread_data->store_change_endpoint = STORE_CHANGE_ENDPOINT.c_str();
    this->streaming_thread_data->streaming_endpoint = STREAMING_ENDPOINT.c_str();
    this->streaming_thread_data->client_updates_endpoint = STREAMING_STREAMING_NOTIFY_ENDPOINT.c_str();
    this->streaming_thread_data->subscriptions_endpoint = STREAMING_SUBSCRIPTIONS_ENDPOINT.c_str();
    this->streaming_thread_data->logging_endpoint = LOGGER_ENDPOINT.c_str();
    this->streaming_thread_data->zctx = &this->zctx;
    this->streaming_thread_data->store = this->store;
    this->streaming_thread_data->active = &this->active;
    this->streaming_thread_data->subscription_ttl = this->config.subscription_ttl;

    for (int i = this->config.streaming_threads; i; i--) {
        void * thread = create_thread(Broker::StreamingStart, this->streaming_thread_data);
        if (!thread) {
            throw "error creating streaming thread";
        }
        this->streaming_threads.push_back(thread);
    }
}

void Broker::setup_internal_sockets()
{
    this->logger_sock = new zmq::socket_t(this->zctx, ZMQ_PULL);
    this->logger_sock->bind(LOGGER_ENDPOINT.c_str());

    this->log_client_sock = new zmq::socket_t(this->zctx, ZMQ_PUSH);
    this->log_client_sock->connect(LOGGER_ENDPOINT.c_str());

    this->workers_sock = new zmq::socket_t(this->zctx, ZMQ_XREQ);
    this->workers_sock->bind(WORKER_ENDPOINT.c_str());

    this->streaming_sock = new zmq::socket_t(this->zctx, ZMQ_PULL);
    this->streaming_sock->bind(STREAMING_ENDPOINT.c_str());

    this->worker_subscriptions_sock = new zmq::socket_t(this->zctx, ZMQ_PULL);
    this->worker_subscriptions_sock->bind(WORKER_SUBSCRIPTIONS_ENDPOINT.c_str());

    this->worker_streaming_notify_sock = new zmq::socket_t(this->zctx, ZMQ_PULL);
    this->worker_streaming_notify_sock->bind(WORKER_STREAMING_NOTIFY_ENDPOINT.c_str());

    this->streaming_subscriptions_sock = new zmq::socket_t(this->zctx, ZMQ_PUSH);
    this->streaming_subscriptions_sock->bind(STREAMING_SUBSCRIPTIONS_ENDPOINT.c_str());

    this->streaming_streaming_notify_sock = new zmq::socket_t(this->zctx, ZMQ_PUB);
    this->streaming_streaming_notify_sock->bind(STREAMING_STREAMING_NOTIFY_ENDPOINT.c_str());
}

void Broker::setup_listener_sockets()
{
    this->clients_sock = new zmq::socket_t(this->zctx, ZMQ_XREP);

    // 0MQ sockets can bind to multiple endpoints
    // how AWESOME is that?
    for (int i = 0; i < this->config.listen_endpoints.size(); i++) {
        this->clients_sock->bind(this->config.listen_endpoints[i].c_str());
    }
}

void Broker::Shutdown()
{
    this->active = false;

    // wait for workers to terminate
    vector<void *>::iterator i;
    for (i = this->worker_threads.begin(); i < this->worker_threads.end(); i++) {
        join_thread(*i);
    }

    // wait for streaming threads to terminate
    for (i = this->streaming_threads.begin(); i < this->streaming_threads.end(); i++) {
        join_thread(*i);
    }

    // forcibly kill store watcher, b/c that's the only way right now
    join_thread(this->store_watcher_thread);

    if (this->exec_thread) {
        join_thread(this->exec_thread);
    }
}

bool Broker::ParseConfig(const string path, broker_config &config, string &error)
{
    ostringstream os;

    lua_State *L = luaL_newstate();
    if (luaL_dofile(L, path.c_str())) {
        os << "error running config file: " << lua_tostring(L, -1);
        goto cleanup;
    }

    // store_path defines path to stream store
    lua_getglobal(L, "store_path");
    if (lua_isnil(L, -1)) {
        os << "'store_path' is not defined";
        goto cleanup;
    }
    if (!lua_isstring(L, -1)) {
        os << "'store_path' not a string";
        goto cleanup;
    }
    config.store_path = lua_tostring(L, -1);
    lua_pop(L, 1);

    // endpoints is a table of strings representing 0MQ socket endpoints to
    // listen on
    lua_getglobal(L, "endpoints");
    if (lua_isnil(L, -1)) {
        os << "'endpoints' not defined";
        goto cleanup;
    }
    if (!lua_istable(L, -1)) {
        os << "'endpoints' variable not a table";
        goto cleanup;
    }

    // iterate over the table
    lua_pushnil(L);
    while (lua_next(L, 1) != 0) {
        if (!lua_isstring(L, -1)) {
            if (lua_isstring(L, -2)) {
                os << "endpoints value at index '" << lua_tostring(L, -2) << "' is not a string";
                goto cleanup;
            }
            else {
                os << "non-string value seen in endpoints table. index not printable";
                goto cleanup;
            }
        }
        // else
        config.listen_endpoints.push_back(lua_tostring(L, -1));
        lua_pop(L, 1);
    }

    // number of worker threads to run
    lua_getglobal(L, "worker_threads");
    config.worker_threads = luaL_optinteger(L, -1, 3);
    lua_pop(L, 1);

    // number of streaming threads to run
    lua_getglobal(L, "streaming_threads");
    config.streaming_threads = luaL_optinteger(L, -1, 1);
    lua_pop(L, 1);

    // time to live of streaming subscriptions
    lua_getglobal(L, "streaming_subscription_ttl");
    config.subscription_ttl = luaL_optinteger(L, -1, 60);
    lua_pop(L, 1);

    // logging settings
    lua_getglobal(L, "log_bucket");
    config.log_bucket = luaL_optstring(L, -1, "zippylog");
    lua_pop(L, 1);

    lua_getglobal(L, "log_stream_set");
    config.log_stream_set = luaL_optstring(L, -1, "zippylogd");
    lua_pop(L, 1);

    // interval at which to flush streams
    lua_getglobal(L, "stream_flush_interval");
    config.stream_flush_interval = luaL_optinteger(L, -1, 5000);
    lua_pop(L, 1);
    if (config.stream_flush_interval < 0) {
        os << "stream_flush_interval must be positive";
        goto cleanup;
    }
    else if (config.stream_flush_interval < 1000) {
        os << "stream_flush_interval must be greater than 1000";
        goto cleanup;
    }


cleanup:
    lua_close(L);

    if (os.str().length()) {
        error = os.str();
        return false;
    }

    return true;
}

broker_config::broker_config()
{
    listen_endpoints = vector<string>();
}

void * Broker::StoreWatcherStart(void *d)
{
    store_watcher_start_data * data = (store_watcher_start_data *)d;

    assert(data->endpoint);
    assert(data->zctx);
    assert(data->store);
    assert(data->logging_endpoint);
    assert(data->active);

    StoreWatcher watcher(data->store, data->zctx, data->endpoint, data->logging_endpoint);
    watcher.SetShutdownSemaphore(data->active);
    watcher.run();

    return NULL;
}

void * Broker::StreamingStart(void *d)
{
    streaming_start_data * data = (streaming_start_data *)d;

    assert(data->zctx);
    assert(data->store_change_endpoint);
    assert(data->streaming_endpoint);
    assert(data->subscriptions_endpoint);
    assert(data->client_updates_endpoint);
    assert(data->logging_endpoint);
    assert(data->store);
    assert(data->subscription_ttl);

    try {
        Streamer streamer(
            data->store,
            data->zctx,
            data->store_change_endpoint,
            data->streaming_endpoint,
            data->subscriptions_endpoint,
            data->client_updates_endpoint,
            data->logging_endpoint,
            data->subscription_ttl
        );
        streamer.SetShutdownSemaphore(data->active);

        streamer.Run();
    }
    catch (zmq::error_t e) {
        const char *error = e.what();
        throw error;
    }
    catch (...) {

    }

    return NULL;
}

}} // namespaces
