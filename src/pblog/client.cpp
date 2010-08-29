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

#include <pblog/client.hpp>

#include <pblog/envelope.hpp>
#include <pblog/protocol/request.pb.h>

namespace pblog {
namespace client {

using ::pblog::Envelope;
using ::google::protobuf::int64;

Client::Client(context_t *ctx, string connect)
{
    this->_sock = new socket_t(*ctx, ZMQ_REQ);
    this->_sock->connect(connect.c_str());
}

Client::~Client()
{
    delete this->_sock;
}

StoreInfo * Client::store_info()
{
    pblog::Envelope e = Envelope();
    protocol::request::StoreInfo req = protocol::request::StoreInfo();
    req.add_to_envelope(&e);

    this->_send_envelope(e);

    int64 more;
    size_t moresz = sizeof(more);
    message_t res;

    while (true) {
        bool result = this->_sock->recv(&res, 0);
        if (!result) {
            printf("error receiving\n");
            break;
        }

        printf("received message with %d bytes\n", res.size());
        this->_sock->getsockopt(ZMQ_RCVMORE, &more, &moresz);

        if (!more) break;
    }

    Envelope e2 = Envelope(&res);
    Message *m = e2.get_message(0);

    return (StoreInfo *)m;
}

void Client::get_stream(const string bucket, const string set, const string stream)
{
    /*
    request::Get get = request::Get();
    GetStreamDescription *desc = get.add_stream();

    desc->set_bucket(bucket);
    desc->set_stream_set(set);
    desc->set_stream(stream);
    desc->set_start_byte_offset(0);
    desc->set_end_byte_offset(10000000);

    ::pblog::Envelope e = ::pblog::Envelope();
    get.add_to_envelope(&e);
    this->_send_envelope(e);
    */
}

bool Client::read_envelope(pblog::Envelope &envelope)
{
    message_t res;

    int64 more;
    size_t moresz = sizeof(more);

    while (true) {
        if (!this->_sock->recv(&res, ZMQ_NOBLOCK)) break;
        printf("received message with %d bytes\n", res.size());
        this->_sock->getsockopt(ZMQ_RCVMORE, &more, &moresz);
        if (!more) break;
    }

    return envelope.merge_from_zmq_message(&res);
}

bool Client::_send_envelope(::pblog::Envelope &envelope)
{
    string buffer;
    envelope.envelope.SerializeToString(&buffer);
    message_t *req = new message_t(buffer.length());
    memcpy(req->data(), (void *)buffer.c_str(), buffer.length());
    return this->_sock->send(*req, 0);
}

}} // namespaces
