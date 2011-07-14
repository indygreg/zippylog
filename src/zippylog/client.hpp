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

// forward declarations for later
class Client;

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

        /// Set fields in the stream segment from values in another instance
        bool CopyFrom(const StreamSegment &orig);

        ::std::string Path;
        uint64 StartOffset;
        uint64 EndOffset;
        uint32 BytesSent;
        uint32 EnvelopesSent;
        ::std::vector<Envelope> Envelopes;
};

/// Records what parts of a stream have been fetched
class ZIPPYLOG_EXPORT StreamFetchState {
public:
    StreamFetchState() : end_offset(0) { }

    uint64 end_offset;
};

class ZIPPYLOG_EXPORT StoreMirrorState {
public:
    StoreMirrorState() { }

    void SetStreamState(const ::std::string &path, StreamFetchState state);

    void SetStreamEndOffset(const ::std::string &path, uint64 offset);

    friend class Client;
protected:

    // maps path to fetch state
    ::std::map< ::std::string, StreamFetchState > states;
};

/// Function types for callbacks when the client has received a subscribed event
///
/// The first string parameter is the subscription id. The final parameter is
/// always a void *. Callers can associate the subscription id with their own
/// metadata independent of the client API. Their callbacks can fetch this data
/// at callback time.
typedef void (StoreChangeStreamAddedCallback)(Client *, ::std::string, protocol::StoreChangeStreamAddedV1 &, void *);
typedef void (StoreChangeStreamDeletedCallback)(Client *, ::std::string, protocol::StoreChangeStreamDeletedV1 &, void *);
typedef void (StoreChangeStreamAppendedCallback)(Client *, ::std::string, protocol::StoreChangeStreamAppendedV1 &, void *);
typedef void (StoreChangeBucketAddedCallback)(Client *, ::std::string, protocol::StoreChangeBucketAddedV1 &, void *);
typedef void (StoreChangeBucketDeletedCallback)(Client *, ::std::string, protocol::StoreChangeBucketDeletedV1 &, void *);
typedef void (StoreChangeStreamSetAddedCallback)(Client *, ::std::string, protocol::StoreChangeStreamSetAddedV1 &, void *);
typedef void (StoreChangeStreamSetDeletedCallback)(Client *, ::std::string, protocol::StoreChangeStreamSetDeletedV1 &, void *);

/// Callback executed when a ping response is received
typedef void (PingCallback)(Client *, void *);

/// Callback for feature specification responses
///
/// Receives a pointer to the client instance firing the callback, the
/// response, and a pointer to arbitrary data supplied at request time.
typedef void (GetFeaturesCallback)(Client *, protocol::response::FeatureSpecificationV1 &, void *);

/// Callback executed when a store info response is received
typedef void (StoreInfoCallback)(Client *, protocol::StoreInfoV1 &, void *);

/// Callback for bucket info responses
typedef void (BucketInfoCallback)(Client *, protocol::BucketInfoV1 &, void *);

/// Callback for stream set info responses
typedef void (StreamSetInfoCallback)(Client *, protocol::StreamSetInfoV1 &, void *);

/// Callback for stream info responses
typedef void (StreamInfoCallback)(Client *, protocol::StreamInfoV1 &, void *);

/// Executed when a stream segment is received
///
/// Invoked with the path, start offset, and the data in that segment
typedef void (StreamSegmentCallback)(Client *, const ::std::string &, uint64, StreamSegment &, void *);

/// Executed when an envelope is received
///
/// Invoked with subscription id, the envelope, and supplied user data to subscription
typedef void (EnvelopeCallback)(Client *, const ::std::string &, Envelope &, void *);

/// Defines the set of function callbacks for a subscription.
///
/// Not all callback types are valid for every subscription type. If a
/// callback in not defined, no function is executed when an event for that
/// callback is received.
class ZIPPYLOG_EXPORT SubscriptionCallbackInfo {
public:
    SubscriptionCallbackInfo() :
        StreamAdded(NULL),
        StreamDeleted(NULL),
        StreamAppended(NULL),
        BucketAdded(NULL),
        BucketDeleted(NULL),
        StreamSetAdded(NULL),
        StreamSetDeleted(NULL),
        StoreInfo(NULL),
        Envelope(NULL)
    { }

