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

#ifndef ZIPPYLOG_CLIENT_HPP_
#define ZIPPYLOG_CLIENT_HPP_

#include <zippylog/zippylog.hpp>
#include <zippylog/envelope.hpp>
#include <zippylog/platform.hpp>
#include <zippylog/protocol.pb.h>
#include <zippylog/protocol/response.pb.h>

#include <map>
#include <string>
#include <vector>
#include <zmq.hpp>

namespace zippylog {
namespace client {

/// Represents a segment of a stream
///
/// Stream segments have begin and end offsets and contain envelopes.
/// Segments are returned from requests to obtain parts of streams.
class ZIPPYLOG_EXPORT StreamSegment {
    public:
        StreamSegment();
        ~StreamSegment();

        bool SetPath(const ::std::string path);
        bool SetStartOffset(uint64 offset);
        bool SetEndOffset(uint64 offset);
        bool SetBytesSent(uint32 count);
        bool SetEnvelopesSent(uint32 number);
        bool AddEnvelope(Envelope e);

        ::std::string Path;
        uint64 StartOffset;
        uint64 EndOffset;
        uint32 BytesSent;
        uint32 EnvelopesSent;
        ::std::vector<Envelope> Envelopes;

};

/// Function types for callbacks when the client has received a subscribed event
///
/// The first string parameter is the subscription id. The final parameter is
/// always a void *. Callers can associate the subscription id with their own
/// metadata independent of the client API. Their callbacks can fetch this data
/// at callback time.
typedef void (__stdcall * StoreChangeStreamAddedCallback)(::std::string, protocol::StoreChangeStreamAdded &, void *);
typedef void (__stdcall * StoreChangeStreamDeletedCallback)(::std::string, protocol::StoreChangeStreamDeleted &, void *);
typedef void (__stdcall * StoreChangeStreamAppendedCallback)(::std::string, protocol::StoreChangeStreamAppended &, void *);
typedef void (__stdcall * StoreChangeBucketAddedCallback)(::std::string, protocol::StoreChangeBucketAdded &, void *);
typedef void (__stdcall * StoreChangeBucketDeletedCallback)(::std::string, protocol::StoreChangeBucketDeleted &, void *);
typedef void (__stdcall * StoreChangeStreamSetAddedCallback)(::std::string, protocol::StoreChangeStreamSetAdded &, void *);
typedef void (__stdcall * StoreChangeStreamSetDeletedCallback)(::std::string, protocol::StoreChangeStreamSetDeleted &, void *);

/// Callback executed when a store info response is received
typedef void (__stdcall * StoreInfoCallback)(protocol::StoreInfo &, void *);

/// Executed when a stream segment is received
///
/// Invoked with the path, start offset, and the data in that segment
typedef void (__stdcall * StreamSegmentCallback)(const ::std::string &, uint64, StreamSegment &, void *);

/// Executed when an envelope is received
///
/// Invoked with subscription id, the envelope, and supplied user data to subscription
typedef void (__stdcall * EnvelopeCallback)(const ::std::string &, Envelope &, void *);

/// Defines the set of function callbacks for a subscription.
///
/// Not all callback types are valid for every subscription type. If a
/// callback in not defined, no function is executed when an event for that
/// callback is received.
class ZIPPYLOG_EXPORT SubscriptionCallback {
public:
    SubscriptionCallback();

    StoreChangeStreamAddedCallback      StreamAdded;
    StoreChangeStreamDeletedCallback    StreamDeleted;
    StoreChangeStreamAppendedCallback   StreamAppended;
    StoreChangeBucketAddedCallback      BucketAdded;
    StoreChangeBucketDeletedCallback    BucketDeleted;
    StoreChangeStreamSetAddedCallback   StreamSetAdded;
    StoreChangeStreamSetDeletedCallback StreamSetDeleted;
    StoreInfoCallback                   StoreInfo;
    EnvelopeCallback                    Envelope;
};

/// Represents a client subscription
///
/// Used internally by the client to keep track of state.
class Subscription {
public:
    Subscription();

    friend class Client;
protected:
    ::std::string id;

    platform::Timer expiration_timer;

    SubscriptionCallback cb;

