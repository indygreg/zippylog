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

#ifndef ZIPPYLOG_DEVICE_STORE_WRITER_SENDER_HPP_
#define ZIPPYLOG_DEVICE_STORE_WRITER_SENDER_HPP_

#include <zippylog/zippylog.hpp>
#include <zippylog/envelope.hpp>

#include <zmq.hpp>

#include <string>

namespace zippylog {
namespace device {

/// Class parameters for the store writer sender device
class ZIPPYLOG_EXPORT StoreWriterSenderStartParams {
public:
    StoreWriterSenderStartParams() :
      ctx(NULL)
    { }

    /// 0MQ context to use
    ///
    /// If NULL, a new context will be created
    ::zmq::context_t *ctx;

    /// socket endpoint that store writer's envelope_pull socket is bound to
    ///
    /// Required to send unvalidated, non-guaranteed envelopes
    ::std::string envelope_pull_endpoint;

    /// socket endpoint that store writer's envelope rep socket is bound to
    ///
    /// Required to send unvalidated, guaranteed envelopes
    ::std::string envelope_rep_endpoint;
};

/// A client/sender for the StoreWriter device
///
/// Instances of this class send data to the StoreWriter device for writing
/// In setups utilizing a StoreWriter, there is typically a separate instance
/// of this class per thread or logical processing unit.
class ZIPPYLOG_EXPORT StoreWriterSender {
public:
    /// Construct a new store sender instance
    StoreWriterSender(StoreWriterSenderStartParams &params);
    ~StoreWriterSender();

    /// Delivers an envelope to the store writer
    ///
    /// Function only delivers the envelope to the writer for writing. This
    /// doesn't actually guarantee that the envelope will be written. If an
    /// error occurs in the writer, the caller will never know.
    ///
    /// No part of the envelope is modified as part of the function.
    ///
    /// Returns whether the envelope was successfully delivered.
    bool DeliverEnvelope(::std::string const &bucket, ::std::string const &set, ::zippylog::Envelope &e);

    /// Writes an envelope using the store writer
    ///
    /// Functions sends the envelope to the writer and then waits for it to
    /// reply, saying it has written the envelope.
    ///
    /// Returns true if writer says it has written the envelope.
    /// False otherwise.
    bool WriteEnvelope(::std::string const &bucket, ::std::string const &set, ::zippylog::Envelope &e);

protected:
    /// 0MQ context being used
    ::zmq::context_t *ctx;

    /// Whether we own the 0MQ context
    bool own_context;

    /// 0MQ endpoint to bind a PULL socket to for receiving envelopes
    ::std::string envelope_pull_endpoint;

    /// 0MQ endpoint to bind a REP socket to for receiving envelopes
    ::std::string envelope_rep_endpoint;

    /// PULL socket to receive envelopes that don't need responses
    ::zmq::socket_t * envelope_pull_sock;

    /// REP socket to receive envelopes that do need responses
    ::zmq::socket_t * envelope_rep_sock;

private:
    StoreWriterSender(StoreWriterSender const &orig);
    StoreWriterSender & operator=(StoreWriterSender const &orig);
};

}} // namespaces

#endif