    StoreChangeStreamAddedCallback *      StreamAdded;
    StoreChangeStreamDeletedCallback *    StreamDeleted;
    StoreChangeStreamAppendedCallback *   StreamAppended;
    StoreChangeBucketAddedCallback *      BucketAdded;
    StoreChangeBucketDeletedCallback *    BucketDeleted;
    StoreChangeStreamSetAddedCallback *   StreamSetAdded;
    StoreChangeStreamSetDeletedCallback * StreamSetDeleted;
    StoreInfoCallback *                   StoreInfo;
    EnvelopeCallback *                    Envelope;
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

    SubscriptionCallbackInfo cb;

    void *data;
};

/// Keeps track of requests sent whose replies have not yet been seen
///
/// Used internally by the client.
class OutstandingRequest {
public:
    OutstandingRequest() :
        cb_ping(NULL),
        cb_features(NULL),
        cb_store_info(NULL),
        cb_bucket_info(NULL),
        cb_stream_set_info(NULL),
        cb_stream_info(NULL),
        cb_stream_segment(NULL),
        data(NULL)
    { }

    friend class Client;

protected:
    ::std::string id;

    PingCallback *          cb_ping;
    GetFeaturesCallback *   cb_features;
    StoreInfoCallback *     cb_store_info;
    BucketInfoCallback *    cb_bucket_info;
    StreamSetInfoCallback * cb_stream_set_info;
    StreamInfoCallback *    cb_stream_info;
    StreamSegmentCallback * cb_stream_segment;

    SubscriptionCallbackInfo callbacks;

    void *data;
};

/// Client that talks to a server
///
/// An individual client instance connects to a single 0MQ socket. The client
/// issues zippylog protocol request messages, sends them through the socket,
/// and processes the responses.
///
/// For many methods, the client has both a synchronous and asynchronous mode
/// of operation. In asynchronous mode, the caller supplies a function callback
/// which will be called when the response to said request is received.
///
/// For synchronous mode, the caller supplies a reference to the data structure
/// that is typically sent to the callback and the function blocks until the
/// response is received or until the timeout specified is reached. Behind the
/// scenes, synchronous calls are calling Pump() until the response for the
/// request is seen. This means that responses to asynchronous requests might
/// be processed during synchronous calls.
///
/// In asynchronous mode, it is the caller's responsibility to periodically
/// trigger response processing. This is typically accomplished by calling
/// Pump() repeatedly.
///
/// The asynchronous callbacks should be able to make function calls on the
/// issuing client. In other words, your callback can invoke another function
/// in an event-driven pattern.
///
/// Streaming operations are by definition asynchronous, so no synchronous
/// API is available for these.
///
/// The class is not thread safe.
class ZIPPYLOG_EXPORT Client {
    public:
        /// Create a client that connects to the specified 0MQ endpoint
        ///
        /// The 0MQ context is required. If not set, an exception will be
        /// thrown.
        Client(::zmq::context_t *ctx, const ::std::string &endpoint);
        ~Client();

        /// Asynchronously send a ping request
        ///
        /// @param callback Function to be called when response received
        /// @param data Arbitrary data to be supplied to callback function
        /// @return Whether request was sent without error
        bool Ping(PingCallback *callback, void *data = NULL);

        /// Synchronously send a ping request
        bool Ping(int32 timeout_microseconds = -1);

        /// Asynchronously obtain the server's features
        ///
        /// @param callback Function to be called when response received
        /// @param data Arbitrary data to be supplied to callback function
        /// @return Whether request was sent without error
        bool GetFeatures(GetFeaturesCallback *callback, void *data = NULL);

        /// Synchronously obtain the server's features
        ///
        /// @param features Populated with server's features on success
        /// @param timeout_microseconds How long to wait for response
        bool GetFeatures(protocol::response::FeatureSpecificationV1 &features, int32 timeout_microseconds = -1);

        /// Asynchronously obtain the store info.
        ///
        /// Executes supplied callback when store info response received.
        ///
        /// Returns true if request was sent without error.
        bool GetStoreInfo(StoreInfoCallback * callback, void *data = NULL);

        /// Synchronously obtain store info
        ///
        /// Will wait up to specified microseconds for store info to be
        /// returned. If we find the store info in the time specified, returns
        /// true. Else, returns false.
        bool GetStoreInfo(protocol::StoreInfoV1 &info, int32 timeout_microseconds = -1);