    void *data;
};

/// Keeps track of requests sent whose replies have not yet been seen
///
/// Used internally by the client.
class OutstandingRequest {
public:
    OutstandingRequest();

    friend class Client;

protected:
    ::std::string id;

    StoreInfoCallback cb_store_info;
    StreamSegmentCallback cb_stream_segment;
    SubscriptionCallback subscription_callback;

    void *data;
};

/// zippylog protocol client
///
/// Clients connect to 0MQ endpoints and send zippylog protocol requests and
/// react to responses.
class ZIPPYLOG_EXPORT Client {
    public:
        /// Create a client that connects to the specified 0MQ endpoint
        ///
        /// The 0MQ context is required. If not set, an exception will be
        /// thrown.
        Client(::zmq::context_t *ctx, const ::std::string &endpoint);
        ~Client();

        // Asynchronously obtain the store info. Executes supplied callback when store
        // info available
        bool StoreInfo(StoreInfoCallback callback, void *data = NULL);

        // Cancels the subscription with specified ID
        bool CancelSubscription(const ::std::string &id);

        // cancels all subscriptions registered with the client
        bool CancelAllSubscriptions();

        // Whether the client has a subscription with the specified subscription ID
        bool HasSubscription(const ::std::string &id);

        // Waits up to N microseconds for response messages to arrive and process
        // any that arrive. Returns whether messages were processed
        // callers will likely want to call this function in an event loop
        bool TryProcessMessages(uint32 timeout);

        // renew all subscriptions
        // unless the bool parameter is true, only the subscriptions that are
        // near to expiration will be renewed. It is generally OK to let this
        // be
        bool RenewSubscriptions(bool force=false);

        bool Get(const ::std::string &path, uint64 start_offset, StreamSegmentCallback callback, void *data = NULL);
        bool Get(const ::std::string &path, uint64 start_offset, uint64 stop_offset, StreamSegmentCallback callback, void *data = NULL);
        bool Get(const ::std::string &path, uint64 start_offset, uint32 max_response_bytes, StreamSegmentCallback callback, void *data = NULL);

        // Subscribe to store change events
        // This subscribes to events that describe the store, not envelopes in
        // streams. For that, use one of the other Subscribe* functions.
        //
        // The first argument is the path in the store to subscribe to. To
        // subscribe to all paths, set this path to "/".
        //
        // The subscription will receive notifications for numerous store
        // change events. However, unless your SubscriptionCallback defines
        // functions for all of them, some events will be dropped by the client.
        bool SubscribeStoreChanges(const ::std::string &path, SubscriptionCallback &callback, void *data = NULL);

        // Subscribes to new envelopes written on the server
        bool SubscribeEnvelopes(const ::std::string &path, SubscriptionCallback &callback, void *data = NULL);

        // subscribes to new envelopes w/ Lua code specifying additional features
        bool SubscribeEnvelopes(const ::std::string &path, const ::std::string &lua, SubscriptionCallback &callback, void *data = NULL);

    protected:
        // socket connect to server
        ::zmq::socket_t *client_sock;

        ::std::map< ::std::string, Subscription > subscriptions;
        ::std::map< ::std::string, OutstandingRequest > outstanding;

        ::zmq::pollitem_t pollitem[1];

        uint32 subscription_renewal_offset;

        bool SendRequest(Envelope &e, OutstandingRequest &req);

        // processe an individual pending zeromq message on the socket
        // this function assumes messages are available
        // pending message could be multipart
        bool ProcessPendingMessage();

        // validates that a received SubscriptionStart message is OK
        // returns false if we don't know how to handle message fields or if
        // we don't know about the subscription
        bool ValidateSubscriptionStart(protocol::response::SubscriptionStart &start);

        // handles a response to a subscription
        bool HandleSubscriptionResponse(Envelope &e, protocol::response::SubscriptionStart &start, ::std::vector< ::zmq::message_t * > &msgs);

        // handles a response to a normal/outstanding request
        bool HandleRequestResponse(Envelope &e, ::std::vector< ::zmq::message_t * > &msgs);

    private:
        // disable copy constructor and assignment operator
        Client(const Client &orig);
        Client & operator=(const Client &orig);
};

}} // namespaces

#endif