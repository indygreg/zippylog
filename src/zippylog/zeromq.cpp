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

bool receive_multipart_message(socket_t * socket, vector<message_t *> &messages)
{
    messages.clear();

    while (true) {
        message_t *msg = new message_t();
        if (!socket->recv(msg, 0)) {
            delete msg;
            return false;
        }

        messages.push_back(msg);

        int64 more;
        size_t moresz = sizeof(more);
        socket->getsockopt(ZMQ_RCVMORE, &more, &moresz);

        if (!more) break;
    }

    return true;
}

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

bool send_multipart_message(socket_t * socket, vector<string> &identities, vector<message_t *> &messages, int last_flags)
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
        if (!socket->send(*messages[i], i+1 == messages.size() ? last_flags : ZMQ_SNDMORE)) return false;
    }

    return true;
}

bool send_multipart_message(socket_t * socket, vector<string> &identities, message_t *message, int last_flags)
{
    vector<message_t *> messages;
    messages.push_back(message);

    return send_multipart_message(socket, identities, messages, last_flags);
}

bool send_multipart_more(socket_t *socket, vector<string> &identities, message_t &msg)
{
    return send_multipart_message(socket, identities, &msg, ZMQ_SNDMORE);
}

bool send_envelope(socket_t *socket, Envelope &envelope, int flags)
{
    message_t msg;
    envelope.ToZmqMessage(msg);
    return socket->send(msg, flags);
}

bool send_envelope(socket_t *socket, vector<string> &identities, Envelope &envelope, int flags)
{
    message_t msg;
    envelope.ToZmqMessage(msg);
    return send_multipart_message(socket, identities, &msg, flags);
}

bool send_envelope_more(socket_t *socket, Envelope &envelope)
{
    message_t msg;
    envelope.ToZmqMessage(msg);
    return socket->send(msg, ZMQ_SNDMORE);
}

bool send_envelope_more(socket_t *socket, vector<string> &identities, Envelope &e)
{
    message_t msg;
    e.ToZmqMessage(msg);

    return send_multipart_more(socket, identities, msg);
}

bool send_envelope_xreq(socket_t *socket, Envelope &envelope)
{
    message_t empty(0);
    if (!socket->send(empty, ZMQ_SNDMORE)) return false;

    return send_envelope(socket, envelope);
}

}} // namespaces