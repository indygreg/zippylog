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

enum worker_state {
    CREATE_SOCKET = 1,
    WAITING = 2,
    RESET_CONNECTION = 3,
    PROCESS_REQUEST = 4,
    SEND_ENVELOPE_AND_DONE = 5,
    SEND_ERROR_RESPONSE = 6,
    PROCESS_STOREINFO = 7,
    PROCESS_GET = 8,
    PROCESS_STREAM = 9,
};

void * __stdcall worker(apr_thread_t *thread, void *data)
{
    apr_pool_t *p;
    int loop = 1;
    int state = CREATE_SOCKET;
    worker_start_data *d = (worker_start_data *)data;

    apr_pool_create(&p, d->p);
    socket_t *socket = NULL;

    /* variables used across states */
    Envelope response_envelope;
    protocol::response::ErrorCode error_code;
    string error_message;
    message_t identities[2];

    /* common variables used frequently enough to warrant declaration */
    int64 more;
    size_t moresz = sizeof(more);
    pollitem_t poll[1];

    while (loop) {
        switch (state) {
            case CREATE_SOCKET:
                socket = new socket_t(*d->ctx, ZMQ_XREP);
                socket->connect("inproc://tcp_requests");
                state = WAITING;
                break;

            case WAITING:
                /* we are using XREQ/XRES sockets, which means each message
                   will begin with an identity message */
                socket->recv(&identities[0], 0); /* blocks */
                socket->getsockopt(ZMQ_RCVMORE, &more, &moresz);

                /* no initial identity message !? */
                if (!more) {
                    state = RESET_CONNECTION;
                    break;
                }

                /* 2nd identity message */
                socket->recv(&identities[1], 0);
                socket->getsockopt(ZMQ_RCVMORE, &more, &moresz);

                if (!more) {
                    state = RESET_CONNECTION;
                    break;
                }

                /* now we have the client's actual message on the queue */
                /* the message should always be there since routing exists */
                poll[0] = pollitem_t();
                poll[0].socket = *socket;
                poll[0].events = ZMQ_POLLIN;
                if (zmq::poll(poll, 1, 10000) != 1) {
                    state = RESET_CONNECTION;
                    break;
                }

                state = PROCESS_REQUEST;
                break;

            case RESET_CONNECTION:
                identities[0].~message_t();
                identities[1].~message_t();
                delete socket;
                socket = NULL;
                state = CREATE_SOCKET;
                break;

            case PROCESS_REQUEST:
            {
                /* we've already verified message is available, so we grab it */
                message_t msg;
                ::pblog::message::Envelope envelope = ::pblog::message::Envelope();

                socket->recv(&msg, 0);
                if (!envelope.ParseFromArray(msg.data(), msg.size())) {
                    error_code = protocol::response::ENVELOPE_PARSE_FAILURE;
                    error_message = "could not parse received envelope";
                    state = SEND_ERROR_RESPONSE;
                    break;
                }

                if (envelope.message_size() < 1) {
                    error_code = protocol::response::EMPTY_ENVELOPE;
                    error_message = "envelope contains no messages";
                    state = SEND_ERROR_RESPONSE;
                    break;
                }

                if (envelope.message_namespace_size() < 1 || envelope.message_type_size() < 1) {
                    error_code = protocol::response::MISSING_ENUMERATIONS;
                    error_message = "message received without namespace or type enumerations";
                    state = SEND_ERROR_RESPONSE;
                    break;
                }

                /* must be in the pblog namespace */
                if (envelope.message_namespace(0) != 1) {
                    error_code = protocol::response::INVALID_MESSAGE_NAMESPACE;
                    error_message = "message namespace is not pblog's";
                    state = SEND_ERROR_RESPONSE;
                    break;
                }

                uint32 request_type = envelope.message_type(0);
                if (request_type == protocol::request::StoreInfo::pblog_enumeration) {
                    state = PROCESS_STOREINFO;
                    break;
                }
                else if (request_type == protocol::request::Get::pblog_enumeration) {
                    state = PROCESS_GET;
                    break;
                }
                else if (request_type == protocol::request::Stream::pblog_enumeration) {
                    state = PROCESS_STREAM;
                    break;
                }
                else {
                    error_code = protocol::response::UNKNOWN_REQUEST_TYPE;
                    error_message = "server does not know how to process the request";
                    state = SEND_ERROR_RESPONSE;
                    break;
                }

                break;
            }

            case PROCESS_STOREINFO:
            {
                protocol::StoreInfo info = d->store->store_info();

                response_envelope = Envelope();
                info.add_to_envelope(&response_envelope);
                state = SEND_ENVELOPE_AND_DONE;
                break;
            }

            case PROCESS_STREAM:
                error_code = protocol::response::REQUEST_NOT_IMPLEMENTED;
                error_message = "streaming requests are currently not implemented";
                state = SEND_ERROR_RESPONSE;
                break;

            case PROCESS_GET:
                error_code = protocol::response::REQUEST_NOT_IMPLEMENTED;
                error_message = "stream download is not yet implemented";
                state = SEND_ERROR_RESPONSE;
                break;

            case SEND_ERROR_RESPONSE:
            {
                protocol::response::Error error = protocol::response::Error();
                error.set_code(error_code);
                error.set_msg(error_message);
                response_envelope = Envelope();
                error.add_to_envelope(&response_envelope);
                state = SEND_ENVELOPE_AND_DONE;
                break;
            }

            case SEND_ENVELOPE_AND_DONE:
            {
                message_t *msg = response_envelope.to_zmq_message();

                if (!socket->send(identities[0], ZMQ_SNDMORE)) {
                    state = RESET_CONNECTION;
                    break;
                }

                if (!socket->send(identities[1], ZMQ_SNDMORE)) {
                    state = RESET_CONNECTION;
                    break;
                }

                if (!socket->send(*msg, 0)) {
                    state = RESET_CONNECTION;
                    break;
                }

                state = WAITING;
                break;
            }
        }
    }

    if (socket) { delete socket; }

    apr_pool_destroy(p);

    return NULL;

}

}} // namespaces