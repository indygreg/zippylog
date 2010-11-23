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
#include <zippylog/protocol/response.pb.h>

#include <zmq.hpp>

namespace zippylog {

/// Used to construct a request processor
class ZIPPYLOG_EXPORT RequestProcessorStartParams {
public:
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
            // processor encountered a significant error and couldn't
            // process the request. this likely resembles a coding bug
            PROCESS_ERROR = 1,

            // processor is authoritative responder for this request
            // the Envelopes/messages set in a passed vector should be
            // sent to the client
            AUTHORITATIVE = 2,

            // processor deferred to send a response
            // caller should not send any response to client, as this will be
            // done by some other process
            DEFERRED = 3,
        };

        RequestProcessor(RequestProcessorStartParams &params);
        ~RequestProcessor();

        /// Runs the request processor
        /// Will listen for messages on the 0MQ socket specified in the start parameters
        /// This function executes forever in an infinite loop until a
        /// catastrophic error or the active parameter passed in the start
        /// parameters goes to false
        void Run();

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
        ResponseStatus ProcessRequest(Envelope &e, vector<Envelope> &output);

    protected:

        /// callback to handle a validated request to subscribe to store changes
        /// TODO should probably have a custom class for the request instance
        virtual ResponseStatus HandleSubscribeStoreChanges(Envelope &request, vector<Envelope> &output) = 0;

        /// callback to handle a subscription to envelopes
        /// TODO specific class for request
        virtual ResponseStatus HandleSubscribeEnvelopes(Envelope &request, vector<Envelope> &output) = 0;

        /// callback to handle a subscription keepalive
        /// TODO specific class for request
        virtual ResponseStatus HandleSubscribeKeepalive(Envelope &request, vector<Envelope> &output) = 0;


        /// Process a StoreInfo request and populate the passed envelope with the response
        ///
        /// This function is typically called only by ProcessRequest()
        ResponseStatus ProcessStoreInfo(Envelope &response);

        /// Process a Get request
        ResponseStatus ProcessGet(Envelope &request, vector<Envelope> &output);

        ResponseStatus ProcessSubscribeStoreChanges(Envelope &request, vector<Envelope> &output);

        ResponseStatus ProcessSubscribeEnvelopes(Envelope &request, vector<Envelope> &output);

        ResponseStatus ProcessSubscribeKeepalive(Envelope &request, vector<Envelope> &output);

        bool PopulateErrorResponse(::zippylog::protocol::response::ErrorCode code, string message, vector<Envelope> &msgs);

        ::zmq::context_t *ctx;
        string store_path;
        string logger_endpoint;
        string client_endpoint;

        ::zmq::socket_t * logger_sock;
        ::zmq::socket_t * socket;

        Store store;

        string id;
        vector<string> current_request_identities;
        bool *active;

private:
        RequestProcessor(const RequestProcessor &orig);
        RequestProcessor & operator=(const RequestProcessor &orig);

};

} // namespace

#endif