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

#include <zippylog/zippylog.h>
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

using protocol::StoreInfo;
using protocol::response::SubscriptionStart;
using ::std::map;
using ::std::string;
using ::std::vector;
using ::zmq::socket_t;
using ::zmq::context_t;
using ::zmq::message_t;
using ::zmq::pollitem_t;
using ::zippylog::Envelope;

/*
The client API is pretty bad at the moment. In the future, everything will
likely be implemented via callbacks. There will be synchronous and asynchronous
calls for every functionality. Subscriptions, by definition, will be
asynchronous. This will all get flushed out eventually.
*/

// represents a stream segment response (from Get requests)
class ZIPPYLOG_EXPORT StreamSegment {
    public:
        StreamSegment();
        ~StreamSegment();

        bool SetPath(const string path);
        bool SetStartOffset(uint64 offset);
        bool SetEndOffset(uint64 offset);
        bool SetBytesSent(uint32 count);
        bool SetEnvelopesSent(uint32 number);
        bool AddEnvelope(Envelope e);

        string Path;
        uint64 StartOffset;
        uint64 EndOffset;
        uint32 BytesSent;
        uint32 EnvelopesSent;
        vector<Envelope> Envelopes;

};

// function types for callbacks when the client has received a subscribed event
// the first string parameter is the subscription id
// callers can associate the subscription id with their own metadata
// independent of the client API. their callbacks can fetch this data at
// callback time.
typedef void (__stdcall * StoreChangeStreamAddedCallback)(string, protocol::StoreChangeStreamAdded &, void *);
typedef void (__stdcall * StoreChangeStreamDeletedCallback)(string, protocol::StoreChangeStreamDeleted &, void *);
typedef void (__stdcall * StoreChangeStreamAppendedCallback)(string, protocol::StoreChangeStreamAppended &, void *);
typedef void (__stdcall * StoreChangeBucketAddedCallback)(string, protocol::StoreChangeBucketAdded &, void *);
typedef void (__stdcall * StoreChangeBucketDeletedCallback)(string, protocol::StoreChangeBucketDeleted &, void *);
typedef void (__stdcall * StoreChangeStreamSetAddedCallback)(string, protocol::StoreChangeStreamSetAdded &, void *);
typedef void (__stdcall * StoreChangeStreamSetDeletedCallback)(string, protocol::StoreChangeStreamSetDeleted &, void *);

// callback executed when a store info response is received
typedef void (__stdcall * StoreInfoCallback)(protocol::StoreInfo &, void *);

// executed when a stream segment is received
// invoked with the path, start offset, and the data in that segment
typedef void (__stdcall * StreamSegmentCallback)(const string &, uint64, StreamSegment &, void *);

// executed when an envelope is received
// invoked with subscription id, the envelope, and supplied user data to subscription
typedef void (__stdcall * EnvelopeCallback)(const string &, Envelope &, void *);

// The SubscriptionCallback defines the set of function callbacks for a
// subscription. Not all callback types are valid for every subscription
// type.
// If a callback in not defined, no function is executed when an event for
// that callback is received.
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

// represents a client subscription
class Subscription {
public:
    Subscription();

    friend class Client;
protected:
    string id;

    platform::Timer expiration_timer;

    SubscriptionCallback cb;

    void *data;
};

// keeps track of requests sent whose replies have not yet been seen
class OutstandingRequest {
public:
    OutstandingRequest();

    friend class Client;

protected:
    string id;

    StoreInfoCallback cb_store_info;
    StreamSegmentCallback cb_stream_segment;
    SubscriptionCallback subscription_callback;

    void *data;
};

// Client instances talk to zippylog servers
class ZIPPYLOG_EXPORT Client {
    public:
        // establish a client and bind to the location specified
        Client(context_t *ctx, const string &connect);
        ~Client();

        // Asynchronously obtain the store info. Executes supplied callback when store
        // info available
        bool StoreInfo(StoreInfoCallback callback, void *data = NULL);

        // Cancels the subscription with specified ID
        bool CancelSubscription(const string &id);

        // cancels all subscriptions registered with the client
        bool CancelAllSubscriptions();

        // Whether the client has a subscription with the specified subscription ID
        bool HasSubscription(const string &id);

        // Waits up to N microseconds for response messages to arrive and process
        // any that arrive. Returns whether messages were processed
        // callers will likely want to call this function in an event loop
        bool TryProcessMessages(uint32 timeout);

        // renew all subscriptions
        // unless the bool parameter is true, only the subscriptions that are
        // near to expiration will be renewed. It is generally OK to let this
        // be
        bool RenewSubscriptions(bool force=false);

        bool Get(const string &path, uint64 start_offset, StreamSegmentCallback callback, void *data = NULL);
        bool Get(const string &path, uint64 start_offset, uint64 stop_offset, StreamSegmentCallback callback, void *data = NULL);
        bool Get(const string &path, uint64 start_offset, uint32 max_response_bytes, StreamSegmentCallback callback, void *data = NULL);

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
        bool SubscribeStoreChanges(const string &path, SubscriptionCallback &callback, void *data = NULL);

        // Subscribes to new envelopes written on the server
        bool SubscribeEnvelopes(const string &path, SubscriptionCallback &callback, void *data = NULL);

    protected:
        // socket connect to server
        socket_t *client_sock;

        map<string, Subscription> subscriptions;
        map<string, OutstandingRequest> outstanding;

        pollitem_t * pollitem;

        uint32 subscription_renewal_offset;

        bool SendRequest(Envelope &e, OutstandingRequest &req);

        // processe an individual pending zeromq message on the socket
        // this function assumes messages are available
        // pending message could be multipart
        bool ProcessPendingMessage();

        // validates that a received SubscriptionStart message is OK
        // returns false if we don't know how to handle message fields or if
        // we don't know about the subscription
        bool ValidateSubscriptionStart(SubscriptionStart &start);

        // handles a response to a subscription
        bool HandleSubscriptionResponse(Envelope &e, SubscriptionStart &start, vector<message_t *> &msgs);

        // handles a response to a normal/outstanding request
        bool HandleRequestResponse(Envelope &e, vector<message_t *> &msgs);
};

}} // namespaces

#endif