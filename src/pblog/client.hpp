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

#ifndef PBLOG_CLIENT_HPP_
#define PBLOG_CLIENT_HPP_

#include <pblog/pblog.h>
#include <pblog/protocol.pb.h>

#include <string>
#include <zmq.hpp>

namespace pblog {
namespace client {

using protocol::StoreInfo;
using ::std::string;
using ::zmq::socket_t;
using ::zmq::context_t;

class PBLOG_EXPORT Client {
    public:
        /* establish a client client and bind to the location specified */
        Client(context_t *ctx, string bind);

        ~Client();

        StoreInfo * store_info();

        void get_stream(const string bucket, const string stream_set, const string stream);
        bool read_envelope(pblog::Envelope &envelope);

    protected:
        socket_t *_sock;

        bool _send_envelope(::pblog::Envelope &envelope);
};

}} // namespaces

#endif