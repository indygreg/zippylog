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

#include <zippylog/zippylogd/worker.hpp>
#include <zippylog/zippylogd.pb.h>
#include <zippylog/zeromq.hpp>

namespace zippylog {
namespace zippylogd {

#define LOG_MESSAGE(msgvar, socketvar) { \
    msgvar.set_id(this->id); \
    Envelope logenvelope = Envelope(); \
    msgvar.add_to_envelope(&logenvelope); \
    zeromq::send_envelope(socketvar, logenvelope); \
}

using ::zippylog::RequestProcessor;
using ::zmq::socket_t;

Worker::Worker(WorkerStartParams &params) :
    streaming_subscriptions_endpoint(params.streaming_subscriptions_endpoint),
    streaming_updates_endpoint(params.streaming_updates_endpoint),
    RequestProcessor(params.request_processor_params),
    subscriptions_sock(NULL),
    subscription_updates_sock(NULL)
{
    this->subscriptions_sock = new socket_t(*this->ctx, ZMQ_PUSH);
    this->subscriptions_sock->connect(this->streaming_subscriptions_endpoint.c_str());

    this->subscription_updates_sock = new socket_t(*this->ctx, ZMQ_PUSH);
    this->subscription_updates_sock->connect(this->streaming_updates_endpoint.c_str());
}

Worker::~Worker()
{
    if (this->subscriptions_sock) delete this->subscriptions_sock;
    if (this->subscription_updates_sock) delete this->subscription_updates_sock;
}

RequestProcessor::ResponseStatus Worker::HandleSubscribeStoreChanges(Envelope &request, vector<Envelope> &output)
{
    // we pass the identities and the original message to the streamer
    // we don't pass the first identity, b/c it belongs to the local socket
    // TODO should probably create a special message type with identities embed
    vector<string> subscription_identities = this->current_request_identities;
    subscription_identities.erase(subscription_identities.begin());

    ::zippylog::zeromq::send_envelope(this->subscriptions_sock, subscription_identities, request);

    return DEFERRED;
}

RequestProcessor::ResponseStatus Worker::HandleSubscribeEnvelopes(Envelope &request, vector<Envelope> &output)
{
    // proxy the message w/ identities (minus first one)
    vector<string> subscription_identities = this->current_request_identities;
    subscription_identities.erase(subscription_identities.begin());

    ::zippylog::zeromq::send_envelope(this->subscriptions_sock, subscription_identities, request);

    return DEFERRED;
}

RequestProcessor::ResponseStatus Worker::HandleSubscribeKeepalive(Envelope &request, vector<Envelope> &output)
{
    ::zippylog::zeromq::send_envelope(this->subscription_updates_sock, request);

    return DEFERRED;
}



}} // namespaces
