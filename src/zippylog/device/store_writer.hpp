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

#ifndef ZIPPYLOG_DEVICE_STORE_WRITER_HPP_
#define ZIPPYLOG_DEVICE_STORE_WRITER_HPP_

#include <zippylog/zippylog.hpp>
#include <zippylog/store.hpp>

#include <zmq.hpp>

#include <string>

namespace zippylog {
namespace device {

/// Class parameters for the store writer device
class ZIPPYLOG_EXPORT StoreWriterStartParams {
public:
    StoreWriterStartParams() :
      ctx(NULL),
      active(NULL)
    { }

    /// 0MQ context to use
    ///
    /// If NULL, a new context will be created
    ::zmq::context_t *ctx;

    /// path from which to create the store we will be writing to
    ///
    /// will be fed into Store constructor
    ::std::string store_path;

    /// socket endpoint to which to bind a PULL socket that receives envelopes
    ///
    /// Data received should be a multipart 0MQ message. The first part is the
    /// string store path to which to write the data. e.g. "/bucket/set". All
    /// parts that follow are assumed to be valid protocol buffer
    /// encoded zippylog envelope messages. No verification will be
    /// performed. Caller will not receive notification that envelope has
    /// been written.
    ::std::string envelope_pull_endpoint;

    /// socket endpoint to which to bind a REP socket that receives envelopes
    ///
    /// Data format is the same as for the envelope_pull socket. However, the
    /// socket will send a single part 0MQ message reply once the envelope
    /// has been written. Please keep in mind that this socket makes no
    /// guarantee that the data has been persisted to disk.
    ///
    /// Once the envelope has been written, an empty 0MQ message will be sent
    /// back to the client. If there was an error writing the envelope, a
    /// message with non-0 size will be returned. This message will contain a
    /// string describing the error encountered.
    ::std::string envelope_rep_endpoint;

    /// Semaphore indicating whether device should remain active
    bool *active;
};

/// A device that writes data to a zippylog backing store
///
/// This device listens for 0MQ messages on various sockets and writes received
/// messages to a zippylog store. For a description of the various sockets the
/// device can listen on and how those sockets treat received data, see the
/// documentation for StoreWriterStartParams
///
/// This class exists mainly because of the share-nothing approach of zippylog,
/// which applies to the store. Store writing is currently not thread safe, so
/// all writing needs to be performed by a single thread. The store writer
/// device performs this function. Typically, there is one instance of this
/// class per zippylog store per process.
///
/// For sending messages to this device, see
/// ::zippylog::device::StoreWriterSender.
class ZIPPYLOG_EXPORT StoreWriter {
public:
    /// Construct a new store writer
    StoreWriter(StoreWriterStartParams &params);
    ~StoreWriter();

    /// Processes messages pending on incoming queues and return
    ///
    /// The timeout parameter is passed into zmq_poll(). If messages are
    /// available, will process immediately. Else, it will block for up to
    /// the specified number of microseconds before giving up.
    ///
    /// @param timeout Maximum microseconds we should wait for messages
    int Pump(long timeout = 0);

    /// Runs the store writer
    ///
    /// Will block until the boolean pointed to in the argument goes to false
    bool Run();

protected:
    ::zmq::context_t *ctx;

    // whether we own the 0MQ context
    bool own_context;

    bool *active;

    ::std::string store_path;
    ::zippylog::Store *store;

    ::std::string envelope_pull_endpoint;
    ::std::string envelope_rep_endpoint;

    ::zmq::socket_t *envelope_pull_sock;
    ::zmq::socket_t *envelope_rep_sock;

    ::zmq::pollitem_t *pollitem;

    enum ReceiveResult {
        OK = 1,
        NO_MESSAGES = 2,
        RECEIVE_ERROR = 3,
    };

    int envelope_pull_index;
    int envelope_rep_index;
    int active_sockets;

    bool ProcessEnvelopePull();
    bool ProcessEnvelopeRep();

    ReceiveResult ReceiveAndWrite(::zmq::socket_t *sock);

private:
    StoreWriter(StoreWriter const &orig);
    StoreWriter & operator=(StoreWriter const &orig);
};

}} // namespaces

#endif