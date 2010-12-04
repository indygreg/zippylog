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

#ifndef ZIPPYLOG_REQUEST_PROCESSOR_HPP_
#define ZIPPYLOG_REQUEST_PROCESSOR_HPP_

#include <zippylog/zippylog.hpp>
#include <zippylog/store.hpp>
#include <zippylog/protocol/request.pb.h>
#include <zippylog/protocol/response.pb.h>

#include <zmq.hpp>

namespace zippylog {

/// Used to construct a request processor
class ZIPPYLOG_EXPORT RequestProcessorStartParams {
public:
    RequestProcessorStartParams() :
        ctx(NULL),
        active(NULL),
        get_stream_max_bytes(256000),
        get_stream_max_envelopes(10000)
    { }

    /// The path to the store we should operate against
    ::std::string store_path;

    /// 0MQ context to use
    ///
    /// must not be NULL
    ::zmq::context_t *ctx;

    /// 0MQ endpoint to which to bind a XREP socket to receive client requests
    ::std::string client_endpoint;

    /// 0MQ endpoint to which to connect to send log envelopes
    ::std::string logger_endpoint;

    /// Semaphore indicating whether the processor should remain alive
    ///
    /// When this goes to false, the request processor will cease processing
    /// new messages and will return from its run routine.
    bool *active;

    /// Maximum number of bytes that can be returned from a GetStream request
    uint32 get_stream_max_bytes;

    /// Maximum number of envelopes that can be returned from a GetStream request
    uint32 get_stream_max_envelopes;
};

/// Processes zippylog protocol requests
///
/// This class is designed to be an abstract base class. It implements core
/// functionality for parsing and verifying protocol requests. However, the
/// actual implementation is abstracted away in the various Handle* functions.
/// Derived classes should implement these functions.
///
/// Currently, we make the assumption that protocol requests arrive via
/// 0MQ sockets. Strictly speaking, this isn't very loosely coupled. However,
/// the various Process* functions don't expose 0MQ details, so it should be
/// possible to call into this class without serializing envelopes as 0MQ
/// messages. That being said, the constructor still requires the 0MQ
/// parameters (for now, at least)
class ZIPPYLOG_EXPORT RequestProcessor {
    public:
        // return code from the various request processors
        enum ResponseStatus {
            // processor is authoritative responder for this request
            // the Envelopes/messages set in a passed vector should be
            // sent to the client
            AUTHORITATIVE = 1,

            // processor deferred to send a response
            // caller should not send any response to client, as this will be
            // done by some other process
            DEFERRED = 2,
        };

        RequestProcessor(RequestProcessorStartParams &params);
        ~RequestProcessor();

        /// Runs the request processor
        /// Will listen for messages on the 0MQ socket specified in the start parameters
        /// This function executes forever in an infinite loop until a
        /// catastrophic error or the active parameter passed in the start
        /// parameters goes to false
        void Run();

        /// Perform processing activities
        ///
        /// Reads available client messages from the listening socket(s) and
        /// processes them.
        ///
        /// If no messages are available, waits up to the specified number of
        /// microseconds and process any that arrive before that time window
        /// expires.
        int Pump(long wait_microseconds);

        /// Processes received 0MQ messages
        ///
        /// The identities parameter is a list of 0MQ socket identities. These
        /// are used to craft the appropriate multipart 0MQ response message.
        /// The input parameter is a list of received 0MQ messages that should
        /// contain a request of some type. The output parameter will receive
        /// the list of 0MQ messages that would have been sent back to the
        /// client immediately.
        ///
        /// The request processor is not always authoritative for responses.
        /// For some request types, control is handed off to another processor
        /// for additional processing. In these scenarios, the output messages
        /// list might be empty. It all depends on the implementation.
        ///
        /// This function is called by Run() whenever it has received messages
        /// for processing. The preferred way to pass messages to the request
        /// processor is by sending them over the socket. However, this
        /// function is public just in case.
        void ProcessMessages(::std::vector< ::std::string > &identities, ::std::vector< ::zmq::message_t * > &input, ::std::vector<Envelope> &output);

