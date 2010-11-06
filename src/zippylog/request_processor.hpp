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
    bool *active;
};

class ZIPPYLOG_EXPORT RequestProcessor {
public:
        RequestProcessor(RequestProcessorStartParams params);
        ~RequestProcessor();

        void Run();

    enum state {
        CREATE_SOCKET = 1,
        WAITING = 2,
        RESET_CONNECTION = 3,
        PROCESS_REQUEST = 4,
        SEND_ENVELOPE_AND_DONE = 5,
        SEND_ERROR_RESPONSE = 6,
        PROCESS_STOREINFO = 7,
        PROCESS_GET = 8,
        PROCESS_SUBSCRIBE_STORE_CHANGES = 9,
        SETUP_INITIAL_SOCKETS = 10,
        REQUEST_CLEANUP = 11,
        PROCESS_SUBSCRIBE_KEEPALIVE = 12,
        PROCESS_SUBSCRIBE_ENVELOPES = 13,
    };

protected:
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

private:
    RequestProcessor(const RequestProcessor &orig);
    RequestProcessor & operator=(const RequestProcessor &orig);

};

}} // namespaces

#endif