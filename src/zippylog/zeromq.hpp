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

#ifndef ZIPPYLOG_ZEROMQ_HPP_
#define ZIPPYLOG_ZEROMQ_HPP_

#include <zippylog/zippylog.hpp>
#include <zippylog/envelope.hpp>
#include <zmq.hpp>

#include <string>
#include <vector>

namespace zippylog {
namespace zeromq {

// receives a multipart message from a socket, with identities
// this is likely used by XREQ/XREP sockets
    bool receive_multipart_message(::zmq::socket_t * socket, ::std::vector< ::std::string > &identities, ::std::vector< ::zmq::message_t * > &messages);

/// Receives a multipart message into a vector
///
/// It is the caller's responsibility to free message_t objects pointed to by
/// vector contents.
///
/// Returns true if message(s) received successfully. False if there was an
/// error or ZMQ_NOBLOCK was set and there were no messages available.
bool receive_multipart_message(::zmq::socket_t * socket, ::std::vector< ::zmq::message_t * > &messages, int flags=0);

// sends a multipart message with identities
bool send_multipart_message(::zmq::socket_t * socket, ::std::vector< ::std::string > &identities, ::std::vector< ::zmq::message_t * > &messages, int last_flags=0);

bool send_multipart_message(::zmq::socket_t * socket, ::std::vector< ::std::string> &identities, ::zmq::message_t *message, int last_flags=0);

// sends a multipart message with the last message having ZMQ_SNDMORE
bool send_multipart_more(::zmq::socket_t *socket, ::std::vector< ::std::string > &identities, ::zmq::message_t &msg);

/// Sends an envelope with messages coming before
///
/// This should really be called send_envelope() and those should have "with_identities_
/// but that would break backwards compatibility.
/// @todo break backwards compatibility someday
bool send_envelope_with_preceding(::zmq::socket_t *, ::std::vector< ::std::string > &preceding, Envelope &envelope, int flags=0);

bool send_envelope(::zmq::socket_t *socket, Envelope &envelope, int flags=0);
bool send_envelope(::zmq::socket_t *socket, ::std::vector< ::std::string > &identities, Envelope &envelope, int flags=0);

// sends multiple envelopes with identities as part of a multipart message
bool send_envelopes(::zmq::socket_t *socket, ::std::vector< ::std::string > &identities, ::std::vector<Envelope> &envelopes);

// sends an envelope with ZMQ_SNDMORE flag
bool send_envelope_more(::zmq::socket_t *socket, Envelope &envelope);
bool send_envelope_more(::zmq::socket_t *socket, ::std::vector< ::std::string > &identities, Envelope &envelope);

// sends an envelope, but from an XREQ socket
// this inserts an empty message part to cover the missing identity message
bool send_envelope_xreq(::zmq::socket_t *socket, Envelope &envelope);

/// Sends an envelope over a socket
///
/// @param socket Socket to send over
/// @param e Envelope to send
/// @param is_protocol Serialize as a protocol message
/// @param flags 0MQ send flags
/// @return 0 on send failure (try again), -1 on serialization failure, 1 success
int SendEnvelope(::zmq::socket_t &socket, Envelope &e, bool is_protocol, int flags);

/// Sends an envelope over a socket with message identities
///
/// This is a convenience method to sends an envelope over an XREQ/XREP socket
/// with message identities.
///
/// If the identities list is empty, an empty message will be sent before
/// the envelope. The send flags for the identities messages and the empty
/// message always include ZMQ_SNDMORE. The flags for the envelope message
/// are always from the parameter flags. If the ZMQ_NOBLOCK flag is set, it
/// is set on all message parts.
int SendEnvelope(::zmq::socket_t &socket, ::std::vector< ::std::string >  const &identities, Envelope &e, bool is_protocol, int flags);

}} // end namespaces

#endif