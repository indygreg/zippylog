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

#include <zippylog/device/server.hpp>

#include <zippylog/platform.hpp>
#include <zippylog/zeromq.hpp>
#include <zippylog/zippylogd.pb.h>

// Lua is compiled as C most of the time, so import as such to avoid
// name mangling
extern "C" {
#include <lua.h>
#include <lauxlib.h>
}

#include <sstream>

#ifdef _DEBUG
#include <iostream>
#endif

namespace zippylog {
namespace device {

using ::std::string;
using ::std::ostringstream;
using ::std::vector;
using ::zippylog::platform::Thread;
using ::zippylog::zippylogd::BrokerStartup;
using ::zippylog::zippylogd::BrokerShutdown;
using ::zippylog::zippylogd::BrokerFlushOutputStreams;
using ::zippylog::device::Streamer;
using ::zippylog::device::StreamerStartParams;
using ::zippylog::device::server::WatcherStartParams;
using ::zippylog::device::server::Watcher;
using ::zippylog::device::server::Worker;
using ::zippylog::device::server::WorkerStartParams;

#define CLIENT_INDEX 0
#define WORKER_INDEX 1
#define STREAMING_INDEX 2
#define WORKER_SUBSCRIPTIONS_INDEX 3
#define STREAMING_NOTIFY_INDEX 4
#define LOGGER_INDEX 5

Server::Server(const string config_file_path) :
    store(NULL),
    active(true),
    initialized(false),
    zctx(3),
    workers_sock(NULL),
    clients_sock(NULL),
    streaming_sock(NULL),
    worker_subscriptions_sock(NULL),
    streaming_subscriptions_sock(NULL),
    worker_streaming_notify_sock(NULL),
    streaming_streaming_notify_sock(NULL),
    logger_sock(NULL),
    log_client_sock(NULL),
    exec_thread(NULL),
    store_writer_thread(NULL),
    store_watcher_thread(NULL)
{
    platform::UUID uuid;
    if (!platform::CreateUUID(uuid)) {
        throw "could not create UUID";
    }

    this->id = string((const char *)&uuid, sizeof(uuid));

    // the 0MQ socket endpoints used internally are created by taking the UUID
    // of the server and adding a descriptor to the end of it. The actual
    // value doesn't matter, since the strings are only passed around within
    // the object and derived objects. However, they should be unique so
    // multiple servers can exist within the same process. I don't think
    // the length of the endpoints will negatively impact runtime performance
    // since I'm pretty sure everything gets converted to a descriptor by 0MQ,
    // but it might be worth investigating. We preserve human-friendly names
    // until proven otherwise.
    string uuid_s;
    if (!platform::FormatUUID(uuid, uuid_s)) {
        throw "could not format UUID";
    }

    this->worker_endpoint = "inproc://" + uuid_s + "workers";
    this->store_change_endpoint = "inproc://" + uuid_s + "store_changes";
    this->streaming_endpoint = "inproc://" + uuid_s + "streaming";
    this->logger_endpoint = "inproc://" + uuid_s + "logger";
    this->worker_subscriptions_endpoint = "inproc://" + uuid_s + "worker_subscriptions";
    this->streaming_subscriptions_endpoint = "inproc://" + uuid_s + "streaming_subscriptions";
    this->worker_streaming_notify_endpoint = "inproc://" + uuid_s + "worker_streaming_notify";
    this->streaming_streaming_notify_endpoint = "inproc://" + uuid_s + "streaming_notify";
    this->store_writer_envelope_pull_endpoint = "inproc://" + uuid_s + "store_writer_envelope_pull";
    this->store_writer_envelope_rep_endpoint = "inproc://" + uuid_s + "store_writer_envelope_rep";

    string error;
    if (!ParseConfig(config_file_path, this->config, error)) {
        throw error;
    }

    this->store = Store::CreateStore(this->config.store_path);
}

Server::~Server()
{
    this->Shutdown();

    if (this->workers_sock) delete this->workers_sock;
    if (this->clients_sock) delete this->clients_sock;
    if (this->streaming_sock) delete this->streaming_sock;
    if (this->worker_subscriptions_sock) delete this->worker_subscriptions_sock;
    if (this->streaming_subscriptions_sock) delete this->streaming_subscriptions_sock;
    if (this->worker_streaming_notify_sock) delete this->worker_streaming_notify_sock;
    if (this->streaming_streaming_notify_sock) delete this->streaming_streaming_notify_sock;
    if (this->logger_sock) delete this->logger_sock;
    if (this->log_client_sock) delete this->log_client_sock;
    if (this->store) delete this->store;
}

void Server::Run()
{
    if (!this->initialized) this->Initialize();

    {
        BrokerStartup log = BrokerStartup();
        log.set_id(this->id);
        Envelope logenvelope = Envelope();
        log.add_to_envelope(&logenvelope);
        zeromq::send_envelope(this->log_client_sock, logenvelope);
    }

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

    platform::Timer stream_flush_timer(this->config.stream_flush_interval * 1000);
    if (!stream_flush_timer.Start()) {
        throw "could not start stream flush timer";
    }

    // TODO this is a giant hack until we have a better system in place
    platform::Timer thread_exit_timer(5000000);
    if (!thread_exit_timer.Start()) {
        throw "TODO handle failure to start time";
    }

    // TODO so much repetition here. it makes me feel dirty
    // TODO better error handling
    while (this->active) {
        // perform maintenance at the top of the loop

        // we flush output streams if we need to
        // TODO move this to writer thread once we isolate all writing to there
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

        if (thread_exit_timer.Signaled()) {
            this->CheckThreads();
            if (!thread_exit_timer.Start()) {
                throw "TODO handle failure of timer to start";
            }
        }

        // wait up to 0.5s for a message to become available
        int rc = zmq::poll(pollitems, number_pollitems, 500000);
        if (rc < 1) continue;

        if (pollitems[LOGGER_INDEX].revents & ZMQ_POLLIN) {
            while (true) {
                if (!this->logger_sock->recv(&msg, 0)) break;

                this->store->WriteEnvelope(this->config.log_bucket, this->config.log_stream_set, msg.data(), msg.size());

                // TODO this is mostly for debugging purposes and should be implemented another way
                // once the project has matured

#ifdef _DEBUG
                Envelope debugEnvelope = Envelope(msg.data(), msg.size());
                ::std::cout << debugEnvelope.ToString();
#endif

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
    }

    delete [] pollitems;
}

void Server::RunAsync()
{
    this->exec_thread = new Thread(Server::AsyncExecStart, this);
}

void * Server::AsyncExecStart(void *data)
{
    Server *server = (Server *)data;
    server->Run();

    return NULL;
}

bool Server::SynchronizeStartParams()
{
    // worker/request processor
    ::zippylog::RequestProcessorStartParams params;
    params.active = &this->active;
    params.ctx = &this->zctx;
    params.client_endpoint = this->worker_endpoint;
    params.logger_endpoint = this->logger_endpoint;
    params.store_path = this->config.store_path;

    this->request_processor_params.request_processor_params = params;
    this->request_processor_params.streaming_subscriptions_endpoint = this->worker_subscriptions_endpoint;
    this->request_processor_params.streaming_updates_endpoint = this->worker_streaming_notify_endpoint;
    this->request_processor_params.store_writer_envelope_pull_endpoint = this->store_writer_envelope_pull_endpoint;
    this->request_processor_params.store_writer_envelope_rep_endpoint = this->store_writer_envelope_rep_endpoint;

    // store watcher
    StoreWatcherStartParams swparams;
    swparams.active = &this->active;
    swparams.logging_endpoint = this->logger_endpoint;

    // TODO this assumes we're using a file-based store, which is a no-no
    swparams.store_path = ((SimpleDirectoryStore *)(this->store))->RootDirectoryPath();
    swparams.zctx = &this->zctx;

    this->store_watcher_params.params = swparams;
    this->store_watcher_params.socket_endpoint = this->store_change_endpoint;

    // streamer
    this->streamer_params.client_endpoint = this->streaming_endpoint;
    this->streamer_params.store_changes_endpoint = this->store_change_endpoint;
    this->streamer_params.logging_endpoint = this->logger_endpoint;
    this->streamer_params.subscriptions_endpoint = this->streaming_subscriptions_endpoint;
    this->streamer_params.subscription_updates_endpoint = this->streaming_streaming_notify_endpoint;

    this->streamer_params.ctx = &this->zctx;
    this->streamer_params.store_path = this->config.store_path;
    this->streamer_params.subscription_ttl = this->config.subscription_ttl;
    this->streamer_params.lua_allow = this->config.lua_execute_client_code;
    this->streamer_params.lua_max_memory = this->config.lua_streaming_max_memory;
    this->streamer_params.active = &this->active;

    // store writer
    this->store_writer_params.ctx = &this->zctx;
    this->store_writer_params.active = &this->active;
    this->store_writer_params.store_path = this->config.store_path;
    this->store_writer_params.envelope_pull_endpoint = this->store_writer_envelope_pull_endpoint;
    this->store_writer_params.envelope_rep_endpoint = this->store_writer_envelope_rep_endpoint;

    return true;
}

bool Server::Initialize()
{
    if (this->initialized) {
        throw "Initialized() already called!";
    }

    // we set the flag early to catch partial initialization
    this->initialized = true;

    this->SynchronizeStartParams();

    // create all our sockets
    this->logger_sock = new zmq::socket_t(this->zctx, ZMQ_PULL);
    this->logger_sock->bind(this->logger_endpoint.c_str());

    this->log_client_sock = new zmq::socket_t(this->zctx, ZMQ_PUSH);
    this->log_client_sock->connect(this->logger_endpoint.c_str());

    this->workers_sock = new zmq::socket_t(this->zctx, ZMQ_XREQ);
    this->workers_sock->bind(this->worker_endpoint.c_str());

    this->streaming_sock = new zmq::socket_t(this->zctx, ZMQ_PULL);
    this->streaming_sock->bind(this->streaming_endpoint.c_str());

    this->worker_subscriptions_sock = new zmq::socket_t(this->zctx, ZMQ_PULL);
    this->worker_subscriptions_sock->bind(this->worker_subscriptions_endpoint.c_str());

    this->worker_streaming_notify_sock = new zmq::socket_t(this->zctx, ZMQ_PULL);
    this->worker_streaming_notify_sock->bind(this->worker_streaming_notify_endpoint.c_str());

    this->streaming_subscriptions_sock = new zmq::socket_t(this->zctx, ZMQ_PUSH);
    this->streaming_subscriptions_sock->bind(this->streaming_subscriptions_endpoint.c_str());

    this->streaming_streaming_notify_sock = new zmq::socket_t(this->zctx, ZMQ_PUB);
    this->streaming_streaming_notify_sock->bind(this->streaming_streaming_notify_endpoint.c_str());

    // now create child threads

    // start with store writer
    this->store_writer_thread = new Thread(StoreWriterStart, &this->store_writer_params);

    // spin up configured number of workers
    for (int i = this->config.worker_threads; i; --i) {
        if (!this->CreateWorkerThread()) return false;
    }

    // and the streamers
    for (int i = this->config.streaming_threads; i; --i) {
        if (!this->CreateStreamingThread()) return false;
    }

    this->store_watcher_thread = new Thread(StoreWatcherStart, &this->store_watcher_params);

    // bind sockets to listen for client requests
    this->clients_sock = new zmq::socket_t(this->zctx, ZMQ_XREP);

    // 0MQ sockets can bind to multiple endpoints
    // how AWESOME is that?
    for (size_t i = 0; i < this->config.listen_endpoints.size(); i++) {
        this->clients_sock->bind(this->config.listen_endpoints[i].c_str());
    }

    return true;
}

bool Server::CreateWorkerThread()
{
    this->worker_threads.push_back(new Thread(Server::RequestProcessorStart, &this->request_processor_params));

    return true;
}

bool Server::CreateStreamingThread()
{
    this->streaming_threads.push_back(new Thread(Server::StreamingStart, &this->streamer_params));

    return true;
}

void Server::CheckThreads()
{
    if (this->store_writer_thread && !this->store_writer_thread->Alive()) {
        this->Shutdown();
        return;
    }

    if (this->store_watcher_thread && !this->store_watcher_thread->Alive()) {
        this->Shutdown();
        return;
    }

    for (size_t i = 0; i < this->worker_threads.size(); i++) {
        if (!this->worker_threads[i]->Alive()) {
            this->Shutdown();
            return;
        }
    }

    for (size_t i = 0; i < this->streaming_threads.size(); i++) {
        if (!this->streaming_threads[i]->Alive()) {
            this->Shutdown();
            return;
        }
    }
}

void Server::Shutdown()
{
    if (!this->active) return;

    this->active = false;

    // wait for workers to terminate
    vector<Thread *>::iterator i;
    for (i = this->worker_threads.begin(); i < this->worker_threads.end(); i++) {
        (*i)->Join();
        delete *i;
    }
    this->worker_threads.clear();

    // wait for streaming threads to terminate
    for (i = this->streaming_threads.begin(); i < this->streaming_threads.end(); i++) {
        (*i)->Join();
        delete *i;
    }
    this->streaming_threads.clear();

    this->store_watcher_thread->Join();
    delete this->store_watcher_thread;
    this->store_watcher_thread = NULL;

    if (this->store_writer_thread) {
        this->store_writer_thread->Join();
        delete this->store_writer_thread;
        this->store_writer_thread = NULL;
    }

    if (this->exec_thread) {
        this->exec_thread->Join();
        delete this->exec_thread;
        this->exec_thread = NULL;
    }
}

bool Server::ParseConfig(const string path, ServerConfig &config, string &error)
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

    // time to live of streaming subscriptions in milliseconds
    lua_getglobal(L, "streaming_subscription_ttl");
    config.subscription_ttl = luaL_optinteger(L, -1, 60000);
    lua_pop(L, 1);

    // logging settings
    lua_getglobal(L, "log_bucket");
    config.log_bucket = luaL_optstring(L, -1, "zippylog");
    lua_pop(L, 1);

    lua_getglobal(L, "log_stream_set");
    config.log_stream_set = luaL_optstring(L, -1, "zippylogd");
    lua_pop(L, 1);

    // interval at which to flush streams in milliseconds
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

    // Lua settings
    lua_getglobal(L, "lua_execute_client_code");
    config.lua_execute_client_code = lua_toboolean(L, -1);
    lua_pop(L, 1);

    lua_getglobal(L, "lua_streaming_max_memory");
    config.lua_streaming_max_memory = luaL_optinteger(L, -1, 524288);
    lua_pop(L, 1);


cleanup:
    lua_close(L);

    if (os.str().length()) {
        error = os.str();
        return false;
    }

    return true;
}

ServerConfig::ServerConfig()
{
    listen_endpoints = vector<string>();
}

void * Server::StoreWatcherStart(void *d)
{
    WatcherStartParams *params = (WatcherStartParams *)d;

    try {
        Watcher watcher(*params);
        watcher.Run();
    } catch (::std::exception e) {
        string error = e.what();
        return (void *)1;
    }

    return (void *)0;
}

void * Server::StreamingStart(void *d)
{
    StreamerStartParams *params = (StreamerStartParams *)d;
    try {
        Streamer streamer(*params);
        streamer.Run();
    }
    catch (::std::exception e) {
        string error = e.what();
        return (void *)1;
    }

    return (void *)0;
}

void * Server::RequestProcessorStart(void *d)
{
    assert(d);
    WorkerStartParams *params = (WorkerStartParams *)d;
    try {
        Worker processor(*params);
        processor.Run();
    }
    catch (::std::exception e) {
        string error = e.what();
        return (void *)1;
    }

    return (void *)0;
}

void * Server::StoreWriterStart(void *d)
{
    assert(d);
    StoreWriterStartParams *params = (StoreWriterStartParams *)d;
    try {
        StoreWriter writer(*params);
        writer.Run();
    }
    catch (::std::exception e) {
        string error = e.what();
        return (void *)1;
    }

    return (void *)0;
}

}} // namespaces
