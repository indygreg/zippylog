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

/*

This file implements routines for the server-side processing of client
requests.

The main worker routine implements a crude state machine to help make things
easier to grok.

*/

#include <zippylog/request_processor.hpp>

#include <zippylog/message.pb.h>
#include <zippylog/protocol.pb.h>
#include <zippylog/protocol/request.pb.h>
#include <zippylog/protocol/response.pb.h>
#include <zippylog/zeromq.hpp>
#include <zippylog/zippylogd.pb.h>

#include <string>
#include <zmq.hpp>

namespace zippylog {
namespace server {

#define LOG_MESSAGE(msgvar, socketvar) { \
    msgvar.set_id(this->id); \
    Envelope logenvelope = Envelope(); \
    msgvar.add_to_envelope(&logenvelope); \
    zeromq::send_envelope(socketvar, logenvelope); \
}

using ::std::string;
using ::zippylog::zippylogd::WorkerForwardSubscribeKeepalive;
using ::zippylog::zippylogd::WorkerReceiveUnknownRequestType;
using ::zmq::message_t;
using ::zmq::socket_t;

RequestProcessor::RequestProcessor(RequestProcessorStartParams params) :
    store(params.store_path),
    socket(NULL),
    logger_sock(NULL),
    subscriptions_sock(NULL),
    subscription_updates_sock(NULL)
{
    if (!params.active) {
        throw "active parameter cannot be null";
    }

    this->ctx = params.ctx;
    this->store_path = params.store_path;
    this->broker_endpoint = params.broker_endpoint;
    this->streaming_subscriptions_endpoint = params.streaming_subscriptions_endpoint;
    this->streaming_updates_endpoint = params.streaming_updates_endpoint;
    this->logger_endpoint = params.logger_endpoint;
    this->active = params.active;

    platform::UUID uuid;
    platform::CreateUUID(uuid);

    this->id = string((const char *)&uuid, sizeof(uuid));
}

RequestProcessor::~RequestProcessor()
{
    if (this->socket) delete this->socket;
    if (this->subscriptions_sock) delete this->subscriptions_sock;
    if (this->subscription_updates_sock) delete this->subscription_updates_sock;
    if (this->logger_sock) delete this->logger_sock;
}

// TODO break function into multiple functions
void RequestProcessor::Run()
{
    this->logger_sock = new socket_t(*this->ctx, ZMQ_PUSH);
    this->logger_sock->connect(this->logger_endpoint.c_str());
    {
        ::zippylog::zippylogd::WorkerStartup log = ::zippylog::zippylogd::WorkerStartup();
        LOG_MESSAGE(log, this->logger_sock);
    }

    this->subscriptions_sock = new socket_t(*this->ctx, ZMQ_PUSH);
    this->subscriptions_sock->connect(this->streaming_subscriptions_endpoint.c_str());

    this->subscription_updates_sock = new socket_t(*this->ctx, ZMQ_PUSH);
    this->subscription_updates_sock->connect(this->streaming_updates_endpoint.c_str());

    this->socket = new socket_t(*this->ctx, ZMQ_XREP);
    this->socket->connect(this->broker_endpoint.c_str());

    zmq::pollitem_t pollitems[1];
    pollitems[0].events = ZMQ_POLLIN;
    pollitems[0].fd = 0;
    pollitems[0].revents = 0;
    pollitems[0].socket = *this->socket;

    /* common variables used frequently enough to warrant declaration */
    int64 more;
    size_t moresz = sizeof(more);

    while (*this->active) {
        // wait up to 250ms for a message to become available
        // if not, check the active semaphore and try again
        if (!zmq::poll(&pollitems[0], 1, 250000)) {
            continue;
        }

        // we have a message available, so we receive it
        vector<message_t *> msgs;
        this->current_request_identities.clear();
        if (!zeromq::receive_multipart_message(this->socket, this->current_request_identities,msgs)) {
            ::zippylog::zippylogd::WorkerFailReceiveMessage log = ::zippylog::zippylogd::WorkerFailReceiveMessage();
            LOG_MESSAGE(log, this->logger_sock);

            // TODO shouldn't this be part of the receive_multipart_message API?
            vector<message_t *>::iterator iter = msgs.begin();
            for (; iter != msgs.end(); iter++) {
                delete *iter;
            }

            // TODO we used to rebuild socket here. is this a good recovery strategy?
            // this exception is here so we can see if this actually happens
            // I suspect it doesn't
            throw "error receiving multipart message from worker sock";
        }

        assert(msgs.size());

        // an empty initial message is very weird
        if (!msgs[0]->size()) {
            ::zippylog::zippylogd::WorkerReceiveEmptyMessage log = ::zippylog::zippylogd::WorkerReceiveEmptyMessage();
            LOG_MESSAGE(log, this->logger_sock);

            // TODO if there are identities, we should probably send an error response
            continue;
        }

        vector<Envelope> response_envelopes;
        ResponseStatus result = this->ProcessRequest(this->current_request_identities, msgs, response_envelopes);

        // destroy input messages immediately since they aren't needed any more
        vector<message_t *>::iterator iter = msgs.begin();
        for (; iter != msgs.end(); iter++) {
            delete *iter;
        }

        if (result == AUTHORITATIVE) {
            assert(response_envelopes.size());

            if (!zeromq::send_envelopes(this->socket, this->current_request_identities, response_envelopes)) {
                // TODO log error
                throw "error sending envelopes to client";
                continue;
            }
        }
        else if (result == DEFERRED) {
            // we have nothing to send to the client here
            continue;
        }
        else if (result == PROCESS_ERROR) {
            // TODO log and recover better
            throw "error when processing request";
        }
    }

    ::zippylog::zippylogd::WorkerShutdown log = ::zippylog::zippylogd::WorkerShutdown();
    LOG_MESSAGE(log, this->logger_sock);

    return;
}

RequestProcessor::ResponseStatus RequestProcessor::ProcessRequest(const vector<string> &identities, const vector<::zmq::message_t *> &input, vector<Envelope> &output)
{
    // no input is very funky
    assert(input.size());
    assert(input[0]->size());

    Envelope request_envelope = Envelope(input[0]);
    // TODO handle parse failure

    RequestProcessor::ResponseStatus result;

    if (request_envelope.envelope.message_size() < 1) {
        ::zippylog::zippylogd::WorkerRequestEmptyEnvelope log = ::zippylog::zippylogd::WorkerRequestEmptyEnvelope();
        LOG_MESSAGE(log, this->logger_sock);

        this->PopulateErrorResponse(
            protocol::response::EMPTY_ENVELOPE,
            "envelope contains no messages",
            output
        );
        result = AUTHORITATIVE;
        goto SEND_RESPONSE;
    }

    if (request_envelope.envelope.message_namespace_size() < 1 || request_envelope.envelope.message_type_size() < 1) {
        ::zippylog::zippylogd::WorkerInvalidMessageEnumeration log = ::zippylog::zippylogd::WorkerInvalidMessageEnumeration();
        LOG_MESSAGE(log, this->logger_sock);

        this->PopulateErrorResponse(
            protocol::response::MISSING_ENUMERATIONS,
            "message received without namespace or type enumerations",
            output
        );
        result = AUTHORITATIVE;
        goto SEND_RESPONSE;
    }

    /* must be in the zippylog namespace */
    if (request_envelope.envelope.message_namespace(0) != 1) {
        ::zippylog::zippylogd::WorkerInvalidMessageEnumeration log = ::zippylog::zippylogd::WorkerInvalidMessageEnumeration();
        LOG_MESSAGE(log, this->logger_sock);

        this->PopulateErrorResponse(
            protocol::response::INVALID_MESSAGE_NAMESPACE,
            "message namespace is not zippylog's",
            output
        );
        result = AUTHORITATIVE;
        goto SEND_RESPONSE;
    }

    uint32 request_type = request_envelope.envelope.message_type(0);
    switch (request_type) {
        case protocol::request::StoreInfo::zippylog_enumeration:
        {
            Envelope response;
            this->ProcessStoreInfo(response);
            output.push_back(response);
            result = AUTHORITATIVE;
            break;
        }

        case protocol::request::Get::zippylog_enumeration:
            result = this->ProcessGet(request_envelope, output);
            break;

        case protocol::request::SubscribeStoreChanges::zippylog_enumeration:
            result = this->ProcessSubscribeStoreChanges(request_envelope, output);
            break;

        case protocol::request::SubscribeEnvelopes::zippylog_enumeration:
            result = this->ProcessSubscribeEnvelopes(request_envelope, output);
            break;

        case protocol::request::SubscribeKeepalive::zippylog_enumeration:
            result = this->ProcessSubscribeKeepalive(request_envelope, output);
            break;

        default:
        {
            WorkerReceiveUnknownRequestType log = WorkerReceiveUnknownRequestType();
            log.set_enumeration(request_type);
            LOG_MESSAGE(log, this->logger_sock);

            this->PopulateErrorResponse(
                protocol::response::UNKNOWN_REQUEST_TYPE,
                "server does not know how to process the request",
                output
            );
            result = AUTHORITATIVE;
            break;
        }
    }

SEND_RESPONSE:
    if (result == AUTHORITATIVE) {
        assert(output.size() > 0);

        // tags associated with request attached to response so client
        // can associate a single response to a request
        if (request_envelope.envelope.tag_size() >= 0) {
            for (size_t i = 0; i < request_envelope.envelope.tag_size(); i++) {
                output[0].envelope.add_tag(request_envelope.envelope.tag(i));
            }
        }
    }

    return result;
}

RequestProcessor::ResponseStatus RequestProcessor::ProcessStoreInfo(Envelope &e)
{
    ::zippylog::zippylogd::WorkerBeginProcessStoreInfo logstart = ::zippylog::zippylogd::WorkerBeginProcessStoreInfo();
    LOG_MESSAGE(logstart, this->logger_sock);

    protocol::StoreInfo info = protocol::StoreInfo();
    this->store.StoreInfo(info);

    ::zippylog::zippylogd::WorkerEndProcessStoreInfo logend = ::zippylog::zippylogd::WorkerEndProcessStoreInfo();
    LOG_MESSAGE(logend, this->logger_sock);

    info.add_to_envelope(&e);
    return AUTHORITATIVE;
}

RequestProcessor::ResponseStatus RequestProcessor::ProcessGet(Envelope &request, vector<Envelope> &output)
{
    protocol::request::Get *get = (protocol::request::Get *)request.GetMessage(0);
    if (!get) {
        ::zippylog::zippylogd::WorkerReceiveInvalidGet log = ::zippylog::zippylogd::WorkerReceiveInvalidGet();
        LOG_MESSAGE(log, this->logger_sock);

        this->PopulateErrorResponse(
            protocol::response::UNKNOWN_REQUEST_TYPE,
            "error parsing get message... weird",
            output
        );
        return AUTHORITATIVE;
    }

    if (!get->has_path()) {
        ::zippylog::zippylogd::WorkerReceiveInvalidGet log = ::zippylog::zippylogd::WorkerReceiveInvalidGet();
        LOG_MESSAGE(log, this->logger_sock);

        this->PopulateErrorResponse(
            protocol::response::EMPTY_FIELD,
            "required field 'path' is empty",
            output
        );
        return AUTHORITATIVE;
    }

    if (!get->has_start_offset()) {
        ::zippylog::zippylogd::WorkerReceiveInvalidGet log = ::zippylog::zippylogd::WorkerReceiveInvalidGet();
        LOG_MESSAGE(log, this->logger_sock);

        this->PopulateErrorResponse(
            protocol::response::EMPTY_FIELD,
            "required field 'start_offset' is not defined",
            output
        );
        return AUTHORITATIVE;
    }

    // TODO perform additional stream verification

    InputStream stream;
    if (!this->store.GetInputStream(get->path(), stream)) {
        ::zippylog::zippylogd::WorkerGetInvalidStream log = ::zippylog::zippylogd::WorkerGetInvalidStream();
        LOG_MESSAGE(log, this->logger_sock);

        this->PopulateErrorResponse(
            protocol::response::PATH_NOT_FOUND,
            "requested stream could not be found",
            output
        );
        return AUTHORITATIVE;
    }

    uint64 start_offset = get->start_offset();
    stream.Seek(start_offset);

    // determine how much to fetch
    uint32 bytes_left = 256000; // TODO pull from server config

    // client can lower server default if it wants
    if (get->has_max_response_bytes() && get->max_response_bytes() < bytes_left) {
        bytes_left = get->max_response_bytes();
    }

    zippylog::Envelope m = zippylog::Envelope();

    uint32 envelope_size = stream.NextEnvelopeSize();
    // could not find envelope in stream at offset
    if (!envelope_size) {
        ::zippylog::zippylogd::WorkerGetInvalidOffset log = ::zippylog::zippylogd::WorkerGetInvalidOffset();
        LOG_MESSAGE(log, this->logger_sock);

        // TODO need better error code
        this->PopulateErrorResponse(
            protocol::response::PATH_NOT_FOUND,
            "no envelopes found at stream offset",
            output
        );
        return AUTHORITATIVE;
    }

    // we must have an envelope, so start the send sequence
    ::zippylog::zippylogd::WorkerBeginProcessGet logstart = ::zippylog::zippylogd::WorkerBeginProcessGet();
    LOG_MESSAGE(logstart, this->logger_sock);

    protocol::response::StreamSegmentStart segment_start = protocol::response::StreamSegmentStart();
    segment_start.set_path(get->path());
    segment_start.set_offset(get->start_offset());
    ::zippylog::Envelope env = ::zippylog::Envelope();
    segment_start.add_to_envelope(&env);

    // copy request tags to response for client association
    if (request.envelope.tag_size() >= 0) {
        for (size_t i = 0; i < request.envelope.tag_size(); i++) {
            env.envelope.add_tag(request.envelope.tag(i));
        }
    }

    output.push_back(env);

    uint32 bytes_read = 0;
    uint32 envelopes_read = 0;

    while (true) {
        if (!stream.ReadEnvelope(env, envelope_size)) break;

        output.push_back(env);

        bytes_read += envelope_size;
        envelopes_read++;

        if (bytes_left - envelope_size < 0) break;
        bytes_left -= envelope_size;

        envelope_size = stream.NextEnvelopeSize();
        if (!envelope_size) break;
    }

    protocol::response::StreamSegmentEnd segment_end = protocol::response::StreamSegmentEnd();
    segment_end.set_envelopes_sent(envelopes_read);
    segment_end.set_bytes_sent(bytes_read);
    segment_end.set_offset(start_offset + bytes_read);

    env = ::zippylog::Envelope();
    segment_end.add_to_envelope(&env);
    output.push_back(env);

    ::zippylog::zippylogd:: WorkerEndProcessGet logend = ::zippylog::zippylogd:: WorkerEndProcessGet();
    LOG_MESSAGE(logend, this->logger_sock);

    return AUTHORITATIVE;
}

RequestProcessor::ResponseStatus RequestProcessor::ProcessSubscribeStoreChanges(Envelope &request, vector<Envelope> &output)
{
    protocol::request::SubscribeStoreChanges *m =
        (protocol::request::SubscribeStoreChanges *)request.GetMessage(0);

    if (!m) {
        // TODO log error and send error response
        return PROCESS_ERROR;
    }

    // TODO validation

    // we pass the identities and the original message to the streamer
    // we don't pass the first identity, b/c it belongs to the local socket
    // TODO should probably create a special message type with identities embedded
    vector<string> subscription_identities = this->current_request_identities;
    subscription_identities.erase(subscription_identities.begin());

    zeromq::send_envelope(this->subscriptions_sock, subscription_identities, request);

    return DEFERRED;
}

RequestProcessor::ResponseStatus RequestProcessor::ProcessSubscribeEnvelopes(Envelope &request, vector<Envelope> &output)
{
    protocol::request::SubscribeEnvelopes *m =
        (protocol::request::SubscribeEnvelopes *)request.GetMessage(0);

    // TODO validation

    // proxy the message w/ identities (minus first one)
    vector<string> subscription_identities = this->current_request_identities;
    subscription_identities.erase(subscription_identities.begin());

    zeromq::send_envelope(this->subscriptions_sock, subscription_identities, request);

    return DEFERRED;
}

RequestProcessor::ResponseStatus RequestProcessor::ProcessSubscribeKeepalive(Envelope &request, vector<Envelope> &output)
{
    protocol::request::SubscribeKeepalive *m =
        (protocol::request::SubscribeKeepalive *)request.GetMessage(0);

    // TODO validation

    WorkerForwardSubscribeKeepalive log = WorkerForwardSubscribeKeepalive();
    log.set_subscription(m->id());
    LOG_MESSAGE(log, this->logger_sock);

    zeromq::send_envelope(this->subscription_updates_sock, request);

    return DEFERRED;
}

bool RequestProcessor::PopulateErrorResponse(protocol::response::ErrorCode code, string message, vector<Envelope> &msgs)
{
    ::zippylog::zippylogd::WorkerSendErrorResponse log = ::zippylog::zippylogd::WorkerSendErrorResponse();
    log.set_message(message);
    LOG_MESSAGE(log, this->logger_sock);

    protocol::response::Error error = protocol::response::Error();
    error.set_code(code);
    error.set_msg(message);
    Envelope response = zippylog::Envelope();
    error.add_to_envelope(&response);
    msgs.push_back(response);

    return true;
}

}} // namespaces
