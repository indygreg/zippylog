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
#include <zippylog/device/device.hpp>
#include <zippylog/persisted_state_manager.hpp>
#include <zippylog/store.hpp>

#include <zmq.hpp>

#include <string>

namespace zippylog {
namespace device {

/// Container for parameters used to initialize PersistedStateReactor
class ZIPPYLOG_EXPORT PersistedStateReactorStartParams {
public:
    /// Create a new container
    PersistedStateReactorStartParams() :
        ctx(NULL),
        active(NULL)
    { }

    /// 0MQ context
    ::zmq::context_t *ctx;

    /// Endpoint for socket that should receive store changes
    ::std::string store_change_endpoint;

    /// Endpoint for socket that leads back to clients
    ::std::string client_endpoint;

    /// Endpoint for socket that receives new persisted state requests
    ::std::string subscription_endpoint;

    /// Endpoint for socket that receives updates from clients
    ::std::string subscription_updates_endpoint;

    /// Endpoint for a logger to send logging information
    ::std::string logger_endpoint;

    /// Semaphore stating whether device should remain active
    ::zippylog::platform::ConditionalWait *active;

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
class ZIPPYLOG_EXPORT PersistedStateReactor : public ::zippylog::device::Device {
public:
    /// Construct a persisted state reactor from parameters
    ///
    /// @param params Options to define instance
    PersistedStateReactor(PersistedStateReactorStartParams const &params);

    ~PersistedStateReactor();

    /// Performs work on the device if there is work to be done
    ///
    /// Waits up to the specified time for work to become available, if none is available
    ::zippylog::device::PumpResult Pump(int32 timeout_microseconds);
protected:

    ///@{
    /// Persisted state manager handlers
    ///
    /// The persisted state manager calls into these for every subscription
    /// that cares about them. The functions formulate a response and send
    /// them to the client.
    static void PathAddedCallback(SubscriptionInfo const &subscription, ::std::string const &path, void *data);
    static void PathDeletedCallback(SubscriptionInfo const &subscription, ::std::string const &path, void *data);
    static void StreamAppendedCallback(SubscriptionInfo const &subscription, EnvelopeSubscriptionResponseState &state, void *data);
    ///@}

    /// Initializes the object
    ///
    /// Called by the constructor. Throws an exception on error.
    void Initialize();

    /// Processes a received store change message
    void ProcessStoreChangeMessage(::zmq::message_t &msg);

    /// Process a subscription update message
    void ProcessSubscriptionUpdate(::zmq::message_t &msg);

    /// 0MQ context
    ::zmq::context_t *ctx;

    /// Endpoint on which we listen for store change notifications
    ::std::string store_changes_endpoint;

    /// Endpoint that leads back to clients
    ::std::string client_endpoint;

    /// Endpoint that receives new persisted state requests
    ::std::string subscription_endpoint;

    /// Endpoint that receives persisted state updates
    ::std::string subscription_updates_endpoint;

    /// Endpoint for where to send log messages
    ::std::string logger_endpoint;

    /// Parameters used to initialize the PersistedStateManager
    PersistedStateManagerStartParams manager_params;

    /// Identifier for this instance
    ::std::string id;

    /// SUB that receives store change notifications
    ::zmq::socket_t *store_changes_sock;

    /// PUSH that sends messages back to clients
    ::zmq::socket_t *client_sock;

    /// PULL that receives new persisted state requests
    ::zmq::socket_t *subscription_sock;

    /// SUB that receives persisted state updates
    ::zmq::socket_t *subscription_updates_sock;

    /// PUSH that sends logging messages
    ::zmq::socket_t *logger_sock;

    /// Polling structure for 0MQ
    ::zmq::pollitem_t pollitems[3];

    /// The thing that actually handles persisted state
    PersistedStateManager *manager;

private:
    // disallow copy constructor and assignment operator
    PersistedStateReactor(PersistedStateReactor const &);
    PersistedStateReactor & operator=(PersistedStateReactor const &);
};

}} // namespace

#endif // file include