        /// Processes a request envelope
        ///
        /// This is the main request processing function. If there are
        /// envelopes to be sent to the client, they are added to the passed
        /// vector.
        ///
        /// Callers should inspect the return value to determine how to handle
        /// response messages.
        ///
        /// Most people typically have no need to call this function. However,
        /// it is provided public just in case.
        ResponseStatus ProcessRequest(Envelope &e, ::std::vector<Envelope> &output);

    protected:

        /// callback to handle a validated request to subscribe to store changes
        virtual ResponseStatus HandleSubscribeStoreChanges(Envelope &request, ::std::vector<Envelope> &output) = 0;

        /// callback to handle a subscription to envelopes
        virtual ResponseStatus HandleSubscribeEnvelopes(Envelope &request, ::std::vector<Envelope> &output) = 0;

        /// callback to handle a subscription keepalive
        virtual ResponseStatus HandleSubscribeKeepalive(Envelope &request, ::std::vector<Envelope> &output) = 0;

        /// Callback to handle writing of envelopes
        ///
        /// Receives the path we are writing to (already validated to be a
        /// stream set or stream). If a stream set, it is validated to exist.
        /// If a stream, it may not exist.
        ///
        /// The synchronous parameter says whether to wait for writes before
        /// returning.
        virtual bool HandleWriteEnvelopes(const ::std::string &path, ::std::vector<Envelope> &to_write, bool synchronous) = 0;

        /// Process a StoreInfo request and populate the passed envelope with the response
        ///
        /// This function is typically called only by ProcessRequest()
        ResponseStatus ProcessStoreInfo(Envelope &request, ::std::vector<Envelope> &output);

        ResponseStatus ProcessBucketInfo(Envelope &request, ::std::vector<Envelope> &output);
        ResponseStatus ProcessStreamSetInfo(Envelope &request, ::std::vector<Envelope> &outpute);
        ResponseStatus ProcessStreamInfo(Envelope &request, ::std::vector<Envelope> &output);

        /// Process a GetStream request
        ResponseStatus ProcessGetStream(Envelope &request, ::std::vector<Envelope> &output);

        ResponseStatus ProcessSubscribeStoreChanges(Envelope &request, ::std::vector<Envelope> &output);

        ResponseStatus ProcessSubscribeEnvelopes(Envelope &request, ::std::vector<Envelope> &output);

        ResponseStatus ProcessSubscribeKeepalive(Envelope &request, ::std::vector<Envelope> &output);

        /// Process a WriteEnvelope request
        ResponseStatus ProcessWriteEnvelope(Envelope &request, ::std::vector<Envelope> &output);

        /// Checks that a path supplied by the client is valid
        ///
        /// Returns false if:
        ///
        ///  - the path is not a valid path
        ///  - the require_ values are true and that path component doesn't
        ///    exist in the path or on the store
        ///
        /// If the path does not validate, an error response is added to the
        /// output messages with an appropriate description of the failure.
        bool CheckPath(const ::std::string &path, ::std::vector<Envelope> &output, bool require_bucket = false, bool require_set = false, bool require_stream = false);

        /// Checks that the version in a message is THE version we support
        ///
        /// If it isn't, an error response is added to the output messages.
        bool CheckMessageVersion(uint32 seen_version, uint32 supported_version, ::std::vector<Envelope> &output);

        bool PopulateErrorResponse(::zippylog::protocol::response::ErrorCode code, ::std::string message, ::std::vector<Envelope> &msgs);

        ::zmq::context_t *ctx;
        ::std::string store_path;
        ::std::string logger_endpoint;
        ::std::string client_endpoint;

        ::zmq::socket_t * logger_sock;
        ::zmq::socket_t * socket;

        Store * store;

        ::std::string id;
        ::std::vector< ::std::string > current_request_identities;
        bool *active;

        /// maximum number of bytes we're allowed to return per GetStream request
        uint32 get_stream_max_bytes;

        /// maximum number of enevelopes we can return per GetStream request
        uint32 get_stream_max_envelopes;

private:
        RequestProcessor(const RequestProcessor &orig);
        RequestProcessor & operator=(const RequestProcessor &orig);

};

} // namespace

#endif