        /// Asynchronously obtain info about a single bucket
        ///
        /// @param path Path to bucket of which info should be obtained
        /// @param callback Function to be called when response is received
        /// @param data Arbitrary data to be supplied to callback function
        /// @return Whether request was sent without error
        bool GetBucketInfo(const ::std::string &path, BucketInfoCallback *callback, void *data = NULL);

        /// Synchronously obtain info about a single bucket
        ///
        /// @param path Path to obtain info about
        /// @param info Populated with result on successful response
        /// @param timeout_microseconds How long to wait for response
        /// @return Whether we received a successful response
        bool GetBucketInfo(const ::std::string &path, protocol::BucketInfoV1 &info, int32 timeout_microseconds = -1);

        /// Asynchronously obtain info about a single stream set at a path
        ///
        /// @param path Path to stream set we want to obtain info about
        /// @param callback Function to be called when response is received
        /// @param data Arbitrary data to be passed to callback function
        /// @return Whether request was sent without error
        bool GetStreamSetInfo(const ::std::string &path, StreamSetInfoCallback *callback, void *data = NULL);

        /// Synchronously obtain info about a single stream set at a path
        ///
        /// @param path Path to stream set we want to obtain info about
        /// @param info Populated with result on successful response
        /// @param timeout_microseconds How long to wait for a response
        /// @return Whether we received a successful response
        bool GetStreamSetInfo(const ::std::string &path, protocol::StreamSetInfoV1 &info, int32 timeout_microseconds = -1);

        /// Asynchronously obtain stream info.
        ///
        /// Supplied callback will be invoked when stream info response is
        /// received.
        ///
        /// Returns true if request sent without error.
        bool GetStreamInfo(const ::std::string &path, StreamInfoCallback * callback, void * data = NULL);

        /// Synchronously obtain stream info
        ///
        /// Will wait up to specified microseconds for response.
        /// Returns true if info retrieved or false if error or timeout.
        bool GetStreamInfo(const ::std::string &path, protocol::StreamInfoV1 &info, int32 timeout_microseconds = -1);

        bool GetStreamSegment(const ::std::string &path, uint64 start_offset, StreamSegmentCallback * callback, void *data = NULL);
        bool GetStreamSegment(const ::std::string &path, uint64 start_offset, uint64 stop_offset, StreamSegmentCallback * callback, void *data = NULL);
        bool GetStreamSegment(const ::std::string &path, uint64 start_offset, uint32 max_response_bytes, StreamSegmentCallback * callback, void *data = NULL);

        /// Synchronously obtain a stream segment starting from an offset
        bool GetStreamSegment(const ::std::string &path, uint64 start_offset, StreamSegment &segment, int32 timeout = -1);

        /// Synchronously fetch all unfetched parts of a stream
        ///
        /// The function consults the StreamFetchState object passed and
        /// fetches all unfetched segments from the server. For each segment
        /// retrieved, the StreamSegmentCallback callback will be called with
        /// the userdata specified.
        ///
        /// The end_offset parameter can set an end offset ceiling. If 0, the
        /// client will query the remote server for the size of the stream
        /// before fetching. If not 0, this step is skipped.
        ///
        /// The function doesn't return until all stream segments have been
        /// fetched.
        ///
        /// Unlike many other synchronous functions, this one has a callback.
        /// The rationale for the callback is that it is the most flexible way
        /// to handle received stream segments. If we were to pass in a
        /// store writer, for example, we'd be limiting ourselves to what
        /// callers could do with stream segments.
        bool GetStream(const ::std::string &path,
                       StreamFetchState &state,
                       StreamSegmentCallback *callback,
                       void *data = NULL,
                       uint64 end_offset = 0);

        /// Subscribe to store change events
        ///
        /// This subscribes to events that describe the store, not envelopes in
        /// streams. For that, use one of the other Subscribe* functions.
        ///
        /// The first argument is the path in the store to subscribe to. To
        /// subscribe to all paths, set this path to "/".
        ///
        /// The subscription will receive notifications for numerous store
        /// change events. However, unless your SubscriptionCallbackInfo defines
        /// functions for all of them, some events will be dropped by the client.
        bool SubscribeStoreChanges(const ::std::string &path, SubscriptionCallbackInfo &callback, void *data = NULL);

        /// Subscribes to new envelopes written on the server
        bool SubscribeEnvelopes(const ::std::string &path, SubscriptionCallbackInfo &callback, void *data = NULL);

