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

#include <zippylog/zippylogd/request_processor.cpp>

namespace zippylog {
namespace zippylogd {

RequestProcessor::RequestProcessor(RequestProcessorStartParams params) :
    ::zippylog::server::RequestProcessor(params.store_path, params.ctx, params.logger_endpoint),
    socket(NULL),
    logger_sock(NULL),
    subscriptions_sock(NULL),
    subscription_updates_sock(NULL)
{
    if (!params.active) {
        throw "active parameter cannot be null";
    }

    this->broker_endpoint = params.broker_endpoint;
    this->streaming_subscriptions_endpoint = params.streaming_subscriptions_endpoint;
    this->streaming_updates_endpoint = params.streaming_updates_endpoint;
    this->active = params.active;

    this->subscriptions_sock = new socket_t(*this->ctx, ZMQ_PUSH);
    this->subscriptions_sock->connect(this->streaming_subscriptions_endpoint.c_str());

    this->subscription_updates_sock = new socket_t(*this->ctx, ZMQ_PUSH);
    this->subscription_updates_sock->connect(this->streaming_updates_endpoint.c_str());
}

RequestProcessor::~RequestProcessor()
{
    if (this->subscriptions_sock) delete this->subscriptions_sock;
    if (this->subscription_updates_sock) delete this->subscription_updates_sock;
}

ResponseStatus RequestProcessor::HandleSubscribeStoreInfo(Envelope &request, vector<Envelope> &output)
{
    // we pass the identities and the original message to the streamer
    // we don't pass the first identity, b/c it belongs to the local socket
    // TODO should probably create a special message type with identities embedded
    vector<string> subscription_identities = this->current_request_identities;
    subscription_identities.erase(subscription_identities.begin());

    zeromq::send_envelope(this->subscriptions_sock, subscription_identities, request);

    return DEFERRED;
}

ResponseStatus RequestProcessor::HandleSubscribeEnvelopes(Envelope &request, vector<Envelope> &output)
{
    // proxy the message w/ identities (minus first one)
    vector<string> subscription_identities = this->current_request_identities;
    subscription_identities.erase(subscription_identities.begin());

    zeromq::send_envelope(this->subscriptions_sock, subscription_identities, request);

    return DEFERRED;
}

ResponseStatus RequestProcessor::HandleSubscribeKeepalive(Envelope &request, vector<Envelope> &output)
{
    WorkerForwardSubscribeKeepalive log = WorkerForwardSubscribeKeepalive();
    log.set_subscription(m->id());
    LOG_MESSAGE(log, this->logger_sock);

    zeromq::send_envelope(this->subscription_updates_sock, request);

    return DEFERRED;
}

}} // namespaces