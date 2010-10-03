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

#ifndef ZIPPYLOG_SERVER_HPP_
#define ZIPPYLOG_SERVER_HPP_

#include <zippylog/zippylog.h>
#include <zippylog/store.hpp>

#include <zmq.hpp>

namespace zippylog {
namespace server {

using ::zmq::context_t;

// this is sent to new workers as they are started. it gives them all the
// info they need to start servicing requests
typedef struct request_processor_start_data {
    // for ZMQ initialization on new threads
    context_t *ctx;

    // store worker operates on
    zippylog::Store *store;

    // where to connect to receive requests
    const char *broker_endpoint;

    // where to send client subscription messages
    const char *streaming_subscriptions_endpoint;

    // where to send updates for existing subscriptions
    const char *streaming_updates_endpoint;

    // where to send log messages
    const char *logger_endpoint;

    bool active;
} request_processor_start_data;

typedef struct stream_processor_start_data {
    context_t *ctx;
    zippylog::Store *store;
    const char *socket_endpoint;
} stream_processor_start_data;

class ZIPPYLOG_EXPORT Request {
public:
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
    };

    // function that waits and processes client requests as they arrive
    // suitable to be called upon thread initialization
    static void * __stdcall request_processor(void *data);

};

}} // namespaces

#endif