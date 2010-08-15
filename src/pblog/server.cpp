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

#include <pblog/server.hpp>
#include <pblog/pblog.pb.h>
#include <pblog/protocol/response.pb.h>

#include <string>
#include <zmq.hpp>

namespace pblog {
namespace server {

using namespace ::zmq;
using namespace ::google::protobuf;

enum worker_state {
    CREATE_SOCKET = 1,
    WAITING = 2,
    RESET_CONNECTION = 3,
    PROCESS_REQUEST = 4,
    PARSE_REQUEST_ERROR = 5,
};

void * __stdcall worker(apr_thread_t *thread, void *data)
{
    apr_pool_t *p;
    int loop = 1;
    int state = CREATE_SOCKET;
    message_t identities[2];
    worker_start_data *d = (worker_start_data *)data;
    int64 more;
    size_t moresz;
    pollitem_t poll[1];

    moresz = sizeof(more);

    apr_pool_create(&p, d->p);
    socket_t *socket = NULL;

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
                ::pblog::Message pb = ::pblog::Message();

                socket->recv(&msg, 0);
                if (!pb.ParseFromArray(msg.data(), msg.size())) {
                    state = PARSE_REQUEST_ERROR;
                    break;
                }

                printf("received pblog message!\n");

                break;
            }

            case PARSE_REQUEST_ERROR:
            {
                protocol::response::Error pb = protocol::response::Error();

                pb.set_code(protocol::response::INVALID_REQUEST_PAYLOAD);
                pb.set_msg("unable to parse request message");

                if (!socket->send(identities[0], ZMQ_SNDMORE)) {
                    state = RESET_CONNECTION;
                    break;
                }

                if (!socket->send(identities[1], ZMQ_SNDMORE)) {
                    state = RESET_CONNECTION;
                    break;
                }

                string serialized;
                assert(pb.SerializeToString(&serialized));

                message_t msg(serialized.length());
                memcpy(msg.data(), (void *)serialized.c_str(), serialized.length());
                socket->send(msg, 0);

                state = WAITING;
            }
        }
    }

    if (socket) { delete socket; }

    apr_pool_destroy(p);

    return NULL;

}

}} // namespaces