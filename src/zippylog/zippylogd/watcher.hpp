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

#ifndef ZIPPYLOGD_STORE_WATCHER_HPP
#define ZIPPYLOGD_STORE_WATCHER_HPP

#include <zippylog/zippylog.h>

#include <zippylog/store.hpp>
#include <zippylog/store_watcher.hpp>

namespace zippylog {
namespace zippylogd {

// Used to initialize Watcher instances
class ZIPPYLOG_EXPORT WatcherStartParams {
public:
    ::zippylog::StoreWatcherStartParams params;

    // 0MQ socket endpoint on which to bind a PUB socket
    string socket_endpoint;
};

// Store watcher implementation for zippylogd
//
// Whenever changes are seen, forwards store change events on a 0MQ PUB socket
// whose endpoint is defined at construction time.
class ZIPPYLOG_EXPORT Watcher : public ::zippylog::StoreWatcher {
public:
    // Construct a watcher that sends events to a 0MQ PUB socket
    Watcher(WatcherStartParams &params);
    ~Watcher();

protected:
    void HandleAdded(string path, platform::FileStat &stat);
    void HandleDeleted(string path);
    void HandleModified(string path, platform::FileStat &stat);

    void SendChangeMessage(Envelope &e);

    string endpoint;
    ::zmq::socket_t * socket;

private:
    Watcher(const Watcher &orig);
    Watcher & operator=(const Watcher &orig);
};

}} // namespaces

#endif