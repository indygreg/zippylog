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

#include <zippylog/zeromq.hpp>

using ::std::string;
using ::std::vector;
using ::zmq::socket_t;
using ::zmq::message_t;

namespace zippylog {
namespace zeromq {

int SendEnvelope(socket_t &socket, Envelope &e, bool is_protocol, int flags)
{
    message_t msg;

    if (is_protocol) {
        if (!e.ToProtocolZmqMessage(msg)) return -1;
    }
    else {
        if (!e.ToZmqMessage(msg)) return -1;
    }

    return socket.send(msg, flags) ? 1 : 0;
}

int SendEnvelope(socket_t &socket, vector<string> const &identities, Envelope &e, bool is_protocol, int flags)
{
    message_t msg;
    int initial_flags = ZMQ_SNDLABEL | (ZMQ_DONTWAIT & flags);

    message_t e_msg;
    if (is_protocol) {
        if (!e.ToProtocolZmqMessage(e_msg)) return -1;
    }
    else {
        if (!e.ToZmqMessage(e_msg)) return -1;
    }

    for (size_t i = 0; i < identities.size(); i++) {
        string identity = identities.at(i);
        msg.rebuild(identity.size());
        memcpy(msg.data(), identity.data(), msg.size());
        if (!socket.send(msg, initial_flags)) return 0;
    }

    return socket.send(e_msg, flags) ? 1 : 0;
}

int SendEnvelopes(socket_t &socket, vector<string> const &identities, vector<Envelope> &envelopes, bool is_protocol, int flags)
{
    int initial_flags = ZMQ_SNDLABEL | (ZMQ_DONTWAIT & flags);

    for (vector<string>::size_type i = 0; i < identities.size(); i++) {
        string identity = identities.at(i);
        if (!socket.send(identity.data(), identity.size(), initial_flags)) {
            return 0;
        }
    }

    message_t msg;
    for (vector<Envelope>::size_type i = 0; i < envelopes.size(); i++) {
        if (is_protocol) {
            if (!envelopes.at(i).ToProtocolZmqMessage(msg)) return -1;
        }
        else {
            if (!envelopes.at(i).ToZmqMessage(msg)) return -1;
        }

        if (!socket.send(msg, flags)) return 0;
    }

    return 1;
}

bool ReceiveMessage(::zmq::socket_t &socket, MessageContainer &container, int flags)
{
    container.Clear();

    int32 more, label;
    size_t moresz, labelsz;

    while (true) {
        message_t *msg = new message_t();
        if (!socket.recv(msg, flags)) {
            delete msg;
            return false;
        }

        labelsz = sizeof(label);
        socket.getsockopt(ZMQ_RCVLABEL, &label, &labelsz);

        if (label) {
            container.AddIdentity(msg);
            continue;
        }

        container.AddMessage(msg);
        moresz = sizeof(more);
        socket.getsockopt(ZMQ_RCVMORE, &more, &moresz);

        if (!more) break;
    }

    return true;
}

bool TransferMessage(::zmq::socket_t &receiver, ::zmq::socket_t &sender)
{
    message_t msg;

    int32 more, label;
    size_t moresz, labelsz;

    while (true) {
        if (!receiver.recv(&msg, 0)) {
            return false;
        }

        labelsz = sizeof(label);
        receiver.getsockopt(ZMQ_RCVLABEL, &label, &labelsz);
        if (label) {
            if (!sender.send(msg, ZMQ_SNDLABEL)) {
                return false;
            }

            continue;
        }

        moresz = sizeof(more);
        receiver.getsockopt(ZMQ_RCVMORE, &more, &moresz);

        if (!sender.send(msg, more ? ZMQ_SNDMORE : 0)) {
            return false;
        }

        if (!more) break;
    }

    return true;
}

MessageContainer::MessageContainer() { }

MessageContainer::~MessageContainer()
{
    this->Clear();
}

void MessageContainer::Clear()
{
    this->identities.clear();

    vector<message_t *>::iterator i = this->messages.begin();
    vector<message_t *>::iterator end = this->messages.end();

    for (; i != end; ++i) {
        if (*i) delete *i;
    }

    this->messages.clear();
}

void MessageContainer::AddIdentity(message_t *msg)
{
    this->identities.push_back(string((const char *)msg->data(), msg->size()));
    delete msg;
}

void MessageContainer::AddMessage(message_t *msg)
{
    this->messages.push_back(msg);
}

}} // namespaces
