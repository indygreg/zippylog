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

#include <lua.h>
#include <lauxlib.h>

#include <sstream>


namespace zippylog {
namespace server {

using ::std::ostringstream;

#define WORKER_ENDPOINT "inproc://workers"
#define CLIENTS_ENDPOINT "inproc://clients"
#define STORE_WATCHER_CHANGE_ENDPOINT "inproc://store_changes"

#define CLIENTS_EXTERNAL_INDEX 0
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
    this->clients_external_sock = NULL;
    this->store = NULL;
    this->store_watcher_thread = NULL;
    this->store_watcher_start = NULL;
}

/*
The broker is kinda a gnarly beast.

It binds to 1 to many listener interfaces. These are defined at run-time.
Each listener socket has a "proxy" socket associated with it. This proxy socket
is connected to a virtual client socket. This allows us to do message routing at
the 0MQ layer through identities automagically.

The clients socket, which encapsulates all listener sockets by extension, reads
client messages and forwards them to the worker socket. This socket has a number
of threads connected to it at the remote end. It is the workers' job to process
client messages.

If a worker sends a response, we grab it and forward it back to the client, via
the proxy sockets.

Some requests like streaming are processed by non-worker threads. In these
cases, the workers deposit a set of messages on some internal sockets. The
broker consumes these messages and forwards them to the appropriate
socket connected to the other thread or thread pool.

The requests processed by non-worker threads deposit messages on their own
inpoc sockets. These sockets are read by the broker and messages are forwarded
to clients, as appropriate.

TODO sockets can bind to multiple endpoints, duh

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

    int number_pollitems = LISTENER_INDEX + 2 * this->listen_sockets.size();
    int number_listeners = this->listen_sockets.size();
    zmq::pollitem_t* pollitems = new zmq::pollitem_t[number_pollitems];

    pollitems[CLIENTS_EXTERNAL_INDEX].socket = *this->clients_external_sock;
    pollitems[CLIENTS_EXTERNAL_INDEX].events = ZMQ_POLLIN;
    pollitems[CLIENTS_EXTERNAL_INDEX].fd = 0;
    pollitems[CLIENTS_EXTERNAL_INDEX].revents = 0;

    pollitems[WORKER_INDEX].socket = *this->workers_sock;
    pollitems[WORKER_INDEX].events = ZMQ_POLLIN;
    pollitems[WORKER_INDEX].fd = 0;
    pollitems[WORKER_INDEX].revents = 0;

    for (int i = 0; i < this->listen_sockets.size(); i++) {
        pollitems[LISTENER_INDEX+i].socket = *this->listen_sockets[i];
        pollitems[LISTENER_INDEX+i].events = ZMQ_POLLIN;
        pollitems[LISTENER_INDEX+i].fd = 0;
        pollitems[LISTENER_INDEX+i].revents = 0;

        pollitems[LISTENER_INDEX+i+number_listeners].socket = *this->listen_proxy_sockets[i];
        pollitems[LISTENER_INDEX+i+number_listeners].events = ZMQ_POLLIN;
        pollitems[LISTENER_INDEX+i+number_listeners].fd = 0;
        pollitems[LISTENER_INDEX+i+number_listeners].revents = 0;
    }

    zmq::message_t msg;
    int64 more;
    size_t moresz = sizeof(more);

    // TODO so much repetition here. it makes me feel dirty
    // TODO better error handling
    while (this->active) {
        // wait for a message to become available
        int rc = zmq::poll(pollitems, number_pollitems, -1);

        // move worker responses to clients sock
        if (pollitems[WORKER_INDEX].revents & ZMQ_POLLIN) {
            while (true) {
                if (!this->workers_sock->recv(&msg, 0)) {
                    break;
                }

                moresz = sizeof(more);
                this->workers_sock->getsockopt(ZMQ_RCVMORE, &more, &moresz);
                if (!this->clients_external_sock->send(msg, more ? ZMQ_SNDMORE : 0)) {
                    break;
                }

                if (!more) break;
            }
        }

        // move from client meta socket to workers
        if (pollitems[CLIENTS_EXTERNAL_INDEX].revents & ZMQ_POLLIN) {
            while (true) {
                if (!this->clients_external_sock->recv(&msg, 0)) {
                    break;
                }

                moresz = sizeof(more);
                this->clients_external_sock->getsockopt(ZMQ_RCVMORE, &more, &moresz);
                if (!this->workers_sock->send(msg, more ? ZMQ_SNDMORE : 0)) {
                    break;
                }

                if (!more) break;
            }
        }

        for (int i = 0; i < number_listeners; i++) {
            // move our responses down to listener
            if (pollitems[LISTENER_INDEX + i + number_listeners].revents & ZMQ_POLLIN) {
                while (true) {
                    if (!this->listen_proxy_sockets[i]->recv(&msg, 0)) {
                        break;
                    }

                    moresz = sizeof(more);
                    this->listen_proxy_sockets[i]->getsockopt(ZMQ_RCVMORE, &more, &moresz);
                    if (!this->listen_sockets[i]->send(msg, more ? ZMQ_SNDMORE : 0)) {
                        break;
                    }
                    if (!more) break;
                }
            }

            // move client requests to the client proxy virtual device
            if (pollitems[LISTENER_INDEX + i].revents & ZMQ_POLLIN) {
                while (true) {
                    if (!this->listen_sockets[i]->recv(&msg, 0)) {
                        break;
                    }

                    moresz = sizeof(more);
                    this->listen_sockets[i]->getsockopt(ZMQ_RCVMORE, &more, &moresz);

                    if (!this->listen_proxy_sockets[i]->send(msg, more ? ZMQ_SNDMORE : 0)) {
                        break;
                    }

                    if (!more) break;
                }
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

    for (int i = 3; i; --i) {
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
    this->clients_external_sock = new zmq::socket_t(*this->zctx, ZMQ_XREP);
    this->clients_external_sock->bind(CLIENTS_ENDPOINT);

    this->workers_sock = new zmq::socket_t(*this->zctx, ZMQ_XREQ);
    this->workers_sock->bind(WORKER_ENDPOINT);
}

void Broker::setup_listener_sockets()
{
    for (int i = 0; i < this->config.listen_endpoints.size(); i++) {
        zmq::socket_t *s = new zmq::socket_t(*this->zctx, ZMQ_XREP);
        s->bind(this->config.listen_endpoints[i].c_str());
        this->listen_sockets.push_back(s);

        zmq::socket_t *p = new zmq::socket_t(*this->zctx, ZMQ_XREQ);
        p->connect(CLIENTS_ENDPOINT);
        this->listen_proxy_sockets.push_back(p);
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
