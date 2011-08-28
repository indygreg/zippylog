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
class StreamSegment;
class StreamFetchState;

/// Represents a client subscription
///
/// Used internally by the client to keep track of state.
///
/// @internal
class Subscription;

/// Keeps track of requests sent whose replies have not yet been seen
///
/// @internal
class OutstandingRequest;

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

/// Callback for subscription acknowledgement
typedef void (SubscriptionRequestCallback)(Client *, protocol::response::SubscriptionAcceptAckV1 &, void *);

/// Executed when a stream segment is received
///
/// Invoked with the path, start offset, and the data in that segment
typedef void (StreamSegmentCallback)(Client *, ::std::string const &, uint64, StreamSegment &, void *);

/// Function types for callbacks when the client has received a subscription
/// response
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

/// Executed when an envelope is received
///
/// Invoked with subscription id, the envelope, and supplied user data to subscription
typedef void (EnvelopeCallback)(Client *, ::std::string const &, Envelope &, void *);

/// Represents a segment of a stream
///
/// Stream segments have begin and end offsets and contain envelopes.
/// Segments are returned from requests to obtain parts of streams.
///
/// @todo formalize API
class ZIPPYLOG_EXPORT StreamSegment {
    public:
        /// Construct a new segment instance
        StreamSegment();
        ~StreamSegment();

        /// Set the path the segment is from
        bool SetPath(const ::std::string path);

        /// Set the start offset of this segment
        bool SetStartOffset(uint64 offset);

        /// Set the end offset of this segment
        bool SetEndOffset(uint64 offset);

        /// Set number of bytes sent in this segment
        bool SetBytesSent(uint32 count);

        /// Set number of envelopes sent in this segment
        bool SetEnvelopesSent(uint32 number);

        /// Add an envelope to the segment
        bool AddEnvelope(Envelope e);

        /// Set fields in the stream segment from values in another instance
        bool CopyFrom(StreamSegment const &orig);

        /// Path that the segment belongs to
        ::std::string Path;

        /// The stream offset this segment started at
        uint64 StartOffset;

        /// The stream offset this segment ended at
        ///
        /// The next segment should begin at this value + 1.
        uint64 EndOffset;

        /// How many bytes are in the segment
        uint32 BytesSent;

        /// How many envelopes were sent in the segment
        uint32 EnvelopesSent;

        /// The envelopes sent in the segment
        ::std::vector<Envelope> Envelopes;
};

/// Records what parts of a stream have been fetched
class ZIPPYLOG_EXPORT StreamFetchState {
public:
    /// Construct a new state instance
    StreamFetchState() : end_offset(0) { }

    /// The last offset we have fetched
    uint64 end_offset;
};

/// Records the state of stream fetches for multiple streams
class ZIPPYLOG_EXPORT StoreMirrorState {
public:
    /// Construct a new state instance
    StoreMirrorState() { }

    /// Set the state of a stream to the record specified
    ///
    /// @param path Path of stream
    /// @param state Fetch state record
    void SetStreamState(::std::string const &path, StreamFetchState state);

    /// Update the stream end offset for a specific path
    ///
    /// If a record does not exist for the specified stream, it will be
    /// created automatically.
    ///
    /// @param path Path of stream to modify
    /// @param offset Offset to set to
    void SetStreamEndOffset(::std::string const &path, uint64 offset);

    friend class Client;
protected:

    /// Maps path to stream fetch state
    ::std::map< ::std::string, StreamFetchState > states;
};

/// Defines the set of function callbacks for a subscription.
///
/// Not all callback types are valid for every subscription type. If a
/// callback in not defined, no function is executed when an event for that
/// callback is received.
class ZIPPYLOG_EXPORT SubscriptionCallbackInfo {
public:
    SubscriptionCallbackInfo() :
        data(NULL),
        StreamAdded(NULL),
        StreamDeleted(NULL),
        StreamAppended(NULL),
        BucketAdded(NULL),
        BucketDeleted(NULL),
        StreamSetAdded(NULL),
        StreamSetDeleted(NULL),
        EnvelopeReceived(NULL)
    { }

    /// Arbitrary data to be passed to callbacks
    void * data;

    /// Callback for when a stream is added
    StoreChangeStreamAddedCallback *      StreamAdded;

