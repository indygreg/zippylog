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

#ifndef STORE_WATCHER_HPP
#define STORE_WATCHER_HPP

#include <zippylog/zippylog.h>

#include <zippylog/platform.hpp>
#include <zippylog/store.hpp>

#include <zmq.hpp>

namespace zippylog {

using ::zmq::context_t;
using ::zmq::socket_t;

// provides file/directory change notification for stores
class ZIPPYLOG_EXPORT StoreWatcher {
public:
    StoreWatcher(Store *store, zmq::context_t *ctx, const string publisher_endpoint);
    void run();

protected:
    Store * _store;
    zmq::context_t *_ctx;
    string _endpoint;
    zmq::socket_t * socket;

    void SendChangeMessage(Envelope &e);
    void HandleAdded(string path, platform::FileStat &stat);
    void HandleDeleted(string path);
    void HandleModified(string path, platform::FileStat &stat);

};

}

#endif