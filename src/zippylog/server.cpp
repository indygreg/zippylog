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

#include <zippylog/server.hpp>
#include <zippylog/message.pb.h>
#include <zippylog/protocol.pb.h>
#include <zippylog/protocol/request.pb.h>
#include <zippylog/protocol/response.pb.h>

#include <string>
#include <zmq.hpp>

namespace zippylog {
namespace server {

using ::std::string;
using ::zmq::message_t;
using ::zmq::socket_t;

void * __stdcall Request::request_processor(void *data)
{
    request_processor_start_data *d = (request_processor_start_data *)data;

    assert(d->ctx);
    assert(d->broker_endpoint);
    assert(d->streaming_subscriptions_endpoint);
    assert(d->streaming_updates_endpoint);
    assert(d->store);

    socket_t *socket = NULL;
    socket_t *subscriptions_sock = NULL;
    socket_t *subscription_updates_sock = NULL;

    Store *store = d->store;

    zmq::pollitem_t pollitems[1];
    pollitems[0].events = ZMQ_POLLIN;
    pollitems[0].fd = 0;
    pollitems[0].revents = 0;

    /* variables used across states */
    int state = Request::SETUP_INITIAL_SOCKETS;
    message_t zreq;
    vector<message_t *> identities;
    zippylog::Envelope request_envelope;
    zippylog::Envelope response_envelope;
    protocol::response::ErrorCode error_code;
    string error_message;

    /* common variables used frequently enough to warrant declaration */
    int64 more;
    size_t moresz = sizeof(more);

    while (true) {
        switch (state) {
            case Request::SETUP_INITIAL_SOCKETS:
                subscriptions_sock = new socket_t(*d->ctx, ZMQ_PUSH);
                subscriptions_sock->connect(d->streaming_subscriptions_endpoint);

                subscription_updates_sock = new socket_t(*d->ctx, ZMQ_PUSH);
                subscription_updates_sock->connect(d->streaming_updates_endpoint);

                state = Request::CREATE_SOCKET;
                break;

            case Request::CREATE_SOCKET:
                socket = new socket_t(*d->ctx, ZMQ_XREP);
                socket->connect(d->broker_endpoint);
                pollitems[0].socket = *socket;
                state = Request::WAITING;
                break;

            case WAITING:
                // wait up to 250ms for a message, then try again
                // we have this poll so the shutdown semaphore can be detected
                if (zmq::poll(&pollitems[0], 1, 250000)) {
                    state = Request::PROCESS_REQUEST;
                }
                break;

            case Request::RESET_CONNECTION:
                delete socket;
                socket = NULL;

                for (size_t i = 0; i < identities.size(); i++) {
                    delete identities[i];
                }
                identities.clear();

                state = Request::CREATE_SOCKET;
                break;

            case Request::PROCESS_REQUEST:
            {
                while (true) {
                    socket->recv(&zreq, 0);

                    if (zreq.size() == 0) break;

                    message_t *identity = new message_t();
                    identity->copy(&zreq);
                    identities.push_back(identity);

                    int64 more;
                    size_t moresz = sizeof(more);

                    socket->getsockopt(ZMQ_RCVMORE, &more, &moresz);

                    if (!more) break;
                }

                socket->recv(&zreq, 0);

                request_envelope = ::zippylog::Envelope(&zreq);
                //{
                //    error_code = protocol::response::ENVELOPE_PARSE_FAILURE;
                //    error_message = "could not parse received envelope";
                //    state = SEND_ERROR_RESPONSE;
                //    break;
                //}

                if (request_envelope.envelope.message_size() < 1) {
                    error_code = protocol::response::EMPTY_ENVELOPE;
                    error_message = "envelope contains no messages";
                    state = Request::SEND_ERROR_RESPONSE;
                    break;
                }

                if (request_envelope.envelope.message_namespace_size() < 1 || request_envelope.envelope.message_type_size() < 1) {
                    error_code = protocol::response::MISSING_ENUMERATIONS;
                    error_message = "message received without namespace or type enumerations";
                    state = Request::SEND_ERROR_RESPONSE;
                    break;
                }

                /* must be in the zippylog namespace */
                if (request_envelope.envelope.message_namespace(0) != 1) {
                    error_code = protocol::response::INVALID_MESSAGE_NAMESPACE;
                    error_message = "message namespace is not zippylog's";
                    state = Request::SEND_ERROR_RESPONSE;
                    break;
                }

                uint32 request_type = request_envelope.envelope.message_type(0);
                if (request_type == protocol::request::StoreInfo::zippylog_enumeration) {
                    state = Request::PROCESS_STOREINFO;
                    break;
                }
                else if (request_type == protocol::request::Get::zippylog_enumeration) {
                    state = Request::PROCESS_GET;
                    break;
                }
                else if (request_type == protocol::request::SubscribeStoreChanges::zippylog_enumeration) {
                    state = Request::PROCESS_SUBSCRIBE_STORE_CHANGES;
                    break;
                }
                else {
                    error_code = protocol::response::UNKNOWN_REQUEST_TYPE;
                    error_message = "server does not know how to process the request";
                    state = Request::SEND_ERROR_RESPONSE;
                    break;
                }

                break;
            }

            case Request::PROCESS_STOREINFO:
            {
                protocol::StoreInfo info = protocol::StoreInfo();
                d->store->StoreInfo(info);

                response_envelope = zippylog::Envelope();
                info.add_to_envelope(&response_envelope);
                state = SEND_ENVELOPE_AND_DONE;
                break;
            }

            case Request::PROCESS_GET:
            {
                Message *msg = request_envelope.get_message(0);
                if (!msg) {
                    error_code = protocol::response::UNKNOWN_REQUEST_TYPE;
                    error_message = "error parsing get message... weird";
                    state = Request::SEND_ERROR_RESPONSE;
                    break;
                }
                protocol::request::Get *get = (protocol::request::Get *)msg;

                if (!get->has_path()) {
                    error_code = protocol::response::EMPTY_FIELD;
                    error_message = "required field 'path' is empty";
                    state = Request::SEND_ERROR_RESPONSE;
                    delete get;
                    break;
                }

                if (!get->has_start_offset()) {
                    error_code = protocol::response::EMPTY_FIELD;
                    error_message = "required field 'start_offset' is not defined";
                    state = Request::SEND_ERROR_RESPONSE;
                    delete get;
                    break;
                }

                // TODO perform additional stream verification

                InputStream stream;
                if (!store->GetInputStream(get->path(), stream)) {
                    error_code = protocol::response::PATH_NOT_FOUND;
                    error_message = "requested stream could not be found";
                    state = Request::SEND_ERROR_RESPONSE;
                    delete get;
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
                    // TODO need better error code
                    error_code = protocol::response::PATH_NOT_FOUND;
                    error_message = "no envelopes found at stream offset";
                    state = Request::SEND_ERROR_RESPONSE;
                    delete get;
                    break;
                }

                // we must have an envelope, so start the send sequence

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

                vector<message_t *>::iterator i = identities.begin();
                for (; i < identities.end(); i++) {
                    socket->send(**i, ZMQ_SNDMORE);
                }
                message_t empty(0);
                socket->send(empty, ZMQ_SNDMORE);

                message_t *zmsg = env.to_zmq_message();
                socket->send(*zmsg, ZMQ_SNDMORE);
                delete msg;

                uint32 bytes_read = 0;
                uint32 envelopes_read = 0;

                while (true) {
                    if (!stream.ReadEnvelope(env, envelope_size)) break;

                    zmsg = env.to_zmq_message();
                    socket->send(*zmsg, ZMQ_SNDMORE);
                    delete zmsg;

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
                zmsg = env.to_zmq_message();
                socket->send(*zmsg, 0);
                delete zmsg;

                state = Request::REQUEST_CLEANUP;
                break;
            }

            case Request::PROCESS_SUBSCRIBE_STORE_CHANGES:
            {
                protocol::request::SubscribeStoreChanges *m =
                    (protocol::request::SubscribeStoreChanges *)request_envelope.get_message(0);

                // TODO validation

                // we pass the identities and the original message to the streamer
                // we don't pass the first identity, b/c it belongs to the local socket
                for (size_t i = 1; i < identities.size(); i++) {
                    subscriptions_sock->send(*identities[i], ZMQ_SNDMORE);
                }

                message_t *msg = new message_t(0);
                subscriptions_sock->send(*msg, ZMQ_SNDMORE);
                delete msg;

                msg = request_envelope.to_zmq_message();
                subscriptions_sock->send(*msg, 0);

                state = Request::REQUEST_CLEANUP;
                break;
            }

            case Request::SEND_ERROR_RESPONSE:
            {
                protocol::response::Error error = protocol::response::Error();
                error.set_code(error_code);
                error.set_msg(error_message);
                response_envelope = zippylog::Envelope();
                error.add_to_envelope(&response_envelope);
                state = Request::SEND_ENVELOPE_AND_DONE;
                break;
            }

            case Request::SEND_ENVELOPE_AND_DONE:
            {
                // tags associated with request attached to response so client
                // can associate a single response to a request
                if (request_envelope.envelope.tag_size() >= 0) {
                    for (size_t i = 0; i < request_envelope.envelope.tag_size(); i++) {
                        response_envelope.envelope.add_tag(request_envelope.envelope.tag(i));
                    }
                }

                for (int i = 0; i < identities.size(); i++) {
                    if (!socket->send(*identities[i], ZMQ_SNDMORE)) {
                        state = Request::RESET_CONNECTION;
                        break;
                    }
                }

                message_t *msg = new message_t(0);
                if (!socket->send(*msg, ZMQ_SNDMORE)) {
                    delete msg;
                    state = Request::RESET_CONNECTION;
                    break;
                }
                delete msg;

                msg = response_envelope.to_zmq_message();

                if (!socket->send(*msg, 0)) {
                    state = Request::RESET_CONNECTION;
                    break;
                }

                state = Request::REQUEST_CLEANUP;
                break;
            }

            case Request::REQUEST_CLEANUP:
            {
                for (size_t i = 0; i < identities.size(); i++) {
                    delete identities[i];
                }
                identities.clear();
                state = Request::WAITING;
                break;
            }
        }

        if (!d->active) break;
    }

    if (socket) { delete socket; }
    if (subscriptions_sock) delete subscriptions_sock;
    if (subscription_updates_sock) delete subscription_updates_sock;

    return NULL;
}

/*

The stream processor handles streaming of events to all interested clients

*/
void * __stdcall stream_processor(void *start_data)
{
    stream_processor_start_data *start = (stream_processor_start_data *)start_data;

    // first, bind to client queue so we can send messages
    socket_t *sock = new socket_t(*start->ctx, ZMQ_XREP);
    sock->connect(start->socket_endpoint);

    bool loop = true;
    while (loop) {

    }

    return NULL;
}

}} // namespaces