    /// Callback for when a stream is deleted
    StoreChangeStreamDeletedCallback *    StreamDeleted;

    /// Callback for when a stream is appended
    StoreChangeStreamAppendedCallback *   StreamAppended;

    /// Callback for when a bucket is added
    StoreChangeBucketAddedCallback *      BucketAdded;

    /// Callback for when a bucket is deleted
    StoreChangeBucketDeletedCallback *    BucketDeleted;

    /// Callback for when a stream set is added
    StoreChangeStreamSetAddedCallback *   StreamSetAdded;

    /// Callback for when a stream set is deleted
    StoreChangeStreamSetDeletedCallback * StreamSetDeleted;

    /// Callback for when an envelope is received
    EnvelopeCallback *                    EnvelopeReceived;
};

/// Represents the result of a subscription request
class ZIPPYLOG_EXPORT SubscriptionRequestResult {
public:
    /// Create a new result instance
    SubscriptionRequestResult() :
        result(UNKNOWN),
        ttl(0)
    { }

    /// Types of results from subscription requests
    enum Result {
        /// Subscription was accepted
        ACCEPTED = 1,

        /// Subscription was rejected
        REJECTED = 2,

        /// Unknown result
        UNKNOWN  = 3,
    };

    /// Result of the subscription request
    Result result;

    /// Subscription id
    ::std::string id;

    /// Time-to-live of subscription
    uint32 ttl;

    /// Error code
    ///
    /// Only populated if the subscription was rejected
    ::zippylog::protocol::response::ErrorCode error_code;

    /// Error message
    ///
    /// Only populated if the subscription was rejected
    ::std::string error_message;
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
///
/// @todo Don't swallow protocol errors behind API
class ZIPPYLOG_EXPORT Client {
    public:
        /// Create a client that connects to the specified 0MQ endpoint
        ///
        /// The 0MQ context is required. If not set, an exception will be
        /// thrown.
        Client(::zmq::context_t *ctx, ::std::string const &endpoint);
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
        bool GetBucketInfo(::std::string const &path, BucketInfoCallback *callback, void *data = NULL);

        /// Synchronously obtain info about a single bucket
        ///
        /// @param path Path to obtain info about
        /// @param info Populated with result on successful response
        /// @param timeout_microseconds How long to wait for response
        /// @return Whether we received a successful response
        bool GetBucketInfo(::std::string const &path, protocol::BucketInfoV1 &info, int32 timeout_microseconds = -1);

        /// Asynchronously obtain info about a single stream set at a path
        ///
        /// @param path Path to stream set we want to obtain info about
        /// @param callback Function to be called when response is received
        /// @param data Arbitrary data to be passed to callback function
        /// @return Whether request was sent without error
        bool GetStreamSetInfo(::std::string const &path, StreamSetInfoCallback *callback, void *data = NULL);

        /// Synchronously obtain info about a single stream set at a path
        ///
        /// @param path Path to stream set we want to obtain info about
        /// @param info Populated with result on successful response
        /// @param timeout_microseconds How long to wait for a response
        /// @return Whether we received a successful response
        bool GetStreamSetInfo(::std::string const &path, protocol::StreamSetInfoV1 &info, int32 timeout_microseconds = -1);

        /// Asynchronously obtain stream info.
        ///
        /// Supplied callback will be invoked when stream info response is
        /// received.
        ///
        /// Returns true if request sent without error.
        bool GetStreamInfo(::std::string const &path, StreamInfoCallback * callback, void * data = NULL);

        /// Synchronously obtain stream info
        ///
        /// Will wait up to specified microseconds for response.
        /// Returns true if info retrieved or false if error or timeout.
        bool GetStreamInfo(::std::string const &path, protocol::StreamInfoV1 &info, int32 timeout_microseconds = -1);

        /// Fetch a single stream segment asynchronously
        ///
        /// @param path The stream path to fetch
        /// @param start_offset The start offset to start fetching at
        /// @param callback The function to call when the response is received
        /// @param data Arbitrary data to be supplied to callback function
        /// @return Whether the request was issued without error
        bool GetStreamSegment(::std::string const &path, uint64 start_offset, StreamSegmentCallback * callback, void *data = NULL);

