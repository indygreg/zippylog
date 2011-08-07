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

#include <zippylog/device/server.hpp>

#include <zippylog/platform.hpp>
#include <zippylog/zeromq.hpp>
#include <zippylog/device/server.pb.h>

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

using ::std::invalid_argument;
using ::std::string;
using ::std::ostringstream;
using ::std::vector;
using ::zippylog::platform::Thread;
using ::zippylog::StoreWatcherStartParams;
using ::zippylog::device::PersistedStateReactor;
using ::zippylog::device::PersistedStateReactorStartParams;
using ::zippylog::device::PumpResult;
using ::zippylog::device::StoreWriterStartParams;
using ::zippylog::device::server::Create;
using ::zippylog::device::server::Destroy;
using ::zippylog::device::server::FlushOutputStreams;
using ::zippylog::device::server::RunStart;
using ::zippylog::device::server::RunFinish;
using ::zippylog::device::server::ReceiveClientMessage;
using ::zippylog::device::server::SendClientMessage;
using ::zippylog::device::server::ServerRequestProcessor;
using ::zippylog::device::server::ServerRequestProcessorStartParams;
using ::zippylog::device::server::WatcherStartParams;
using ::zippylog::device::server::Watcher;
using ::zmq::socket_t;

#define LOG_MESSAGE(msgvar) { \
    msgvar.set_id(this->id); \
    Envelope logenvelope = Envelope(); \
    msgvar.add_to_envelope(&logenvelope); \
    zeromq::send_envelope(this->log_client_sock, logenvelope); \
}

#define CLIENT_INDEX 0
#define WORKER_INDEX 1
#define STREAMING_INDEX 2
#define WORKER_SUBSCRIPTIONS_INDEX 3
#define STREAMING_NOTIFY_INDEX 4
#define LOGGER_INDEX 5
#define STORE_CHANGES_INPUT_INDEX 6

Server::Server(ServerStartParams &params) :
    Device(params.active),
    store_path(params.store_path),
    listen_endpoints(params.listen_endpoints),
    number_worker_threads(params.worker_threads),
    number_persisted_reactor_threads(params.persisted_state_reactor_threads),
    subscription_ttl(params.subscription_ttl),
    log_bucket(params.log_bucket),
    log_stream_set(params.log_stream_set),
    write_logs(false),
    stream_flush_interval(params.stream_flush_interval),
    lua_execute_client_code(params.lua_execute_client_code),
    lua_streaming_max_memory(params.lua_streaming_max_memory),
    store(NULL),
    start_started(false),
    initialized(false),
    zctx(params.ctx),
    own_context(false),
    workers_sock(NULL),
    clients_sock(NULL),
    streaming_sock(NULL),
    worker_subscriptions_sock(NULL),
    streaming_subscriptions_sock(NULL),
    worker_streaming_notify_sock(NULL),
    streaming_streaming_notify_sock(NULL),
    store_changes_input_sock(NULL),
    store_changes_output_sock(NULL),
    logger_sock(NULL),
    log_client_sock(NULL),
    stream_flush_timer(params.stream_flush_interval * 1000),
    thread_check_timer(5000000),
    store_writer(NULL),
    store_watcher(NULL)
{
    // validate input
    if (!params.listen_endpoints.size()) {
        throw invalid_argument("a listen endpoint must be defined to create a server");
    }

    if (!params.store_path.length()) {
        throw invalid_argument("a store path must be defined to create a server");
    }

    platform::UUID uuid;
    platform::CreateUUID(uuid);

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
    /// @todo investigate whether length of descriptor impacts running speed
    string uuid_s;
    platform::FormatUUID(uuid, uuid_s);

    this->worker_endpoint = "inproc://" + uuid_s + "workers";
    this->streaming_endpoint = "inproc://" + uuid_s + "streaming";
    this->logger_endpoint = "inproc://" + uuid_s + "logger";
    this->worker_subscriptions_endpoint = "inproc://" + uuid_s + "worker_subscriptions";
    this->streaming_subscriptions_endpoint = "inproc://" + uuid_s + "streaming_subscriptions";
    this->worker_streaming_notify_endpoint = "inproc://" + uuid_s + "worker_streaming_notify";
    this->streaming_streaming_notify_endpoint = "inproc://" + uuid_s + "streaming_notify";
    this->store_writer_envelope_pull_endpoint = "inproc://" + uuid_s + "store_writer_envelope_pull";
    this->store_writer_envelope_rep_endpoint = "inproc://" + uuid_s + "store_writer_envelope_rep";
    this->store_changes_input_endpoint = "inproc://" + uuid_s + "store_changes_input";
    this->store_changes_output_endpoint = "inproc://" + uuid_s + "store_changes_output";

    this->store = Store::CreateStore(this->store_path);

    if (this->log_bucket.length() && this->log_stream_set.length())
        this->write_logs = true;

    if (!this->zctx) {
        /// @todo magic constant
        this->zctx = new ::zmq::context_t(1);
        this->own_context = true;
    }
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
    if (this->store_changes_input_sock) delete this->store_changes_input_sock;
    if (this->store_changes_output_sock) delete this->store_changes_output_sock;
    if (this->logger_sock) delete this->logger_sock;
    if (this->log_client_sock) delete this->log_client_sock;
    if (this->store) delete this->store;
    if (this->own_context && this->zctx) delete this->zctx;
}

