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

#include <zippylog/device/store_writer_sender.hpp>
#include <zippylog/store.hpp>
#include <zippylog/zeromq.hpp>

#include <vector>

namespace zippylog {
namespace device {

using ::std::string;
using ::std::vector;
using ::zmq::message_t;

StoreWriterSender::StoreWriterSender(StoreWriterSenderStartParams &params) :
    ctx(params.ctx),
    own_context(false),
    envelope_pull_endpoint(params.envelope_pull_endpoint),
    envelope_rep_endpoint(params.envelope_rep_endpoint),
    envelope_pull_sock(NULL),
    envelope_rep_sock(NULL)
{
    // TODO perform validation

    if (!this->ctx) {
        this->ctx = new ::zmq::context_t(1);
        this->own_context = true;
    }

    if (this->envelope_pull_endpoint.length()) {
        this->envelope_pull_sock = new ::zmq::socket_t(*this->ctx, ZMQ_PUSH);
        this->envelope_pull_sock->connect(this->envelope_pull_endpoint.c_str());
    }

    if (this->envelope_rep_endpoint.length()) {
        this->envelope_rep_sock = new ::zmq::socket_t(*this->ctx, ZMQ_REQ);
        this->envelope_rep_sock->connect(this->envelope_rep_endpoint.c_str());
    }
}

StoreWriterSender::~StoreWriterSender()
{
    if (this->own_context && this->ctx) {
        delete this->ctx;
    }

    if (this->envelope_pull_sock) delete this->envelope_pull_sock;
    if (this->envelope_rep_sock) delete this->envelope_rep_sock;
}

bool StoreWriterSender::DeliverEnvelope(const string &bucket, const string &set, ::zippylog::Envelope &e)
{
    // TODO is this appropriate? I think it signifies a coding error (no param to constructor) and thus is
    if (!this->envelope_pull_sock)
        throw "can not deliver envelopes since the pull socket is not configured";

    vector<string> preceding;
    string path = ::zippylog::Store::StreamsetPath(bucket, set);
    preceding.push_back(path);

    return ::zippylog::zeromq::send_envelope_with_preceding(this->envelope_pull_sock, preceding, e);
}

bool StoreWriterSender::WriteEnvelope(const string &bucket, const string &set, ::zippylog::Envelope &e)
{
    if (!this->envelope_rep_sock)
        throw "can not deliver envelopes since the rep sock is not configured";

    vector<string> preceding;
    string path = ::zippylog::Store::StreamsetPath(bucket, set);
    preceding.push_back(path);

    if (!::zippylog::zeromq::send_envelope_with_preceding(this->envelope_rep_sock, preceding, e)) {
        // TODO we might want to reconnect the socket in case the FSM is messed up
        return false;
    }

    // now wait for the reply
    vector<message_t *> msgs;
    if (!::zippylog::zeromq::receive_multipart_message(this->envelope_rep_sock, msgs)) {
        // TODO recover socket
        return false;
    }

    bool result = true;

    if (msgs.size() < 1 || msgs[0]->size() != 0 || msgs.size() > 1) {
        result = false;
    }

    for (vector<message_t *>::iterator i = msgs.begin(); i != msgs.end(); i++) {
        delete *i;
    }

    return result;
}

}} // namespaces