        /// Fetch a single stream segment with offset boundaries asynchronously
        ///
        /// @param path The stream path to fetch
        /// @param start_offset The offset to start fetching from
        /// @param stop_offset The offset to stop fetching at
        /// @param callback The function to call when the response is received
        /// @param data Arbitrary data to be supplied to the callback function
        /// @return Whether the request was issued without error
        bool GetStreamSegment(::std::string const &path, uint64 start_offset, uint64 stop_offset, StreamSegmentCallback * callback, void *data = NULL);

        /// Asynchornously fetch a single stream segment with size limitations
        ///
        /// @todo varying by uint32 and uint64 is pretty stupid
        ///
        /// @param path The stream path to fetch
        /// @param start_offset The offset to start fetching from
        /// @param max_response_bytes Maximum number of bytes to fetch
        /// @param callback The function to call when the response is received
        /// @param data Arbitrary data to be supplied to the callback function
        /// @return Whether the request was issued without error
        bool GetStreamSegment(::std::string const &path, uint64 start_offset, uint32 max_response_bytes, StreamSegmentCallback * callback, void *data = NULL);

        /// Synchronously obtain a stream segment starting from an offset
        ///
        /// @param path The stream path to fetch
        /// @param start_offset The offset from which to start fetching
        /// @param segment Holds result upon successful completion
        /// @param timeout How long to wait for a response, in microseconds
        /// @return Whether the response was received without error
        bool GetStreamSegment(::std::string const &path, uint64 start_offset, StreamSegment &segment, int32 timeout = -1);

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
        ///
        /// @param path The stream path to fetch
        /// @param state The state of the fetch so far
        /// @param callback Function to be called when response is received
        /// @param data Arbitrary data to be passed to callback
        /// @param end_offset Max offset to fetch
        /// @return Whether the request was issued without error
        bool GetStream(::std::string const &path,
                       StreamFetchState &state,
                       StreamSegmentCallback *callback,
                       void *data = NULL,
                       uint64 end_offset = 0);

        /// Synchronously subscribe to store changes
        ///
        /// This subscribes to events that describe changes to the store, such
        /// as the creation and deletion of buckets, stream sets, and streams
        /// as well as when a stream is modified.
        ///
        /// The first argument is the set of paths in the store to subscribe
        /// to. If no paths are defined, the entire store is subscribed to.
        /// (See also the version of this function that doesn't take a path
        /// parameter.). Paths can be to streams, stream sets, buckets, or
        /// the entire store ("/").
        ///
        /// The function receives a class instance that configures the
        /// callbacks executed when an event of interest is received.
        ///
        /// The function blocks until the server responds to the subscription
        /// request. At that time, the result of the subscription will be
        /// populated in the SubscriptionRequestResult parameter.
        ///
        /// @param paths Paths to subscribe to
        /// @param callbacks Configures which callbacks are in effect
        /// @param result Holds the result of the subscription request
        /// @param timeout_microseconds How long to wait for server to respond
        /// @return Whether a response from the server was received in time
        bool SubscribeStoreChanges(::std::vector< ::std::string >  const &paths,
                                   SubscriptionCallbackInfo &callbacks,
                                   SubscriptionRequestResult &result,
                                   int32 timeout_microseconds = -1);

        /// Synchronously subscribe to store changes
        ///
        ///
        /// The first argument is the path in the store to subscribe to. To
        /// subscribe to all paths, set this path to "/".
        ///
        /// The subscription will receive notifications for numerous store
        /// change events. However, unless your SubscriptionCallbackInfo defines
        /// functions for all of them, some events will be dropped by the client.
        ///
        /// @param path Store path to subscribe to
        /// @param callbacks Defines callbacks to handle subscribed events
        /// @param result Stores the result of the subscription
        /// @param timeout_microseconds How long to wait for a server response
        bool SubscribeStoreChanges(::std::string const &path,
                                   SubscriptionCallbackInfo &callbacks,
                                   SubscriptionRequestResult &result,
                                   int32 timeout_microseconds = -1);

        /// Synchronously subscribes to store changes for the entire store
        ///
        /// @param callback Callback configuration
        /// @param result Stores result of the subscription
        /// @param data Data to be passed to callback functions
        bool SubscribeStoreChanges(SubscriptionCallbackInfo &callbacks,
                                   SubscriptionRequestResult &result,
                                   int32 timeout_microseconds);

