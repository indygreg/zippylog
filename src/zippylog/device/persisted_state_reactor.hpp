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

#ifndef ZIPPYLOG_DEVICE_PERSISTED_STATE_REACTOR_HPP_
#define ZIPPYLOG_DEVICE_PERSISTED_STATE_REACTOR_HPP_

#include <zippylog/zippylog.hpp>
#include <zippylog/persisted_state_manager.hpp>
#include <zippylog/store.hpp>

#include <zmq.hpp>

#include <string>

namespace zippylog {
namespace device {

class ZIPPYLOG_EXPORT PersistedStateReactorStartParams {
public:
    PersistedStateReactorStartParams() :
        ctx(NULL),
        active(NULL)
    { }

    /// 0MQ context
    ::zmq::context_t *ctx;

    ::std::string store_change_endpoint;
    ::std::string client_endpoint;
    ::std::string subscription_endpoint;
    ::std::string subscription_updates_endpoint;
    ::std::string logger_endpoint;

    /// Semaphore stating whether device should remain active
    ///
    /// When the boolean pointed to goes to false, Run() will terminate
    bool *active;

    /// Parameters used to construct a persisted state observer
    PersistedStateManagerStartParams manager_params;
};

/// Device that reacts to events related to persisted state
///
/// The device is responsible for managing subscriptions and plugins and
/// handling all events that these are interested in.
///
/// Interaction with the device is performed through a number of 0MQ sockets:
///
///   - store_changes - SUB that receives store change notifications
///   - client        - PUSH that sends subscription responses to clients
///   - subscriptions - PULL that receives new subscription requests
///   - subscription_updates - SUB that receives updates concerning existing subscriptions
///   - logger        - PUSH that sends logging info from the device
///
/// The general pattern is to instantiate this class on a thread and call
/// Run(), which will block until the device is shut down.
class ZIPPYLOG_EXPORT PersistedStateReactor {
public:
    PersistedStateReactor(const PersistedStateReactorStartParams &params);

    ~PersistedStateReactor();

    /// Runs the reactor
    ///
    /// This function runs in the foreground (blocks) until the semaphore in
    /// the start parameters tells it not to.
    void Run();

    /// Performs work on the device if there is work to be done
    ///
    /// Waits up to the specified time for work to become available, if none is available
    void Pump(int32 timeout_microseconds);
protected:

    /// Initializes the object
    ///
    /// Called by the constructor. Throws an exception on error.
    void Initialize();

    /// Processes a received store change message
    void ProcessStoreChangeMessage(::zmq::message_t &msg);

    void ProcessSubscriptionUpdate(::zmq::message_t &msg);

    // variables passed in to constructor
    ::zmq::context_t *ctx;

    ::std::string store_changes_endpoint;
    ::std::string client_endpoint;
    ::std::string subscription_endpoint;
    ::std::string subscription_updates_endpoint;
    ::std::string logger_endpoint;

    bool *active;

    PersistedStateManagerStartParams manager_params;

    // created by instance
    ::std::string id;

    ::zmq::socket_t *store_changes_sock;
    ::zmq::socket_t *client_sock;
    ::zmq::socket_t *subscription_sock;
    ::zmq::socket_t *subscription_updates_sock;
    ::zmq::socket_t *logger_sock;

    ::zmq::pollitem_t pollitems[3];

    PersistedStateManager *manager;

private:
    // disallow copy constructor and assignment operator
    PersistedStateReactor(const PersistedStateReactor &);
    PersistedStateReactor & operator=(const PersistedStateReactor &);
};

}} // namespace

#endif // file include