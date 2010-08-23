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
#include <pblog/pblogd.pb.h>
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
    int state = Request::CREATE_SOCKET;
    request_processor_start_data *d = (request_processor_start_data *)data;

    assert(d->ctx);
    assert(d->broker_endpoint);
    assert(d->download_endpoint);
    assert(d->store);

    socket_t *socket = NULL;
    socket_t *dsock = NULL;

    dsock = new socket_t(*d->ctx, ZMQ_XREQ);
    dsock->connect(d->download_endpoint);

    /* variables used across states */
    pblog::Envelope request_envelope;
    pblog::Envelope response_envelope;
    protocol::response::ErrorCode error_code;
    string error_message;
    message_t identities[2];

    /* common variables used frequently enough to warrant declaration */
    int64 more;
    size_t moresz = sizeof(more);
    pollitem_t poll[1];

    while (loop) {
        switch (state) {
            case Request::CREATE_SOCKET:
                socket = new socket_t(*d->ctx, ZMQ_XREP);
                socket->connect(d->broker_endpoint);
                state = Request::WAITING;
                break;

            case WAITING:
                /* we are using XREQ/XRES sockets, which means each message
                   will begin with an identity message */
                socket->recv(&identities[0], 0); /* blocks */
                socket->getsockopt(ZMQ_RCVMORE, &more, &moresz);

                /* no initial identity message !? */
                if (!more) {
                    state = Request::RESET_CONNECTION;
                    break;
                }

                /* 2nd identity message */
                socket->recv(&identities[1], 0);
                socket->getsockopt(ZMQ_RCVMORE, &more, &moresz);


                if (!more) {
                    state = Request::RESET_CONNECTION;
                    break;
                }

                /* now we have the client's actual message on the queue */
                /* the message should always be there since routing exists */
                poll[0] = pollitem_t();
                poll[0].socket = *socket;
                poll[0].events = ZMQ_POLLIN;
                if (zmq::poll(poll, 1, 10000) != 1) {
                    state = Request::RESET_CONNECTION;
                    break;
                }

                state = Request::PROCESS_REQUEST;
                break;

            case Request::RESET_CONNECTION:
                identities[0].~message_t();
                identities[1].~message_t();
                delete socket;
                socket = NULL;
                state = Request::CREATE_SOCKET;
                break;

            case Request::PROCESS_REQUEST:
            {
                /* we've already verified message is available, so we grab it */
                message_t msg;

                socket->recv(&msg, 0);
                request_envelope = ::pblog::Envelope(&msg);
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
                else if (request_type == protocol::request::Stream::pblog_enumeration) {
                    state = Request::PROCESS_STREAM;
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
                d->store->store_info(info);

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

                if (get->stream_size() < 1) {
                    error_code = protocol::response::EMPTY_FIELD;
                    error_message = "stream repeated field is empty";
                    state = Request::SEND_ERROR_RESPONSE;
                    delete get;
                    break;
                }

                for (int i = 0; i < get->stream_size(); i++) {
                    protocol::GetStreamDescription d = get->stream(i);

                    if (d.start_byte_offset() >= d.end_byte_offset()) {
                        error_code = protocol::response::INVALID_OFFSET;
                        error_message = "start_byte_offset must be less than end_byte_offset";
                        state = Request::SEND_ERROR_RESPONSE;
                        delete get;
                        break;
                    }
                }

                // TODO ensure option of path | {bucket, stream_set, stream} both populated

                for (int i = 0; i < get->stream_size(); i++) {
                    protocol::GetStreamDescription desc = get->stream(i);

                    InputStream stream = InputStream();
                    // TODO don't need to open stream, just check for existence
                    if (!d->store->get_input_stream(desc.bucket(), desc.stream_set(), desc.stream(), stream)) {
                        // TODO assumption not always valid for simple bool return code
                        error_code = protocol::response::PATH_NOT_FOUND;
                        error_message = "requested stream not found";
                        state = Request::SEND_ERROR_RESPONSE;
                        delete get;
                        break;
                    }
                }

                // TODO perform additional stream verification

                // we pass the request to the download worker, which does the heavy lifting
                pblogd::GetRequest get_msg = pblogd::GetRequest();

                for (int i = 0; i < get->stream_size(); i++) {
                    protocol::GetStreamDescription * desc = get_msg.add_stream();
                    desc->CopyFrom(get->stream(i));
                }

                get_msg.add_socket_identity(identities[1].data(), identities[1].size());
                response_envelope = pblog::Envelope();
                get_msg.add_to_envelope(&response_envelope);
                message_t * download_msg = response_envelope.to_zmq_message();
                dsock->send(*download_msg, 0);
                delete download_msg;
                delete get;

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

                if (!socket->send(identities[0], ZMQ_SNDMORE)) {
                    state = Request::RESET_CONNECTION;
                    break;
                }


                if (!socket->send(identities[1], ZMQ_SNDMORE)) {
                    state = Request::RESET_CONNECTION;
                    break;
                }

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

void * __stdcall Download::download_worker(apr_thread_t *thread, void *init_data)
{
    download_worker_init *init = (download_worker_init *)init_data;

    assert(init->ctx);
    assert(init->store);
    assert(init->broker_endpoint);
    assert(init->jobs_endpoint);

    pblog::Store *store = init->store;

    socket_t *jsock = new socket_t(*init->ctx, ZMQ_XREP);
    socket_t *rsock = new socket_t(*init->ctx, ZMQ_XREQ);

    jsock->connect(init->jobs_endpoint);
    rsock->connect(init->broker_endpoint);

    // variables used by multiple states
    int state = Download::WAITING;
    vector<message_t *> identities;
    vector<message_t *> received_msgs;
    int stream_offset;
    pblogd::GetRequest request;

    bool loop = true;
    while (loop) {
        switch (state) {
            case Download::WAITING:
                receive_multipart_blocking(jsock, identities, received_msgs);
                state = Download::PARSE_RECEIVED;
                break;

            case Download::RESET:
                for (size_t i = identities.size(); i; ) {
                    delete identities[--i];
                }
                for (size_t i = received_msgs.size(); i; ) {
                    delete received_msgs[--i];
                }
                stream_offset = -1;
                received_msgs.clear();
                state = Download::WAITING;
                break;

            case Download::PARSE_RECEIVED:
            {
                // TODO handle identities properly

                ::pblog::Envelope envelope = ::pblog::Envelope(received_msgs[received_msgs.size()-1]);
                assert(envelope.number_messages() == 1);
                assert(envelope.message_namespace(0) == pblogd::GetRequest::pblog_namespace);
                assert(envelope.message_type(0) == pblogd::GetRequest::pblog_enumeration);

                request = pblogd::GetRequest(*(pblogd::GetRequest *)envelope.get_message(0));

                stream_offset = 0;
                state = Download::PROCESS_GET;
                break;
            }

            case Download::PROCESS_GET:
            {
                // state transitions to itself until we have exhausted all streams
                if (stream_offset >= request.stream_size()) {
                    state = Download::RESET;
                    break;
                }

                protocol::GetStreamDescription desc = request.stream(stream_offset++);

                InputStream stream;
                if (!store->get_input_stream(desc.bucket(), desc.stream_set(), desc.stream(), stream)) {
                    // TODO handle error
                    break;
                }

                stream.Seek(desc.start_byte_offset());

                pblog::Envelope m = pblog::Envelope();
                for (;;) {
                    if (!stream.ReadEnvelope(m)) break;

                    message_t ident(request.socket_identity(0).length());
                    memcpy(ident.data(), request.socket_identity(0).c_str(), request.socket_identity(0).length());

                    message_t *msg = m.to_zmq_message();

                    // need to copy identity messages b/c that's how 0MQ works
                    rsock->send(ident, ZMQ_SNDMORE);
                    printf("sending envelope...\n");
                    if (!rsock->send(*msg, 0)) {
                        printf("error sending envelope!\n");
                    }

                    delete msg;
                }

                break;
            }
        }
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