        /// Subscribes to new envelopes written on the server
        bool SubscribeEnvelopes(::std::string const &path, SubscriptionCallbackInfo &callback, void *data = NULL);

        /// Subscribes to new envelopes w/ Lua code specifying additional features
        bool SubscribeEnvelopes(::std::string const &path, ::std::string const &lua, SubscriptionCallbackInfo &callback, void *data = NULL);

        /// Cancels the subscription with the specified ID
        ///
        /// @param id Subscription id to cancel
        bool CancelSubscription(::std::string const &id);

        /// Cancels all subscriptions registered with the client
        bool CancelAllSubscriptions();

        /// Whether the client has a subscription with the specified subscription ID
        bool HasSubscription(::std::string const &id);

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

        /// Runs the client continuously
        ///
        /// The client will run, processing received messages until the
        /// flag pointed to by the passed parameter goes to false.
        ///
        /// This function won't return until the flag goes to false.
        void Run(bool *active);

        /// Runs the client in the background
        ///
        /// This is a non-blocking version of Run(). A new thread is started
        /// and the client runs on that.
        ///
        /// Keep in mind that the class is not thread safe, so other methods
        /// should not be called when the client is running in the background.
        void RunAsync(bool *active);

    protected:
        /// Socket connected to server
        ::zmq::socket_t *client_sock;

        /// Mapping of subscription ID to subscription state
        ::std::map< ::std::string, Subscription > subscriptions;

        /// Mapping of request ID to request metadata
        ::std::map< ::std::string, OutstandingRequest > outstanding;

        /// 0MQ poll structure
        ::zmq::pollitem_t pollitem[1];

        /// How long before expiration that we should renew subscriptions
        uint32 subscription_renewal_offset;

        /// Client's execution thread
        ::zippylog::platform::Thread * exec_thread;

        /// Semaphore to determine if client should continue running
        bool *run_flag;

        /// Sends a request to the server
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

        /// validates that a received SubscriptionStart message is OK
        /// returns false if we don't know how to handle message fields or if
        /// we don't know about the subscription
        bool ValidateSubscriptionStart(protocol::response::SubscriptionStartV1 &start);

        /// handles a response to a subscription
        bool HandleSubscriptionResponse(Envelope &e, protocol::response::SubscriptionStartV1 &start, ::std::vector< ::zmq::message_t * > &msgs);

        /// Handles a response to a normal/outstanding request
        bool HandleRequestResponse(Envelope &e, ::std::vector< ::zmq::message_t * > &msgs);

        /// Returns if we have an outstanding request with the specified id
        bool HaveOutstandingRequest(::std::string &id);

        /// Internal callback used for synchronous ping requests
        static void CallbackPing(Client *client, void *data);

        /// Internal callback used for synchronous features requests
        static void CallbackFeatures(Client *client, protocol::response::FeatureSpecificationV1 &features, void *data);

        /// Internal callback used for synchronous store info requests
        static void CallbackStoreInfo(Client *client, protocol::StoreInfoV1 &info, void *data);

        /// Internal callback used for synchronous bucket info requests
        static void CallbackBucketInfo(Client *client, protocol::BucketInfoV1 &info, void *data);

        /// Internal callback used for synchronous stream set info requests
        static void CallbackStreamSetInfo(Client *client, protocol::StreamSetInfoV1 &info, void *data);

        /// Internal callback used for synchronous stream info requests
        static void CallbackStreamInfo(Client *client, protocol::StreamInfoV1 &info, void *data);

        /// Internal callback used for synchronous stream segment requests
        static void CallbackStreamSegment(Client *client, ::std::string const &path, uint64 start_offset, StreamSegment &segment, void *data);

        /// Internal callback used for synchronous subscription requests
        static void CallbackSubscription(Client *client, protocol::response::SubscriptionAcceptAckV1 &result, void *data);

        /// Thread start function for RunAsync()
        static void * AsyncStart(void *data);

    private:
        // disable copy constructor and assignment operator
        Client(Client const &orig);
        Client & operator=(Client const &orig);
};

}} // namespaces

#endif
