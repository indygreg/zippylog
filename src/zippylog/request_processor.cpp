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

    zmq::pollitem_t pollitems[1];
    pollitems[0].events = ZMQ_POLLIN;
    pollitems[0].fd = 0;
    pollitems[0].revents = 0;

    /* variables used across states */
    int state = SETUP_INITIAL_SOCKETS;
    vector<string> identities;
    zippylog::Envelope request_envelope;
    zippylog::Envelope response_envelope;
    protocol::response::ErrorCode error_code;
    string error_message;

    /* common variables used frequently enough to warrant declaration */
    int64 more;
    size_t moresz = sizeof(more);

    while (*this->active) {
        switch (state) {
            case SETUP_INITIAL_SOCKETS:
                this->subscriptions_sock = new socket_t(*this->ctx, ZMQ_PUSH);
                this->subscriptions_sock->connect(this->streaming_subscriptions_endpoint.c_str());

                this->subscription_updates_sock = new socket_t(*this->ctx, ZMQ_PUSH);
                this->subscription_updates_sock->connect(this->streaming_updates_endpoint.c_str());

                state = CREATE_SOCKET;
                break;

            case CREATE_SOCKET:
                this->socket = new socket_t(*this->ctx, ZMQ_XREP);
                this->socket->connect(this->broker_endpoint.c_str());
                pollitems[0].socket = *this->socket;
                state = WAITING;
                break;

            case WAITING:
                // wait up to 250ms for a message, then try again
                // we have this poll so the shutdown semaphore can be detected
                if (zmq::poll(&pollitems[0], 1, 250000)) {
                    state = PROCESS_REQUEST;
                }
                break;

            case RESET_CONNECTION:
                delete this->socket;
                this->socket = NULL;

                identities.clear();

                state = CREATE_SOCKET;
                break;

            case PROCESS_REQUEST:
            {
                vector<message_t *> msgs;
                if (!zeromq::receive_multipart_message(this->socket, identities, msgs)) {
                    ::zippylog::zippylogd::WorkerFailReceiveMessage log = ::zippylog::zippylogd::WorkerFailReceiveMessage();
                    LOG_MESSAGE(log, this->logger_sock);

                    vector<message_t *>::iterator iter = msgs.begin();
                    for (; iter != msgs.end(); iter++) {
                        delete *iter;
                    }

                    state = RESET_CONNECTION;
                    break;
                }

                // no payload messages sure is weird
                if (!msgs.size()) {
                    ::zippylog::zippylogd::WorkerReceiveEmptyMessage log = ::zippylog::zippylogd::WorkerReceiveEmptyMessage();
                    LOG_MESSAGE(log, this->logger_sock);

                    vector<message_t *>::iterator iter = msgs.begin();
                    for (; iter != msgs.end(); iter++) {
                       delete *iter;
                    }

                    state = REQUEST_CLEANUP;
                    break;
                }

                request_envelope = Envelope(msgs[0]);
                // TODO handle parse failure

                // at this points, msgs vector isn't used, so free contents
                vector<message_t *>::iterator iter = msgs.begin();
                for (; iter != msgs.end(); iter++) {
                    delete *iter;
                }
                msgs.clear();

                if (request_envelope.envelope.message_size() < 1) {
                    ::zippylog::zippylogd::WorkerRequestEmptyEnvelope log = ::zippylog::zippylogd::WorkerRequestEmptyEnvelope();
                    LOG_MESSAGE(log, this->logger_sock);

                    error_code = protocol::response::EMPTY_ENVELOPE;
                    error_message = "envelope contains no messages";
                    state = SEND_ERROR_RESPONSE;
                    break;
                }

                if (request_envelope.envelope.message_namespace_size() < 1 || request_envelope.envelope.message_type_size() < 1) {
                    ::zippylog::zippylogd::WorkerInvalidMessageEnumeration log = ::zippylog::zippylogd::WorkerInvalidMessageEnumeration();
                    LOG_MESSAGE(log, this->logger_sock);

                    error_code = protocol::response::MISSING_ENUMERATIONS;
                    error_message = "message received without namespace or type enumerations";
                    state = SEND_ERROR_RESPONSE;
                    break;
                }

                /* must be in the zippylog namespace */
                if (request_envelope.envelope.message_namespace(0) != 1) {
                    ::zippylog::zippylogd::WorkerInvalidMessageEnumeration log = ::zippylog::zippylogd::WorkerInvalidMessageEnumeration();
                    LOG_MESSAGE(log, this->logger_sock);

                    error_code = protocol::response::INVALID_MESSAGE_NAMESPACE;
                    error_message = "message namespace is not zippylog's";
                    state = SEND_ERROR_RESPONSE;
                    break;
                }

                uint32 request_type = request_envelope.envelope.message_type(0);
                if (request_type == protocol::request::StoreInfo::zippylog_enumeration) {
                    state = PROCESS_STOREINFO;
                    break;
                }
                else if (request_type == protocol::request::Get::zippylog_enumeration) {
                    state = PROCESS_GET;
                    break;
                }
                else if (request_type == protocol::request::SubscribeStoreChanges::zippylog_enumeration) {
                    state = PROCESS_SUBSCRIBE_STORE_CHANGES;
                    break;
                }
                else if (request_type == protocol::request::SubscribeEnvelopes::zippylog_enumeration) {
                    state = PROCESS_SUBSCRIBE_ENVELOPES;
                    break;
                }
                else if (request_type == protocol::request::SubscribeKeepalive::zippylog_enumeration) {
                    state = PROCESS_SUBSCRIBE_KEEPALIVE;
                    break;
                }
                else {
                    WorkerReceiveUnknownRequestType log = WorkerReceiveUnknownRequestType();
                    log.set_enumeration(request_type);
                    LOG_MESSAGE(log, this->logger_sock);

                    error_code = protocol::response::UNKNOWN_REQUEST_TYPE;
                    error_message = "server does not know how to process the request";
                    state = SEND_ERROR_RESPONSE;
                    break;
                }

                break;
            }

            case PROCESS_STOREINFO:
            {
                ::zippylog::zippylogd::WorkerBeginProcessStoreInfo logstart = ::zippylog::zippylogd::WorkerBeginProcessStoreInfo();
                LOG_MESSAGE(logstart, this->logger_sock);

                protocol::StoreInfo info = protocol::StoreInfo();
                this->store.StoreInfo(info);

                ::zippylog::zippylogd::WorkerEndProcessStoreInfo logend = ::zippylog::zippylogd::WorkerEndProcessStoreInfo();
                LOG_MESSAGE(logend, this->logger_sock);

                response_envelope = zippylog::Envelope();
                info.add_to_envelope(&response_envelope);
                state = SEND_ENVELOPE_AND_DONE;
                break;
            }

            case PROCESS_GET:
            {
                Message *msg = request_envelope.GetMessage(0);
                if (!msg) {
                    ::zippylog::zippylogd::WorkerReceiveInvalidGet log = ::zippylog::zippylogd::WorkerReceiveInvalidGet();
                    LOG_MESSAGE(log, this->logger_sock);

                    error_code = protocol::response::UNKNOWN_REQUEST_TYPE;
                    error_message = "error parsing get message... weird";
                    state = SEND_ERROR_RESPONSE;
                    break;
                }
                protocol::request::Get *get = (protocol::request::Get *)msg;

                if (!get->has_path()) {
                    ::zippylog::zippylogd::WorkerReceiveInvalidGet log = ::zippylog::zippylogd::WorkerReceiveInvalidGet();
                    LOG_MESSAGE(log, this->logger_sock);

                    error_code = protocol::response::EMPTY_FIELD;
                    error_message = "required field 'path' is empty";
                    state = SEND_ERROR_RESPONSE;
                    break;
                }

                if (!get->has_start_offset()) {
                    ::zippylog::zippylogd::WorkerReceiveInvalidGet log = ::zippylog::zippylogd::WorkerReceiveInvalidGet();
                    LOG_MESSAGE(log, this->logger_sock);

                    error_code = protocol::response::EMPTY_FIELD;
                    error_message = "required field 'start_offset' is not defined";
                    state = SEND_ERROR_RESPONSE;
                    break;
                }

                // TODO perform additional stream verification

                InputStream stream;
                if (!this->store.GetInputStream(get->path(), stream)) {
                    ::zippylog::zippylogd::WorkerGetInvalidStream log = ::zippylog::zippylogd::WorkerGetInvalidStream();
                    LOG_MESSAGE(log, this->logger_sock);

                    error_code = protocol::response::PATH_NOT_FOUND;
                    error_message = "requested stream could not be found";
                    state = SEND_ERROR_RESPONSE;
                    break;
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
                    error_code = protocol::response::PATH_NOT_FOUND;
                    error_message = "no envelopes found at stream offset";
                    state = SEND_ERROR_RESPONSE;
                    break;
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
                if (request_envelope.envelope.tag_size() >= 0) {
                    for (size_t i = 0; i < request_envelope.envelope.tag_size(); i++) {
                        env.envelope.add_tag(request_envelope.envelope.tag(i));
                    }
                }

                zeromq::send_envelope(this->socket, identities, env, ZMQ_SNDMORE);

                uint32 bytes_read = 0;
                uint32 envelopes_read = 0;

                while (true) {
                    if (!stream.ReadEnvelope(env, envelope_size)) break;

                    zeromq::send_envelope_more(this->socket, env);

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
                zeromq::send_envelope(this->socket, env);

                ::zippylog::zippylogd:: WorkerEndProcessGet logend = ::zippylog::zippylogd:: WorkerEndProcessGet();
                LOG_MESSAGE(logend, this->logger_sock);

                state = REQUEST_CLEANUP;
                break;
            }

            case PROCESS_SUBSCRIBE_STORE_CHANGES:
            {
                protocol::request::SubscribeStoreChanges *m =
                    (protocol::request::SubscribeStoreChanges *)request_envelope.GetMessage(0);

                // TODO validation

                // we pass the identities and the original message to the streamer
                // we don't pass the first identity, b/c it belongs to the local socket
                // TODO should probably create a special message type with identities embedded
                vector<string> subscription_identities = identities;
                subscription_identities.erase(subscription_identities.begin());

                zeromq::send_envelope(this->subscriptions_sock, subscription_identities, request_envelope);

                state = REQUEST_CLEANUP;
                break;
            }

            case PROCESS_SUBSCRIBE_ENVELOPES:
            {
                protocol::request::SubscribeEnvelopes *m =
                    (protocol::request::SubscribeEnvelopes *)request_envelope.GetMessage(0);

                // TODO validation

                // proxy the message w/ identities (minus first one)
                vector<string> subscription_identities = identities;
                subscription_identities.erase(subscription_identities.begin());

                zeromq::send_envelope(this->subscriptions_sock, subscription_identities, request_envelope);

                state = REQUEST_CLEANUP;
                break;
            }

            case PROCESS_SUBSCRIBE_KEEPALIVE:
            {
                protocol::request::SubscribeKeepalive *m =
                    (protocol::request::SubscribeKeepalive *)request_envelope.GetMessage(0);

                // TODO validation

                WorkerForwardSubscribeKeepalive log = WorkerForwardSubscribeKeepalive();
                log.set_subscription(m->id());
                LOG_MESSAGE(log, this->logger_sock);

                zeromq::send_envelope(this->subscription_updates_sock, request_envelope);

                state = REQUEST_CLEANUP;
                break;
            }

            case SEND_ERROR_RESPONSE:
            {
                ::zippylog::zippylogd::WorkerSendErrorResponse log = ::zippylog::zippylogd::WorkerSendErrorResponse();
                log.set_message(error_message);
                LOG_MESSAGE(log, this->logger_sock);

                protocol::response::Error error = protocol::response::Error();
                error.set_code(error_code);
                error.set_msg(error_message);
                response_envelope = zippylog::Envelope();
                error.add_to_envelope(&response_envelope);
                state = SEND_ENVELOPE_AND_DONE;
                break;
            }

            case SEND_ENVELOPE_AND_DONE:
            {
                // tags associated with request attached to response so client
                // can associate a single response to a request
                if (request_envelope.envelope.tag_size() >= 0) {
                    for (size_t i = 0; i < request_envelope.envelope.tag_size(); i++) {
                        response_envelope.envelope.add_tag(request_envelope.envelope.tag(i));
                    }
                }

                if (!zeromq::send_envelope(this->socket, identities, response_envelope)) {
                    state = RESET_CONNECTION;
                    break;
                }

                state = REQUEST_CLEANUP;
                break;
            }

            case REQUEST_CLEANUP:
            {
                identities.clear();
                state = WAITING;
                break;
            }
        }
    }

    ::zippylog::zippylogd::WorkerShutdown log = ::zippylog::zippylogd::WorkerShutdown();
    LOG_MESSAGE(log, this->logger_sock);

    return;
}

}} // namespaces