void Server::OnFirstRun()
{
    this->Start();

    Create msg;
    msg.set_store_path(this->store_path);
    for (vector<string>::iterator i = this->listen_endpoints.begin();
         i != this->listen_endpoints.end();
         i++) {
         msg.add_listen_endpoint(*i);
    }

    LOG_MESSAGE(msg);
}

void Server::OnRunStart()
{
    this->Start();

    RunStart log;
    LOG_MESSAGE(log);
}

void Server::OnRunFinish()
{
    this->Shutdown();

    RunFinish log;
    LOG_MESSAGE(log);
}

PumpResult Server::Pump(int32 wait_time)
{
    bool work_done = false;

    // check on background activities

    // we flush output streams if we need to
    // @todo move this to writer thread once we isolate all writing to there
    if (this->stream_flush_timer.Signaled()) {
        work_done = true;

        FlushOutputStreams log;
        LOG_MESSAGE(log);

        this->store->FlushOutputStreams();
        if (!stream_flush_timer.Start()) {
            throw Exception("could not restart stream flush timer");
        }
    }

    if (thread_check_timer.Signaled()) {
        work_done = true;
        this->CheckDevices();
        if (!thread_check_timer.Start()) {
            throw Exception("TODO handle failure of timer to start");
        }
    }

    // look for pending receives on sockets
    int rc = zmq::poll(&this->pollitems[0], 7, wait_time);

    // if nothing pending, return if we have done anything so far
    if (rc < 1) return work_done ? PumpResult::MakeWorkDone() : PumpResult::MakeNoWorkDone();

    // OK, so we have pending 0MQ messages to process!
    // When processing pending messages, we generally favor shipping messages
    // out of the server before we take more in. That way, when under high
    // load, we favor getting smaller before getting bigger.
    //
    // Currently, we only process one socket per call. If we wanted to be
    // exhaustive, we could easily do that.

    zmq::message_t msg;
    int64 more;
    size_t moresz = sizeof(more);
    bool error = false;

    // logging messages have the highest precedence because we want to ensure
    // that all server events, especially errors, have the greatest chance of
    // being recorded
    if (pollitems[LOGGER_INDEX].revents & ZMQ_POLLIN) {
        while (true) {
            if (!this->logger_sock->recv(&msg, 0)) {
                error = true;
                break;
            }

            work_done = true;

            // @todo this should arguably be performed by the dedicated store writer
            if (this->write_logs) {
                this->store->WriteEnvelope(this->log_bucket, this->log_stream_set, msg.data(), msg.size());
            }

            // @todo this is mostly for debugging purposes and should be implemented another way
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

    // move ALL worker responses to client
    else if (pollitems[WORKER_INDEX].revents & ZMQ_POLLIN) {
        while (true) {
            if (!this->workers_sock->recv(&msg, 0)) {
                error = true;
                break;
            }

            moresz = sizeof(more);
            this->workers_sock->getsockopt(ZMQ_RCVMORE, &more, &moresz);
            if (!this->clients_sock->send(msg, more ? ZMQ_SNDMORE : 0)) {
                error = true;
                break;
            }
            work_done = true;

            if (!more) break;
        }

        SendClientMessage log;
        LOG_MESSAGE(log);
    }

    // move streaming messages to client
    else if (pollitems[STREAMING_INDEX].revents & ZMQ_POLLIN) {
        while (true) {
            if (!this->streaming_sock->recv(&msg, 0)) {
                error = true;
                break;
            }

            moresz = sizeof(more);
            this->streaming_sock->getsockopt(ZMQ_RCVMORE, &more, &moresz);
            if (!this->clients_sock->send(msg, more ? ZMQ_SNDMORE : 0)) {
                error = true;
                break;
            }
            work_done = true;

            if (!more) break;
        }

        SendClientMessage log;
        LOG_MESSAGE(log, this->log_client_sock);
    }

    // move subscriptions requests to streamer
    else if (pollitems[WORKER_SUBSCRIPTIONS_INDEX].revents & ZMQ_POLLIN) {
        while (true) {
            if (!this->worker_subscriptions_sock->recv(&msg, 0)) {
                error = true;
                break;
            }

            moresz = sizeof(more);
            this->worker_subscriptions_sock->getsockopt(ZMQ_RCVMORE, &more, &moresz);
            if (!this->streaming_subscriptions_sock->send(msg, more ? ZMQ_SNDMORE : 0)) {
                error = true;
                break;
            }
            work_done = true;

            if (!more) break;
        }
    }

    // move store changes to all subscribed parties
    else if (pollitems[STORE_CHANGES_INPUT_INDEX].revents & ZMQ_POLLIN) {
        while(true) {
            if (!this->store_changes_input_sock->recv(&msg, 0)) {
                error = true;
                break;
            }

            moresz = sizeof(more);
            this->store_changes_input_sock->getsockopt(ZMQ_RCVMORE, &more, &moresz);
            if (!this->store_changes_output_sock->send(msg, more ? ZMQ_SNDMORE : 0)) {
                error = true;
                break;
            }
            work_done = true;

            if (!more) break;
        }
    }

    // move subscription general messages to all streamers
    else if (pollitems[STREAMING_NOTIFY_INDEX].revents & ZMQ_POLLIN) {
        while (true) {
            if (!this->worker_streaming_notify_sock->recv(&msg, 0)) {
                error = true;
                break;
            }

            moresz = sizeof(more);
            this->worker_streaming_notify_sock->getsockopt(ZMQ_RCVMORE, &more, &moresz);
            if (!this->streaming_streaming_notify_sock->send(msg, more ? ZMQ_SNDMORE : 0)) {
                error = true;
                break;
            }
            work_done = true;

            if (!more) break;
        }
    }

    // forward client stream requests to streaming thread pool

    // send client requests to workers
    else if (pollitems[CLIENT_INDEX].revents & ZMQ_POLLIN) {
        while (true) {
            if (!this->clients_sock->recv(&msg, 0)) {
                error = true;
                break;
            }

            moresz = sizeof(more);
            this->clients_sock->getsockopt(ZMQ_RCVMORE, &more, &moresz);
            if (!this->workers_sock->send(msg, more ? ZMQ_SNDMORE : 0)) {
                error = true;
                break;
            }
            work_done = true;

            if (!more) break;
        }

        ReceiveClientMessage log;
        LOG_MESSAGE(log);
    }

    if (error) return PumpResult::MakeError();
    return work_done ? PumpResult::MakeWorkDone() : PumpResult::MakeNoWorkDone();
}

bool Server::Start()
{
    // this isn't fully atomic, but do we care?
    if (this->start_started && !this->initialized) {
        throw Exception("Start() already called but it hasn't finished yet");
    }

    if (this->initialized) return true;

    this->start_started = true;

    this->active.Reset();

    // create our sockets
    this->logger_sock = new socket_t(*this->zctx, ZMQ_PULL);
    this->logger_sock->bind(this->logger_endpoint.c_str());

    this->log_client_sock = new socket_t(*this->zctx, ZMQ_PUSH);
    this->log_client_sock->connect(this->logger_endpoint.c_str());

    this->workers_sock = new socket_t(*this->zctx, ZMQ_XREQ);
    this->workers_sock->bind(this->worker_endpoint.c_str());

    this->streaming_sock = new socket_t(*this->zctx, ZMQ_PULL);
    this->streaming_sock->bind(this->streaming_endpoint.c_str());

    this->worker_subscriptions_sock = new socket_t(*this->zctx, ZMQ_PULL);
    this->worker_subscriptions_sock->bind(this->worker_subscriptions_endpoint.c_str());

    this->worker_streaming_notify_sock = new socket_t(*this->zctx, ZMQ_PULL);
    this->worker_streaming_notify_sock->bind(this->worker_streaming_notify_endpoint.c_str());

    this->streaming_subscriptions_sock = new socket_t(*this->zctx, ZMQ_PUSH);
    this->streaming_subscriptions_sock->bind(this->streaming_subscriptions_endpoint.c_str());

    this->streaming_streaming_notify_sock = new socket_t(*this->zctx, ZMQ_PUB);
    this->streaming_streaming_notify_sock->bind(this->streaming_streaming_notify_endpoint.c_str());

    this->store_changes_input_sock = new socket_t(*this->zctx, ZMQ_PULL);
    this->store_changes_input_sock->bind(this->store_changes_input_endpoint.c_str());

    this->store_changes_output_sock = new socket_t(*this->zctx, ZMQ_PUB);
    this->store_changes_output_sock->bind(this->store_changes_output_endpoint.c_str());

    // now create child threads

    // start with store writer
    StoreWriterStartParams swp;
    swp.ctx = this->zctx;
    swp.active = &this->active;
    swp.store_path = this->store_path;
    swp.envelope_pull_endpoint = this->store_writer_envelope_pull_endpoint;
    swp.envelope_rep_endpoint = this->store_writer_envelope_rep_endpoint;

    this->store_writer = new StoreWriter(swp);
    this->store_writer->RunAsync();

    // store watcher
    StoreWatcherStartParams swparams;
    swparams.active = &this->active;
    swparams.logging_endpoint = this->logger_endpoint;

    // @todo this assumes we're using a file-based store, which is a no-no
    swparams.store_path = ((SimpleDirectoryStore *)(this->store))->RootDirectoryPath();
    swparams.zctx = this->zctx;

    WatcherStartParams wsp;
    wsp.params = swparams;
    wsp.socket_endpoint = this->store_changes_input_endpoint;

    this->store_watcher = new Watcher(wsp);
    this->store_watcher->RunAsync();

    // spin up configured number of request processors
    for (int i = this->number_worker_threads; i; --i) {
        if (!this->CreateRequestProcessorDevice()) return false;
    }

    // and the persisted state reactors
    for (int i = this->number_persisted_reactor_threads; i; --i) {
        if (!this->CreatePersistedStateReactorDevice()) return false;
    }

    // bind sockets to listen for client requests
    this->clients_sock = new socket_t(*this->zctx, ZMQ_XREP);

    // 0MQ sockets can bind to multiple endpoints
    // how AWESOME is that?
    for (size_t i = 0; i < this->listen_endpoints.size(); i++) {
        this->clients_sock->bind(this->listen_endpoints[i].c_str());
    }

    this->pollitems[CLIENT_INDEX].socket = *this->clients_sock;
    this->pollitems[CLIENT_INDEX].events = ZMQ_POLLIN;
    this->pollitems[CLIENT_INDEX].fd = 0;
    this->pollitems[CLIENT_INDEX].revents = 0;

    this->pollitems[WORKER_INDEX].socket = *this->workers_sock;
    this->pollitems[WORKER_INDEX].events = ZMQ_POLLIN;
    this->pollitems[WORKER_INDEX].fd = 0;
    this->pollitems[WORKER_INDEX].revents = 0;

    this->pollitems[STREAMING_INDEX].socket = *this->streaming_sock;
    this->pollitems[STREAMING_INDEX].events = ZMQ_POLLIN;
    this->pollitems[STREAMING_INDEX].fd = 0;
    this->pollitems[STREAMING_INDEX].revents = 0;

    this->pollitems[WORKER_SUBSCRIPTIONS_INDEX].socket = *this->worker_subscriptions_sock;
    this->pollitems[WORKER_SUBSCRIPTIONS_INDEX].events = ZMQ_POLLIN;
    this->pollitems[WORKER_SUBSCRIPTIONS_INDEX].fd = 0;
    this->pollitems[WORKER_SUBSCRIPTIONS_INDEX].revents = 0;

    this->pollitems[STREAMING_NOTIFY_INDEX].socket = *this->worker_streaming_notify_sock;
    this->pollitems[STREAMING_NOTIFY_INDEX].events = ZMQ_POLLIN;
    this->pollitems[STREAMING_NOTIFY_INDEX].fd = 0;
    this->pollitems[STREAMING_NOTIFY_INDEX].revents = 0;

    this->pollitems[STORE_CHANGES_INPUT_INDEX].socket = *this->store_changes_input_sock;
    this->pollitems[STORE_CHANGES_INPUT_INDEX].events = ZMQ_POLLIN;
    this->pollitems[STORE_CHANGES_INPUT_INDEX].fd = 0;
    this->pollitems[STORE_CHANGES_INPUT_INDEX].revents = 0;

    this->pollitems[LOGGER_INDEX].socket = *this->logger_sock;
    this->pollitems[LOGGER_INDEX].events = ZMQ_POLLIN;
    this->pollitems[LOGGER_INDEX].fd = 0;
    this->pollitems[LOGGER_INDEX].revents = 0;

    // start background timers
    if (!this->stream_flush_timer.Start()) {
        throw Exception("could not start stream flush timer");
    }

    if (!this->thread_check_timer.Start()) {
        throw Exception("TODO handle failure to start time");
    }

    this->initialized = true;

    return true;
}

bool Server::CreateRequestProcessorDevice()
{
    ::zippylog::RequestProcessorStartParams params;
    params.active = &this->active;
    params.ctx = this->zctx;
    params.client_endpoint = this->worker_endpoint;
    params.logger_endpoint = this->logger_endpoint;
    params.store_path = this->store_path;

    ServerRequestProcessorStartParams p;
    p.request_processor_params = params;
    p.streaming_subscriptions_endpoint = this->worker_subscriptions_endpoint;
    p.streaming_updates_endpoint = this->worker_streaming_notify_endpoint;
    p.store_writer_envelope_pull_endpoint = this->store_writer_envelope_pull_endpoint;
    p.store_writer_envelope_rep_endpoint = this->store_writer_envelope_rep_endpoint;

    ServerRequestProcessor *srp = new ServerRequestProcessor(p);
    params.implementation = srp;

    RequestProcessor *rp = new RequestProcessor(params);
    rp->RunAsync();

    this->request_processors.push_back(rp);

    return true;
}

bool Server::CreatePersistedStateReactorDevice()
{
    PersistedStateReactorStartParams p;
    p.client_endpoint = this->streaming_endpoint;
    p.store_change_endpoint = this->store_changes_output_endpoint;
    p.logger_endpoint = this->logger_endpoint;
    p.subscription_endpoint = this->streaming_subscriptions_endpoint;
    p.subscription_updates_endpoint = this->streaming_streaming_notify_endpoint;

    p.ctx = this->zctx;
    p.active = &this->active;

    p.manager_params.store_uri = this->store_path;
    p.manager_params.subscription_ttl = this->subscription_ttl;
    p.manager_params.subscription_lua_allow = this->lua_execute_client_code;
    p.manager_params.subscription_lua_memory_ceiling = this->lua_streaming_max_memory;

    PersistedStateReactor *r = new PersistedStateReactor(p);
    r->RunAsync();

    this->persisted_state_reactors.push_back(r);

    return true;
}

void Server::CheckDevices()
{
    if (!this->store_writer || !this->store_writer->IsRunning()) {
        this->Shutdown();
        return;
    }

    if (!this->store_watcher || !this->store_watcher->IsRunning()) {
        this->Shutdown();
        return;
    }

    for (size_t i = 0; i < this->request_processors.size(); i++) {
        if (!this->request_processors[i]->IsRunning()) {
            this->Shutdown();
            return;
        }
    }

    for (size_t i = 0; i < this->persisted_state_reactors.size(); i++) {
        if (!this->persisted_state_reactors[i]->IsRunning()) {
            this->Shutdown();
            return;
        }
    }
}

// @todo consider flushing log socket on shutdown
void Server::Shutdown()
{
    this->active.Signal();

    // wait for request processors to finish
    for (vector<RequestProcessor *>::iterator i = this->request_processors.begin();
         i != this->request_processors.end();
         i++)
    {
        (*i)->StopAsync();
        delete *i;
        *i = NULL;
    }
    this->request_processors.clear();

    // wait for persisted state reactors to finish
    for (vector<PersistedStateReactor *>::iterator i = this->persisted_state_reactors.begin();
        i != this->persisted_state_reactors.end();
        i++) {
        (*i)->StopAsync();
        delete *i;
        *i = NULL;
    }
    this->persisted_state_reactors.clear();

    if (this->store_watcher) {
        this->store_watcher->StopAsync();
        delete this->store_watcher;
        this->store_watcher = NULL;
    }

    if (this->store_writer) {
        this->store_writer->StopAsync();
        delete this->store_writer;
        this->store_writer = NULL;
    }
}

bool Server::ParseConfig(const string path, ServerStartParams &params, string &error)
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
    params.store_path = lua_tostring(L, -1);
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
        params.listen_endpoints.push_back(lua_tostring(L, -1));
        lua_pop(L, 1);
    }

    // number of worker threads to run
    lua_getglobal(L, "worker_threads");
    params.worker_threads = luaL_optinteger(L, -1, ::zippylog::server_default_worker_threads);
    lua_pop(L, 1);

    // number of streaming threads to run
    lua_getglobal(L, "persisted_state_reactor_threads");
    params.persisted_state_reactor_threads = luaL_optinteger(L, -1, ::zippylog::server_default_persisted_state_reactor_threads);
    lua_pop(L, 1);

    // time to live of streaming subscriptions in milliseconds
    lua_getglobal(L, "streaming_subscription_ttl");
    params.subscription_ttl = luaL_optinteger(L, -1, ::zippylog::server_default_subscription_ttl);
    lua_pop(L, 1);

    // logging settings
    lua_getglobal(L, "log_bucket");
    params.log_bucket = luaL_optstring(L, -1, ::zippylog::server_default_log_bucket.c_str());
    lua_pop(L, 1);

    lua_getglobal(L, "log_stream_set");
    params.log_stream_set = luaL_optstring(L, -1, ::zippylog::server_default_log_bucket.c_str());
    lua_pop(L, 1);

    // interval at which to flush streams in milliseconds
    lua_getglobal(L, "stream_flush_interval");
    params.stream_flush_interval = luaL_optinteger(L, -1, ::zippylog::server_default_stream_flush_interval);
    lua_pop(L, 1);
    if (params.stream_flush_interval < 0) {
        os << "stream_flush_interval must be positive";
        goto cleanup;
    }
    else if (params.stream_flush_interval < 1000) {
        os << "stream_flush_interval must be greater than 1000";
        goto cleanup;
    }

    // Lua settings
    lua_getglobal(L, "lua_execute_client_code");
    params.lua_execute_client_code = lua_toboolean(L, -1);
    lua_pop(L, 1);

    lua_getglobal(L, "lua_streaming_max_memory");
    params.lua_streaming_max_memory = luaL_optinteger(L, -1, ::zippylog::server_default_lua_streaming_max_memory);
    lua_pop(L, 1);


cleanup:
    lua_close(L);

    if (os.str().length()) {
        error = os.str();
        return false;
    }

    return true;
}

}} // namespaces