        /// Subscribes to new envelopes w/ Lua code specifying additional features
        bool SubscribeEnvelopes(const ::std::string &path, const ::std::string &lua, SubscriptionCallbackInfo &callback, void *data = NULL);

        /// Cancels the subscription with specified ID
        bool CancelSubscription(const ::std::string &id);

        /// Cancels all subscriptions registered with the client
        bool CancelAllSubscriptions();

        /// Whether the client has a subscription with the specified subscription ID
        bool HasSubscription(const ::std::string &id);

        /// Perform pending operations
        ///
        /// This effectively processes responses from the server.
        ///
        /// Function will wait up to specified microseconds for messages to
        /// become available. -1 is infinite.
        ///
        /// Returns 1 if messages processed, 0 if no messages processed, or -1
        /// on error.
        int Pump(int32 timeout_microseconds);

        /// Renews all subscriptions near expiration
        ///
        /// This sends a subscription keepalive to the server for all
        /// subscriptions about to expire.
        ///
        /// @param force If true, send keepalive to all subscriptions, not
        /// just those about to expire
        bool RenewSubscriptions(bool force=false);

        /// Synchronously mirror the remote server
        ///
        /// The function receives a state object that keeps track of what
        /// stream segments have been received. The initial time this function
        /// is called, it is sufficient to pass an empty state object via its
        /// default constructor.
        ///
        /// When called, this function will query the remote server for
        /// available stream data and will request stream segments for
        /// all unfetched data. For each stream segment returned, the passed
        /// StreamSegmentCallback will be invoked with the user data
        /// passed into the function.
        ///
        /// This function blocks until all stream segments have been fetched.
        ///
        /// Under the hood, this function invokes GetStream() for all remote
        /// streams. This function is provided as a convenience API.
        bool Mirror(StoreMirrorState &state, StreamSegmentCallback *callback, void *data = NULL);

    protected:
        // socket connect to server
        ::zmq::socket_t *client_sock;

        ::std::map< ::std::string, Subscription > subscriptions;
        ::std::map< ::std::string, OutstandingRequest > outstanding;

        ::zmq::pollitem_t pollitem[1];

        uint32 subscription_renewal_offset;

        bool SendRequest(Envelope &e, OutstandingRequest &req);

        /// Sends a synchronous request and wait for a response
        ///
        /// Returns true if the response was processed in the time specified.
        bool SendAndProcessSynchronousRequest(Envelope &e, OutstandingRequest &req, int32 timeout);

        // processe an individual pending zeromq message on the socket
        // this function assumes messages are available
        // pending message could be multipart
        bool ProcessPendingMessage();

        /// Processes received response messages
        bool ProcessResponseMessage(::std::vector< ::zmq::message_t * > &msgs);

        // validates that a received SubscriptionStart message is OK
        // returns false if we don't know how to handle message fields or if
        // we don't know about the subscription
        bool ValidateSubscriptionStart(protocol::response::SubscriptionStartV1 &start);

        // handles a response to a subscription
        bool HandleSubscriptionResponse(Envelope &e, protocol::response::SubscriptionStartV1 &start, ::std::vector< ::zmq::message_t * > &msgs);

        // handles a response to a normal/outstanding request
        bool HandleRequestResponse(Envelope &e, ::std::vector< ::zmq::message_t * > &msgs);

        /// Returns if we have an outstanding request with the specified id
        bool HaveOutstandingRequest(::std::string &id);

        /// Internal callback used for synchronous ping requests
        static void CallbackPing(Client *client, void *data);

        /// Internal callback used for synchronous features requests
        static void CallbackFeatures(Client *client, protocol::response::FeatureSpecificationV1 &features, void *data);

        /// Internal callback used for synchronous store info requests
        static void CallbackStoreInfo(Client *client, protocol::StoreInfoV1 &info, void *data);

        /// Internal callback used for synchronous stream info requests
        static void CallbackStreamInfo(Client *client, protocol::StreamInfoV1 &info, void *data);

        static void CallbackStreamSegment(Client *client, const ::std::string &path, uint64 start_offset, StreamSegment &segment, void *data);

    private:
        // disable copy constructor and assignment operator
        Client(const Client &orig);
        Client & operator=(const Client &orig);
};

}} // namespaces

#endif
