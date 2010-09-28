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

#ifndef ZIPPYLOG_ZEROMQ_HPP_
#define ZIPPYLOG_ZEROMQ_HPP_

#include <zippylog/zippylog.h>
#include <zippylog/envelope.hpp>
#include <zmq.hpp>

#include <string>
#include <vector>

using ::std::string;
using ::std::vector;
using ::zippylog::Envelope;
using ::zmq::socket_t;
using ::zmq::message_t;

namespace zippylog {
namespace zeromq {

// receives a multipart message from a socket, with identities
// this is likely used by XREQ/XREP sockets
bool receive_multipart_message(socket_t * socket, vector<string> &identities, vector<message_t *> &messages);

// sends a multipart message with identities
bool send_multipart_message(socket_t * socket, vector<string> &identities, vector<message_t *> &messages);

bool send_multipart_message(socket_t * socket, vector<string> &identities, message_t *message);

bool send_envelope(socket_t *socket, Envelope &envelope);
bool send_envelope(socket_t *socket, vector<string> &identities, Envelope &envelope);

// sends an envelope with ZMQ_SNDMORE flag
bool send_envelope_more(socket_t *socket, Envelope &envelope);

// sends an envelope, but from an XREQ socket
// this inserts an empty message part to cover the missing identity message
bool send_envelope_xreq(socket_t *socket, Envelope &envelope);

}} // end namespaces

#endif