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
#include <zippylog/zippylogd.pb.h>
#include <zippylog/zeromq.hpp>

namespace zippylog {
namespace device {
namespace server {

#define LOG_MESSAGE(msgvar, socketvar) { \
    msgvar.set_id(this->id); \
    Envelope logenvelope = Envelope(); \
    msgvar.add_to_envelope(&logenvelope); \
    zeromq::send_envelope(socketvar, logenvelope); \
}

using ::std::string;
using ::std::vector;
using ::zippylog::RequestProcessor;
using ::zippylog::SubscriptionInfo;
using ::zmq::socket_t;
using ::zmq::message_t;

Worker::Worker(WorkerStartParams &params) :
    ctx(params.request_processor_params.ctx),
    streaming_subscriptions_endpoint(params.streaming_subscriptions_endpoint),
    streaming_updates_endpoint(params.streaming_updates_endpoint),
    subscriptions_sock(NULL),
    subscription_updates_sock(NULL),
    store_sender(NULL)
{
    this->subscriptions_sock = new socket_t(*this->ctx, ZMQ_PUSH);
    this->subscriptions_sock->connect(this->streaming_subscriptions_endpoint.c_str());

    this->subscription_updates_sock = new socket_t(*this->ctx, ZMQ_PUSH);
    this->subscription_updates_sock->connect(this->streaming_updates_endpoint.c_str());

    StoreWriterSenderStartParams swsp;
    swsp.ctx = this->ctx;
    swsp.envelope_pull_endpoint = params.store_writer_envelope_pull_endpoint;
    swsp.envelope_rep_endpoint = params.store_writer_envelope_rep_endpoint;

    this->store_sender = new StoreWriterSender(swsp);
}

Worker::~Worker()
{
    if (this->store_sender) delete this->store_sender;
    if (this->subscriptions_sock) delete this->subscriptions_sock;
    if (this->subscription_updates_sock) delete this->subscription_updates_sock;
}

RequestProcessorResponseStatus Worker::HandleSubscribeKeepalive(Envelope &request, vector<Envelope> &)
{
    ::zippylog::zeromq::send_envelope(this->subscription_updates_sock, request);

    return DEFERRED;
}

HandleSubscriptionResult Worker::HandleSubscriptionRequest(zippylog::SubscriptionInfo subscription)
{
    HandleSubscriptionResult result;
    result.id = platform::CreateUUID(false);
    result.result = HandleSubscriptionResult::ACCEPTED;
    subscription.id = result.id;

    // we send the raw class through the socket
    /// @todo serialize to protocol buffer message
    message_t m(sizeof(subscription));
    memcpy(m.data(), &subscription, sizeof(subscription));

    this->subscriptions_sock->send(m, 0);

    return result;
}

int Worker::HandleWriteEnvelopes(::std::string const &, ::std::vector<Envelope> &, bool)
{
    /// @todo implement
    throw Exception("TODO implement Worker::HandleWriteEnvelopes");
}

}}} // namespaces
