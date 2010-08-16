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

using namespace pblog::protocol;
using ::pblog::Envelope;
using ::google::protobuf::int64;

Client::Client(context_t *ctx, string connect)
{
    this->_sock = new socket_t(*ctx, ZMQ_XREQ);
    this->_sock->connect(connect.c_str());
}

Client::~Client()
{
    delete this->_sock;
}

StoreInfo * Client::store_info()
{
    pblog::Envelope e = Envelope();
    request::StoreInfo req = request::StoreInfo();
    req.add_to_envelope(&e);

    string buffer;
    e.envelope.SerializeToString(&buffer);
    message_t zreq(buffer.length());
    memcpy(zreq.data(), (void *)buffer.c_str(), buffer.length());

    this->_sock->send(zreq, 0);

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

}} // namespaces
