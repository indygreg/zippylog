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

#include <zippylog/zippylog.h>
#include <zippylog/store.hpp>
#include <zippylog/protocol/response.pb.h>

#include <zmq.hpp>

namespace zippylog {
namespace server {

using ::zmq::context_t;
using ::zmq::socket_t;

class ZIPPYLOG_EXPORT RequestProcessorStartParams {
public:
    // for ZMQ initialization on new threads
    context_t *ctx;

    // store worker operates on
    string store_path;

    // where to connect to receive requests
    string broker_endpoint;

    // where to send client subscription messages
    string streaming_subscriptions_endpoint;

    // where to send updates for existing subscriptions
    string streaming_updates_endpoint;

    // where to send log messages
    string logger_endpoint;

    // whether request processor should remain alive
    // caller sets this value to false when it wishes for the Run()
    // function to exit
    bool *active;
};

class ZIPPYLOG_EXPORT RequestProcessor {
    public:
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

        RequestProcessor(RequestProcessorStartParams params);
        ~RequestProcessor();

        // Runs the request processing loop
        // Listens for messages on a XREP socket connected to the endpoint
        // defined by the broker_endpoint parameter in the start parameters
        void Run();

        // Takes a 0MQ message (possibly multipart) as input and generates the output
        ResponseStatus ProcessRequest(const vector<string> &identities, const vector<::zmq::message_t *> &input, vector<Envelope> &output);

        // Process a StoreInfo request and populate the passed envelope with the response
        ResponseStatus ProcessStoreInfo(Envelope &response);

        // Process a Get request
        ResponseStatus ProcessGet(Envelope &request, vector<Envelope> &output);

        ResponseStatus ProcessSubscribeStoreChanges(Envelope &request, vector<Envelope> &output);

        ResponseStatus ProcessSubscribeEnvelopes(Envelope &request, vector<Envelope> &output);

        ResponseStatus ProcessSubscribeKeepalive(Envelope &request, vector<Envelope> &output);

    protected:
        bool PopulateErrorResponse(::zippylog::protocol::response::ErrorCode code, string message, vector<Envelope> &msgs);

        // start parameters
        context_t *ctx;
        string store_path;
        string broker_endpoint;
        string streaming_subscriptions_endpoint;
        string streaming_updates_endpoint;
        string logger_endpoint;
        bool *active;

        Store store;

        socket_t *socket;
        socket_t *subscriptions_sock;
        socket_t *subscription_updates_sock;
        socket_t *logger_sock;

        string id;

        // I would rather make this a function parameter b/c I don't like keeping
        // per-request state in the class. However, with future refactoring
        // (making this class an abstract base), it makes sense to keep it here
        // at the moment
        vector<string> current_request_identities;

private:
        RequestProcessor(const RequestProcessor &orig);
        RequestProcessor & operator=(const RequestProcessor &orig);

};

}} // namespaces

#endif