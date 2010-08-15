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

#ifndef PBLOG_SERVER_HPP_
#define PBLOG_SERVER_HPP_

#include <pblog/pblog.h>

#include <apr_thread_proc.h>
#include <zmq.hpp>

namespace pblog {
namespace server {

typedef struct worker_start_data {
    ::zmq::context_t *ctx;
    apr_pool_t *p;
    long client_timeout;
} worker_start_data;

PBLOG_EXPORT void * __stdcall worker(apr_thread_t *thread, void *data);

}} // namespaces

#endif