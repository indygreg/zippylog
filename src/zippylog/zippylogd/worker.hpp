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

#ifndef ZIPPYLOG_ZIPPYLOGD_WORKER_HPP_
#define ZIPPYLOG_ZIPPYLOGD_WORKER_HPP_

#include <zippylog/zippylog.hpp>

#include <zippylog/request_processor.hpp>
#include <zmq.hpp>

namespace zippylog {
namespace zippylogd {

class ZIPPYLOG_EXPORT WorkerStartParams {
public:
    // where to send client subscription messages
    string streaming_subscriptions_endpoint;

    // where to send updates for existing subscriptions
    string streaming_updates_endpoint;

    ::zippylog::RequestProcessorStartParams request_processor_params;
};

// Processes zippylog protocol requests
//
// This class is designed to be an abstract base class. It implements core
// functionality for parsing and verifying protocol requests. However, the
// actual implementation is abstracted away in the various Handle* functions.
// Derived classes should implement these functions.
class ZIPPYLOG_EXPORT Worker : public ::zippylog::RequestProcessor {
    public:
        Worker(WorkerStartParams &params);
        ~Worker();

    protected:
        ResponseStatus HandleSubscribeStoreChanges(Envelope &request, vector<Envelope> &output);
        ResponseStatus HandleSubscribeEnvelopes(Envelope &request, vector<Envelope> &output);
        ResponseStatus HandleSubscribeKeepalive(Envelope &request, vector<Envelope> &output);

        string streaming_subscriptions_endpoint;
        string streaming_updates_endpoint;

        ::zmq::socket_t *subscriptions_sock;
        ::zmq::socket_t *subscription_updates_sock;
};

}} // namespaces

#endif