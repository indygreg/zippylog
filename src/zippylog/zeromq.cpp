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

#include <zippylog/zeromq.hpp>

namespace zippylog {
namespace zeromq {

bool receive_multipart_message(socket_t * socket, vector<string> &identities, vector<message_t *> &messages)
{
    identities.clear();
    messages.clear();

    bool in_identity = true;

    while (true) {
        message_t *msg = new message_t();
        if (!socket->recv(msg, 0)) {
            delete msg;
            return false;
        }

        if (msg->size() == 0) {
            in_identity = false;
            delete msg;
            continue;
        }

        if (in_identity) {
            identities.push_back(string((const char *)msg->data(), msg->size()));
            delete msg;
        }
        else {
            messages.push_back(msg);
        }

        int64 more;
        size_t moresz = sizeof(more);
        socket->getsockopt(ZMQ_RCVMORE, &more, &moresz);
        if (!more) break;
    }

    return true;
}

bool send_multipart_message(socket_t * socket, vector<string> &identities, vector<message_t *> &messages)
{
    vector<string>::iterator identity = identities.begin();
    for (; identity != identities.end(); identity++) {
        message_t msg(identity->length());
        memcpy(msg.data(), identity->c_str(), identity->length());
        if (!socket->send(msg, ZMQ_SNDMORE)) return false;
    }

    message_t empty(0);
    if (!socket->send(empty, ZMQ_SNDMORE)) return false;

    for (size_t i = 0; i < messages.size(); i++) {
        if (!socket->send(*messages[i], i+1 == messages.size() ? 0 : ZMQ_SNDMORE)) return false;
    }

    return true;
}

bool send_multipart_message(socket_t * socket, vector<string> &identities, message_t *message)
{
    vector<message_t *> messages;
    messages.push_back(message);

    return send_multipart_message(socket, identities, messages);
}

bool send_envelope(socket_t *socket, Envelope &envelope)
{
    message_t *msg = envelope.to_zmq_message();

    bool result = socket->send(*msg, 0);
    delete msg;
    return result;
}

bool send_envelope(socket_t *socket, vector<string> &identities, Envelope &envelope)
{
    message_t *msg = envelope.to_zmq_message();
    bool result = send_multipart_message(socket, identities, msg);
    delete msg;

    return result;
}

bool send_envelope_xreq(socket_t *socket, Envelope &envelope)
{
    message_t empty(0);
    if (!socket->send(empty, ZMQ_SNDMORE)) return false;

    return send_envelope(socket, envelope);
}

}} // namespaces