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

#include <pblog/server.hpp>
#include <pblog/message.pb.h>
#include <pblog/protocol.pb.h>
#include <pblog/protocol/request.pb.h>
#include <pblog/protocol/response.pb.h>

#include <string>
#include <zmq.hpp>

namespace pblog {
namespace server {

using namespace ::zmq;
using namespace ::google::protobuf;

using ::std::string;

static bool receive_multipart_blocking(socket_t * sock, vector<message_t *> &identities, vector<message_t *> &messages);

void * __stdcall Request::request_processor(apr_thread_t *thread, void *data)
{
    apr_pool_t *p;
    int loop = 1;
    request_processor_start_data *d = (request_processor_start_data *)data;

    assert(d->ctx);
    assert(d->broker_endpoint);
    assert(d->store);

    socket_t *socket = NULL;
    socket_t *dsock = NULL;
    Store *store = d->store;

    /* variables used across states */
    int state = Request::CREATE_SOCKET;
    message_t zreq;
    pblog::Envelope request_envelope;
    pblog::Envelope response_envelope;
    protocol::response::ErrorCode error_code;
    string error_message;

    /* common variables used frequently enough to warrant declaration */
    int64 more;
    size_t moresz = sizeof(more);

    while (loop) {
        switch (state) {
            case Request::CREATE_SOCKET:
                socket = new socket_t(*d->ctx, ZMQ_REP);
                socket->connect(d->broker_endpoint);
                state = Request::WAITING;
                break;

            case WAITING:
                socket->recv(&zreq, 0);
                state = Request::PROCESS_REQUEST;
                break;

            case Request::RESET_CONNECTION:
                delete socket;
                socket = NULL;
                state = Request::CREATE_SOCKET;
                break;

            case Request::PROCESS_REQUEST:
            {
                request_envelope = ::pblog::Envelope(&zreq);
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

                /* must be in the pblog namespace */
                if (request_envelope.envelope.message_namespace(0) != 1) {
                    error_code = protocol::response::INVALID_MESSAGE_NAMESPACE;
                    error_message = "message namespace is not pblog's";
                    state = Request::SEND_ERROR_RESPONSE;
                    break;
                }

                uint32 request_type = request_envelope.envelope.message_type(0);
                if (request_type == protocol::request::StoreInfo::pblog_enumeration) {
                    state = Request::PROCESS_STOREINFO;
                    break;
                }
                else if (request_type == protocol::request::Get::pblog_enumeration) {
                    state = Request::PROCESS_GET;
                    break;
                }
                /*
                else if (request_type == protocol::request::Stream::pblog_enumeration) {
                    state = Request::PROCESS_STREAM;
                    break;
                }
                */
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

                response_envelope = pblog::Envelope();
                info.add_to_envelope(&response_envelope);
                state = SEND_ENVELOPE_AND_DONE;
                break;
            }

            case Request::PROCESS_STREAM:
                error_code = protocol::response::REQUEST_NOT_IMPLEMENTED;
                error_message = "streaming requests are currently not implemented";
                state = Request::SEND_ERROR_RESPONSE;
                break;

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

                stream.Seek(get->start_offset());

                // determine how much to fetch
                uint32 bytes_left = 256000; // TODO pull from server config

                // client can lower server default if it wants
                if (get->has_max_response_bytes() && get->max_response_bytes() < bytes_left) {
                    bytes_left = get->max_response_bytes();
                }

                pblog::Envelope m = pblog::Envelope();

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
                ::pblog::Envelope env = ::pblog::Envelope();
                segment_start.add_to_envelope(&env);
                message_t *zmsg = env.to_zmq_message();
                socket->send(*zmsg, ZMQ_SNDMORE);
                delete msg;

                uint32 bytes_read = 0;
                uint32 events_read = 0;

                while (true) {
                    if (!stream.ReadEnvelope(env, envelope_size)) break;

                    zmsg = env.to_zmq_message();
                    socket->send(*zmsg, ZMQ_SNDMORE);
                    delete zmsg;

                    bytes_read += envelope_size;
                    events_read++;

                    if (bytes_left - envelope_size < 0) break;
                    bytes_left -= envelope_size;

                    envelope_size = stream.NextEnvelopeSize();
                    if (!envelope_size) break;
                }

                protocol::response::StreamSegmentEnd segment_end = protocol::response::StreamSegmentEnd();
                segment_end.set_events_sent(events_read);
                segment_end.set_bytes_sent(bytes_read);
                segment_end.set_offset(get->start_offset() + bytes_read);

                env = ::pblog::Envelope();
                segment_end.add_to_envelope(&env);
                zmsg = env.to_zmq_message();
                socket->send(*zmsg, 0);
                delete zmsg;

                state = Request::WAITING;
                break;
            }

            case Request::SEND_ERROR_RESPONSE:
            {
                protocol::response::Error error = protocol::response::Error();
                error.set_code(error_code);
                error.set_msg(error_message);
                response_envelope = pblog::Envelope();
                error.add_to_envelope(&response_envelope);
                state = Request::SEND_ENVELOPE_AND_DONE;
                break;
            }

            case Request::SEND_ENVELOPE_AND_DONE:
            {
                message_t *msg = response_envelope.to_zmq_message();

                if (!socket->send(*msg, 0)) {
                    state = Request::RESET_CONNECTION;
                    break;
                }

                state = Request::WAITING;
                break;
            }
        }
    }

    if (socket) { delete socket; }
    if (dsock) { delete dsock; }

    return NULL;

}

/*

The stream processor handles streaming of events to all interested clients

*/
void * __stdcall stream_processor(apr_thread_t *thread, void *start_data)
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

// receives a multipart message from a socket
// will block until all parts are received
//
// on success, identities vector contains list of message identities
// messages vector contains list of all content messages
static bool receive_multipart_blocking(socket_t * sock, vector<message_t *> &identities, vector<message_t *> &messages)
{
    int64 more;
    size_t moresz = sizeof(more);

    // TODO implement identity parsing
    while (true) {
        message_t * msg = new message_t();
        sock->recv(msg, 0);
        messages.push_back(msg);

        sock->getsockopt(ZMQ_RCVMORE, &more, &moresz);

        if (!more) break;
    }

    return true;
}

}} // namespaces