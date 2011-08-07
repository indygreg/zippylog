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

#include <zippylog/device/store_writer.hpp>

#include <zippylog/zeromq.hpp>
#include <vector>

namespace zippylog {
namespace device {

using ::std::string;
using ::std::vector;
using ::zippylog::device::PumpResult;
using ::zmq::message_t;

StoreWriter::StoreWriter(StoreWriterStartParams &params) :
    Device(params.active),
    ctx(params.ctx),
    own_context(false),
    store(NULL),
    envelope_pull_endpoint(params.envelope_pull_endpoint),
    envelope_rep_endpoint(params.envelope_rep_endpoint),
    envelope_pull_sock(NULL),
    envelope_rep_sock(NULL),
    pollitem(NULL),
    envelope_pull_index(-1),
    envelope_rep_index(-1),
    active_sockets(0)
{
    if (!this->ctx) {
        this->ctx = new ::zmq::context_t(1);
        this->own_context = true;
    }

    this->store = Store::CreateStore(params.store_path);

    if (this->envelope_pull_endpoint.length()) {
        this->envelope_pull_sock = new ::zmq::socket_t(*this->ctx, ZMQ_PULL);
        this->envelope_pull_sock->bind(this->envelope_pull_endpoint.c_str());
        this->envelope_pull_index = this->active_sockets++;
    }

    if (this->envelope_rep_endpoint.length()) {
        this->envelope_rep_sock = new ::zmq::socket_t(*this->ctx, ZMQ_REP);
        this->envelope_rep_sock->bind(this->envelope_rep_endpoint.c_str());
        this->envelope_rep_index = this->active_sockets++;
    }

    this->pollitem = new ::zmq::pollitem_t[this->active_sockets];

    if (this->envelope_pull_index >= 0) {
        this->pollitem[this->envelope_pull_index].events = ZMQ_POLLIN;
        this->pollitem[this->envelope_pull_index].socket = *this->envelope_pull_sock;
        this->pollitem[this->envelope_pull_index].fd = 0;
        this->pollitem[this->envelope_pull_index].revents = 0;
    }

    if (this->envelope_rep_index >= 0) {
        this->pollitem[this->envelope_rep_index].events = ZMQ_POLLIN;
        this->pollitem[this->envelope_rep_index].socket = *this->envelope_rep_sock;
        this->pollitem[this->envelope_rep_index].fd = 0;
        this->pollitem[this->envelope_rep_index].revents = 0;
    }
}

StoreWriter::~StoreWriter()
{
    if (this->own_context && this->ctx) {
        delete this->ctx;
    }

    if (this->pollitem) delete[] this->pollitem;
    if (this->store) delete this->store;
    if (this->envelope_pull_sock) delete this->envelope_pull_sock;
    if (this->envelope_rep_sock) delete this->envelope_rep_sock;
}

PumpResult StoreWriter::Pump(int32 timeout)
{
    int rc = ::zmq::poll(&this->pollitem[0], this->active_sockets, timeout);
    if (rc < 1) return PumpResult::MakeNoWorkDone();

    if (this->envelope_pull_index >= 0 && this->pollitem[this->envelope_pull_index].revents & ZMQ_POLLIN) {
        if (!this->ProcessEnvelopePull()) return PumpResult::MakeError();
    }

    if (this->envelope_rep_index >= 0 && this->pollitem[this->envelope_rep_index].revents & ZMQ_POLLIN) {
        if (!this->ProcessEnvelopeRep()) return PumpResult::MakeError();
    }

    return PumpResult::MakeWorkDone();
}

bool StoreWriter::ProcessEnvelopePull()
{
    ReceiveResult result;

    do {
        result = this->ReceiveAndWrite(this->envelope_pull_sock);
    } while (result != RECEIVE_ERROR && result != NO_MESSAGES);

    return result == RECEIVE_ERROR ? false : true;
}

bool StoreWriter::ProcessEnvelopeRep()
{
    ReceiveResult result;
    do {
        result = this->ReceiveAndWrite(this->envelope_rep_sock);
        if (result == RECEIVE_ERROR) {
            string error = "unknown error";
            message_t response(error.length());
            memcpy(response.data(), error.c_str(), response.size());

            this->envelope_rep_sock->send(response, 0);
            return false;
        }
        else {
            message_t response(0);
            this->envelope_rep_sock->send(response, 0);
        }
    } while (result != NO_MESSAGES);

    return true;
}

StoreWriter::ReceiveResult StoreWriter::ReceiveAndWrite(::zmq::socket_t *sock)
{
    vector<message_t *> msgs;
    if (!::zippylog::zeromq::receive_multipart_message(sock, msgs, ZMQ_NOBLOCK)) {
        return NO_MESSAGES;
    }

    StoreWriter::ReceiveResult result = OK;
    string path, bucket, set, stream;

    // we expect at least 2 messages. the first is the store path. next are
    // payloads
    if (msgs.size() < 2) {
        result = RECEIVE_ERROR;
        goto cleanup;
    }

    if (msgs[0]->size() < 1) {
        result = RECEIVE_ERROR;
        goto cleanup;
    }

    path = string((const char *)msgs[0]->data());

    if (!::zippylog::Store::ParsePath(path, bucket, set, stream)) {
        result = RECEIVE_ERROR;
        goto cleanup;
    }

    if (!set.size()) {
        result = RECEIVE_ERROR;
        goto cleanup;
    }

    // @todo write to specific stream if instructed?

    for (vector<message_t *>::size_type i = 1; i < msgs.size(); i++) {
        if (!this->store->WriteEnvelope(bucket, set, msgs[i]->data(), msgs[i]->size())) {
            result = RECEIVE_ERROR;
            goto cleanup;
        }
    }

cleanup:
    for (vector<message_t *>::size_type i = 0; i < msgs.size(); i++) {
        delete msgs[i];
    }

    return result;
}

}} // namespaces