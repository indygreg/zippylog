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

#define WORKER_ENDPOINT "inproc://workers"
#define STORE_WATCHER_CHANGE_ENDPOINT "inproc://store_changes"

#define CLIENT_INDEX 0
#define WORKER_INDEX 1
#define LISTENER_INDEX 2

Broker::Broker(string config_file_path)
{
    this->init();

    string error;
    if (!ParseConfig(config_file_path, this->config, error)) {
        throw error;
    }

    this->store = new Store(this->config.store_path);
}

Broker::Broker(Store *store)
{
    Broker(store, NULL);
}

Broker::Broker(Store * store, context_t *ctx)
{
    this->init();

    this->zctx = ctx;
    this->store = store;
}

Broker::~Broker()
{
    if (this->zctx) delete this->zctx;
    if (this->worker_start_data) delete this->worker_start_data;
    if (this->store_watcher_start) delete this->store_watcher_start;

    // TODO clean up store if it was allocated by us
}

void Broker::init()
{
    this->active = true;
    this->zctx = NULL;
    this->worker_start_data = NULL;
    this->workers_sock = NULL;
    this->clients_sock = NULL;
    this->store = NULL;
    this->store_watcher_thread = NULL;
    this->store_watcher_start = NULL;
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
    if (!this->zctx) {
        this->zctx = new zmq::context_t(1);
    }

    this->setup_internal_sockets();
    this->create_worker_threads();
    this->create_store_watcher();
    this->setup_listener_sockets();

    int number_pollitems = LISTENER_INDEX;
    zmq::pollitem_t* pollitems = new zmq::pollitem_t[number_pollitems];

    pollitems[CLIENT_INDEX].socket = *this->clients_sock;
    pollitems[CLIENT_INDEX].events = ZMQ_POLLIN;
    pollitems[CLIENT_INDEX].fd = 0;
    pollitems[CLIENT_INDEX].revents = 0;

    pollitems[WORKER_INDEX].socket = *this->workers_sock;
    pollitems[WORKER_INDEX].events = ZMQ_POLLIN;
    pollitems[WORKER_INDEX].fd = 0;
    pollitems[WORKER_INDEX].revents = 0;

    zmq::message_t msg;
    int64 more;
    size_t moresz = sizeof(more);

    // TODO so much repetition here. it makes me feel dirty
    // TODO better error handling
    while (this->active) {
        // wait for a message to become available
        int rc = zmq::poll(pollitems, number_pollitems, -1);

        // we always favor shipping messages OUT of the server to the clients
        // that way, under heavy load, we try to get smaller before we
        // get bigger

        // move worker responses to client
        if (pollitems[WORKER_INDEX].revents & ZMQ_POLLIN) {
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

        // send client requests to workers
        if (pollitems[CLIENT_INDEX].revents & ZMQ_POLLIN) {
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

    delete pollitems;
}

void Broker::create_worker_threads()
{
    this->worker_start_data = new request_processor_start_data;
    this->worker_start_data->ctx = this->zctx;
    this->worker_start_data->store = this->store;
    this->worker_start_data->broker_endpoint = WORKER_ENDPOINT;

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
    this->store_watcher_start->endpoint = STORE_WATCHER_CHANGE_ENDPOINT;
    this->store_watcher_start->zctx = this->zctx;
    this->store_watcher_start->store = this->store;

    this->store_watcher_thread = create_thread(StoreWatcherStart, this->store_watcher_start);
}

void Broker::setup_internal_sockets()
{
    this->workers_sock = new zmq::socket_t(*this->zctx, ZMQ_XREQ);
    this->workers_sock->bind(WORKER_ENDPOINT);
}

void Broker::setup_listener_sockets()
{
    this->clients_sock = new zmq::socket_t(*this->zctx, ZMQ_XREP);

    // 0MQ sockets can bind to multiple endpoints
    // how AWESOME is that?
    for (int i = 0; i < this->config.listen_endpoints.size(); i++) {
        this->clients_sock->bind(this->config.listen_endpoints[i].c_str());
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

void * __stdcall Broker::StoreWatcherStart(void *d)
{
    store_watcher_start_data * data = (store_watcher_start_data *)d;

    assert(data->endpoint);
    assert(data->zctx);
    assert(data->store);

    StoreWatcher watcher = StoreWatcher(data->store, data->zctx, data->endpoint);
    watcher.run();

    return NULL;
}

}} // namespaces
