//  Copyright 2011 Gregory Szorc
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
#include <zippylog/device/device.hpp>
#include <zippylog/platform.hpp>
#include <zippylog/store.hpp>

#include <zmq.hpp>

namespace zippylog {

/// Parameters to instantiate a StoreWatcher class
class ZIPPYLOG_EXPORT StoreWatcherStartParams {
public:
    /// 0MQ context to use
    ::zmq::context_t *zctx;

    /// Store filesystem path to watch
    ::std::string store_path;

    /// 0MQ socket endpoint to send change notifications to
    ::std::string endpoint;

    /// 0MQ endpoint to send log messages to
    ::std::string logging_endpoint;

    /// Semaphore to control whether device should run
    ::zippylog::platform::ConditionalWait *active;
};

/// A directory watcher for file-based stores
///
/// This class watches the specified store directory for changes.
/// When changes are detected, it executes function callbacks, which
/// must be defined in an inherited class.
///
/// The directory watcher is currently designed to execute on its own
/// thread. Just instantiate a directory watcher and invoke Run(). This
/// function will block until the active semaphore contained in the start
/// parameters goes to false.
///
/// @todo move to device namespace
class ZIPPYLOG_EXPORT StoreWatcher : public ::zippylog::device::Device {
public:
    /// Instantiate a store watcher with the given parameters
    ///
    /// Will not actually start the store watcher. To do that, execute
    /// Run().
    ///
    /// @param params Parameters to construct watcher with
    StoreWatcher(StoreWatcherStartParams params);
    virtual ~StoreWatcher();

    /// Performs work
    ///
    /// @param timeout
    ::zippylog::device::PumpResult Pump(int32 timeout = 100000);

    ///@{
    /// Device hooks
    virtual void OnRunStart();
    virtual void OnRunFinish();
    ///@}

protected:
    /// Function that performs actions when something is created
    ///
    /// Receives the path that was added as a store path (e.g.
    /// "/bucket/store/20101107T1615") and a stat record that describes
    /// the filesystem entity.
    ///
    /// @param path Store path that was added
    /// @param stat Filesystem stat result of new path
    virtual void HandleAdded(::std::string path, platform::FileStat &stat) = 0;

    /// Function that performs actions when something is deleted
    ///
    /// @param path The path that was deleted
    virtual void HandleDeleted(::std::string path) = 0;

    /// Performs actions when something (probably a stream) is modified
    ///
    /// @param path The path that was modified
    /// @param stat The result of a stat() on the modified path
    virtual void HandleModified(::std::string path, platform::FileStat &stat) = 0;

    /// Sends a change notification across the socket
    ///
    /// @param e Envelope to send
    void SendChangeMessage(Envelope &e);

    /// The store we are watching
    SimpleDirectoryStore * _store;

    /// 0MQ context
    ::zmq::context_t *_ctx;

    /// 0MQ endpoint to send notification messages to
    ::std::string _endpoint;

    /// 0MQ endpoint to send logging messages to
    ::std::string logging_endpoint;

    /// Unique identifier of this device
    ::std::string id;

    /// 0MQ socket we are sending notifications on
    ::zmq::socket_t * socket;

    /// 0MQ socket we are sending logging messages on
    ::zmq::socket_t * logging_sock;

    /// The underlying watcher looking for store changes
    platform::DirectoryWatcher watcher;

private:
    // we don't provide these
    StoreWatcher(StoreWatcher const &orig);
    StoreWatcher & operator=(StoreWatcher const &orig);

};

}

#endif