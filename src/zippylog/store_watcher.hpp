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

#include <zippylog/zippylog.hpp>

#include <zippylog/platform.hpp>
#include <zippylog/store.hpp>

#include <zmq.hpp>

namespace zippylog {

// parameters to instantiate a StoreWatcher class
class ZIPPYLOG_EXPORT StoreWatcherStartParams {
public:
    ::zmq::context_t *zctx;
    ::std::string store_path;
    ::std::string endpoint;
    ::std::string logging_endpoint;
    bool *active;
};

// A directory watcher for file-based stores
//
// This class watches the specified store directory for changes.
// When changes are detected, it executes function callbacks, which
// must be defined in an inherited class.
//
// The directory watcher is currently designed to execute on its own
// thread. Just instantiate a directory watcher and invoke Run(). This
// function will block until the active semaphore contained in the start
// parameters goes to false.
class ZIPPYLOG_EXPORT StoreWatcher {
public:
    // Instantiate a store watcher with the given parameters
    //
    // Will not actually start the store watcher. To do that, execute
    // Run().
    StoreWatcher(StoreWatcherStartParams params);
    ~StoreWatcher();

    // Run the store watcher
    //
    // Will not return until a fatal error is encountered or until the
    // active semaphore from the start parameters goes to false;
    void Run();

protected:
    // Function that performs actions when something is created
    //
    // Receives the path that was added as a store path (e.g.
    // "/bucket/store/20101107T1615") and a stat record that describes
    // the filesystem entity.
    virtual void HandleAdded(::std::string path, platform::FileStat &stat) = 0;

    // Function that performs actions when something is deleted
    virtual void HandleDeleted(::std::string path) = 0;

    // Performs actions when something (probably a stream) is modified
    virtual void HandleModified(::std::string path, platform::FileStat &stat) = 0;

    void SendChangeMessage(Envelope &e);

    Store _store;
    ::zmq::context_t *_ctx;
    ::std::string _endpoint;
    ::std::string logging_endpoint;
    ::std::string id;
    ::zmq::socket_t * socket;
    ::zmq::socket_t * logging_sock;
    bool *active;
    platform::DirectoryWatcher watcher;



private:
    // we don't provide these
    StoreWatcher(const StoreWatcher &orig);
    StoreWatcher & operator=(const StoreWatcher &orig);

};

}

#endif