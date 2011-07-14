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

#include <zippylog/device/persisted_state_reactor.hpp>

#include <zippylog/platform.hpp>

using ::std::invalid_argument;
using ::zmq::context_t;
using ::zmq::message_t;
using ::zmq::socket_t;

namespace zippylog {
namespace device {

PersistedStateReactor::PersistedStateReactor(const PersistedStateReactorStartParams &params) :
    ctx(params.ctx),
    store_changes_endpoint(params.store_change_endpoint),
    client_endpoint(params.client_endpoint),
    subscription_endpoint(params.subscription_endpoint),
    subscription_updates_endpoint(params.subscription_updates_endpoint),
    logger_endpoint(params.logger_endpoint),
    active(params.active),
    manager_params(params.manager_params),
    store_changes_sock(NULL),
    client_sock(NULL),
    subscription_sock(NULL),
    subscription_updates_sock(NULL),
    logger_sock(NULL),
    manager(NULL)
{
    if (!this->ctx) throw invalid_argument("0MQ context must be defined");
    if (!this->active) throw invalid_argument("active semaphore must be defined");

    if (this->store_changes_endpoint.empty()) throw invalid_argument("store changes endpoint not defined");
    if (this->client_endpoint.empty()) throw invalid_argument("client endpoint not defined");
    if (this->subscription_endpoint.empty()) throw invalid_argument("subscription endpoint not defined");
    if (this->subscription_updates_endpoint.empty()) throw invalid_argument("subscription updates endpoint not defined");
    if (this->logger_endpoint.empty()) throw invalid_argument("logger endpoint not defined");

    this->Initialize();
}

PersistedStateReactor::~PersistedStateReactor()
{
    if (this->store_changes_sock) delete this->store_changes_sock;
    if (this->client_sock) delete this->client_sock;
    if (this->subscription_sock) delete this->subscription_sock;
    if (this->subscription_updates_sock) delete this->subscription_updates_sock;
    if (this->logger_sock) delete this->logger_sock;

    if (this->manager) delete this->manager;
}

void PersistedStateReactor::Initialize()
{
    this->id = platform::CreateUUID();

    this->manager = new PersistedStateManager(this->manager_params);

    this->logger_sock = new socket_t(*this->ctx, ZMQ_PUSH);
    this->logger_sock->connect(this->logger_endpoint.c_str());

    this->client_sock = new socket_t(*this->ctx, ZMQ_PUSH);
    this->client_sock->connect(this->client_endpoint.c_str());

    this->store_changes_sock = new socket_t(*this->ctx, ZMQ_SUB);
    this->store_changes_sock->setsockopt(ZMQ_SUBSCRIBE, NULL, 0);
    this->store_changes_sock->connect(this->store_changes_endpoint.c_str());

    this->subscription_sock = new socket_t(*this->ctx, ZMQ_PULL);
    this->subscription_sock->connect(this->subscription_endpoint.c_str());

    this->subscription_updates_sock = new socket_t(*this->ctx, ZMQ_SUB);
    this->subscription_updates_sock->connect(this->subscription_updates_endpoint.c_str());
    this->subscription_updates_sock->setsockopt(ZMQ_SUBSCRIBE, NULL, 0);

    this->pollitems[0].events = ZMQ_POLLIN;
    this->pollitems[0].socket = *this->store_changes_sock;
    this->pollitems[0].fd = 0;
    this->pollitems[0].revents = 0;

    this->pollitems[1].events = ZMQ_POLLIN;
    this->pollitems[1].socket = *this->subscription_sock;
    this->pollitems[1].fd = 0;
    this->pollitems[1].revents = 0;

    this->pollitems[2].events = ZMQ_POLLIN;
    this->pollitems[2].socket = *this->subscription_updates_sock;
    this->pollitems[2].fd = 0;
    this->pollitems[2].revents = 0;
}

void PersistedStateReactor::Pump(int32 timeout)
{
    // wait for a message to process
    int result = zmq::poll(&this->pollitems[0], 3, timeout);

    // if no data, perform house keeping and nothing else
    if (!result) {
        this->manager->RemoveExpiredSubscriptions();
        return;
    }

    if (this->pollitems[2].revents & ZMQ_POLLIN) {
        /// @todo receive message and process subscription update
    }

    // we wait until after processing updates to perform housekeeping, as one
    // of those updates may have been overdue and could save an expired object
    // from being reaped
    this->manager->RemoveExpiredSubscriptions();

    message_t msg;

    // subscriptions
    if (this->pollitems[1].revents & ZMQ_POLLIN) {
        if (!this->subscription_sock->recv(&msg, 0)) {
            throw Exception("error receiving 0MQ message on subscriptions sock");
        }

        if (msg.size() != sizeof(SubscriptionInfo *)) {
            throw Exception("SubscriptionInfo message not a pointer!");
        }

        SubscriptionInfo **subscription = (SubscriptionInfo **)msg.data();

        /// @todo handle errors?
        this->manager->RegisterSubscription(*subscription);
    }

    // process store changes and send to subscribers
    if (pollitems[0].revents & ZMQ_POLLIN) {
        // @todo error checking
        this->subscription_updates_sock->recv(&msg, 0);

        /// @todo Send to manager
    }
}

void PersistedStateReactor::Run()
{
    while(*this->active) {
        this->Pump(100000);
    }
}

}} // namespaces