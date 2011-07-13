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

#ifndef ZIPPYLOG_REQUEST_PROCESSOR_HPP_
#define ZIPPYLOG_REQUEST_PROCESSOR_HPP_

#include <zippylog/zippylog.hpp>

#include <zippylog/lua.hpp>
#include <zippylog/store.hpp>
#include <zippylog/protocol/request.pb.h>
#include <zippylog/protocol/response.pb.h>

#include <zmq.hpp>

namespace zippylog {

/// Records information unique to an envelope subscription
class EnvelopeSubscription {
public:
    EnvelopeSubscription();

    ::std::string id;
    ::std::vector< ::std::string > paths;
    ::std::vector< ::std::string > socket_identifiers;

    ::std::vector<uint32> filter_namespaces;
    ::std::vector< ::std::pair<uint32, uint32> > filter_enumerations;
};

/// Records information about a single subscription
class SubscriptionInfo {
public:
    SubscriptionInfo();
    SubscriptionInfo(uint32 expiration_ttl);
    ~SubscriptionInfo();

    void InitializeLua();

    /// Timer that keeps track of subscription expiration
    ::zippylog::platform::Timer expiration_timer;

    /// The type of subscription
    enum SubscriptionType {
        ENVELOPE = 1,
        STORE_CHANGE = 2,
    } type;

    /// Subscription id
    ::std::string id;

    /// Store paths subscribed to
    ::std::vector< ::std::string > paths;

    /// 0MQ socket identifiers to route messages
    ::std::vector< ::std::string > socket_identifiers;

    /// Details about envelope subscription
    EnvelopeSubscription envelope_subscription;

    /// Lua state attached to subscription
    ::zippylog::lua::LuaState *l;

private:
    SubscriptionInfo(const SubscriptionInfo &orig);
    SubscriptionInfo & operator=(const SubscriptionInfo &orig);
};

/// Encapsulates the result of a request to handle a subscription
class ZIPPYLOG_EXPORT HandleSubscriptionResult {
public:
    HandleSubscriptionResult() : result(UNKNOWN) { }

    enum SubscriptionResult {
        ACCEPTED,
        REJECTED,
        UNKNOWN
    } result;

    ::std::string id;

    ::std::string reject_reason;
};

/// Used to construct a request processor
class ZIPPYLOG_EXPORT RequestProcessorStartParams {
public:
    RequestProcessorStartParams() :
        ctx(NULL),
        active(NULL),
        get_stream_max_bytes(256000),
        get_stream_max_envelopes(10000),
        lua_memory_max(65535),
        subscription_ttl(60000)
    { }

    /// The path to the store we should operate against
    ///
    /// A new store instance is created within the processor instance.
    ::std::string store_path;

    /// 0MQ context to use
    ///
    /// must not be NULL
    ::zmq::context_t *ctx;

    /// 0MQ endpoint to which to bind a XREP socket to receive client requests
    ::std::string client_endpoint;

    /// 0MQ endpoint to which to connect to send log envelopes
    ::std::string logger_endpoint;

    /// Semaphore indicating whether the processor should remain alive
    ///
    /// When this goes to false, the request processor will cease processing
    /// new messages and will return from its run routine.
    bool *active;

    /// Maximum number of bytes that can be returned from a GetStream request
    uint32 get_stream_max_bytes;

    /// Maximum number of envelopes that can be returned from a GetStream request
    uint32 get_stream_max_envelopes;

    /// Mamimum amount of memory (in kilobytes) a Lua state can allocate
    uint32 lua_memory_max;

    /// Subscription time-to-live in seconds
    uint32 subscription_ttl;
};

/// Processes zippylog protocol requests
///
/// This class is designed to be an abstract base class. It implements core
/// functionality for parsing and verifying protocol requests. However, the
/// behavior controlling what happens when specific requests are received is
/// abstracted away in the various Handle* functions. Derived classes should
/// implement these functions.
///
/// In other words, this class can be thought of as the library that handles
/// zippylog protocol processing. The library can be utilized to provide
/// server capabilities.
///
/// Ideally, all code interacting with the zippylog protocol messages is
/// limited to this class. If done this way, all protocol debugging can occur
/// within one file. If you find yourself touching protocol messages
/// elsewhere, you are likely doing it wrong.
///
/// Currently, we make the assumption that protocol requests arrive via
/// 0MQ sockets. Strictly speaking, this isn't very loosely coupled. However,
/// the various Process* functions don't expose 0MQ details, so it should be
/// possible to call into this class without serializing envelopes as 0MQ
/// messages. That being said, the constructor still requires the 0MQ
/// parameters (for now, at least).
class ZIPPYLOG_EXPORT RequestProcessor {
    public:
        // return code from the various request processors
        enum ResponseStatus {
            // processor is authoritative responder for this request
            // the Envelopes/messages set in a passed vector should be
            // sent to the client
            AUTHORITATIVE = 1,

            // processor deferred to send a response
            // caller should not send any response to client, as this will be
            // done by some other process
            DEFERRED = 2,
        };

        RequestProcessor(RequestProcessorStartParams &params);
        virtual ~RequestProcessor();

        /// Runs the request processor
        /// Will listen for messages on the 0MQ socket specified in the start parameters
        /// This function executes forever in an infinite loop until a
        /// catastrophic error or the active parameter passed in the start
        /// parameters goes to false
        void Run();

        /// Perform processing activities
        ///
        /// Reads available client messages from the listening socket(s) and
        /// processes them.
        ///
        /// If no messages are available, waits up to the specified number of
        /// microseconds and process any that arrive before that time window
        /// expires.
        int Pump(long wait_microseconds);

        /// Processes received 0MQ messages
        ///
        /// The identities parameter is a list of 0MQ socket identities. These
        /// are used to craft the appropriate multipart 0MQ response message.
        /// The input parameter is a list of received 0MQ messages that should
        /// contain a request of some type. The output parameter will receive
        /// the list of 0MQ messages that would have been sent back to the
        /// client immediately.
        ///
        /// The request processor is not always authoritative for responses.
        /// For some request types, control is handed off to another processor
        /// for additional processing. In these scenarios, the output messages
        /// list might be empty. It all depends on the implementation.
        ///
        /// This function is called by Run() whenever it has received messages
        /// for processing. The preferred way to pass messages to the request
        /// processor is by sending them over the socket. However, this
        /// function is public just in case.
        void ProcessMessages(::std::vector< ::std::string > &identities, ::std::vector< ::zmq::message_t * > &input, ::std::vector<Envelope> &output);

        /// Processes a request envelope
        ///
        /// This is the main request processing function. If there are
        /// envelopes to be sent to the client, they are added to the passed
        /// vector.
        ///
        /// Callers should inspect the return value to determine how to handle
        /// response messages.
        ///
        /// Most people typically have no need to call this function. However,
        /// it is provided public just in case.
        ResponseStatus ProcessRequest(Envelope &e, ::std::vector<Envelope> &output);

        /// Sends a subscription store change response through a socket
        ///
        /// @param sock Socket to send response through
        /// @param subscription Subscription record used to populate metadata
        /// @param e Envelope containing store change message
        static bool SendSubscriptionStoreChangeResponse(::zmq::socket_t &sock,
                                                        const SubscriptionInfo &subscription,
                                                        const Envelope &e);


    protected:
        /// Callback to handle a validated request for a subscription
        ///
        /// This function is called after a subscription request has been
        /// received and validated. The function receives a pointer to a
        /// subscription record. Ownership of this object is transferred to
        /// the called function, which is expected to delete it when it is
        /// no longer used.
        ///
        /// @param subscription metadata
        virtual HandleSubscriptionResult HandleSubscriptionRequest(SubscriptionInfo *subscription) = 0;

        /// callback to handle a subscription keepalive
        virtual ResponseStatus HandleSubscribeKeepalive(Envelope &request, ::std::vector<Envelope> &output) = 0;

        /// Callback to handle writing of envelopes
        ///
        /// Receives the path we are writing to (already validated to be a
        /// stream set or stream). If a stream set, it is validated to exist.
        /// If a stream, it may not exist.
        ///
        /// The synchronous parameter says whether to wait for writes before
        /// returning.
        ///
        /// Returns the number of envelopes written or -1 on failure. If not
        /// performing synchronous writes, it is OK to return 0. However, if
        /// performing synchronous writes the caller will log an unexpected
        /// condition if the number of envelopes written is not equal to the
        /// number requested.
        virtual int HandleWriteEnvelopes(const ::std::string &path, ::std::vector<Envelope> &to_write, bool synchronous) = 0;

        /// Process a ping request
        ResponseStatus ProcessPing(Envelope &request, ::std::vector<Envelope> &output);

        /// Process a GetFeatures request
        ResponseStatus ProcessFeatures(Envelope &request, ::std::vector<Envelope> &output);

        /// Process a GetStoreInfo request and populate the passed envelope with the response
        ///
        /// This function is typically called only by ProcessRequest()
        ResponseStatus ProcessStoreInfo(Envelope &request, ::std::vector<Envelope> &output);

        ResponseStatus ProcessBucketInfo(Envelope &request, ::std::vector<Envelope> &output);
        ResponseStatus ProcessStreamSetInfo(Envelope &request, ::std::vector<Envelope> &output);
        ResponseStatus ProcessStreamInfo(Envelope &request, ::std::vector<Envelope> &output);

        /// Process a GetStream request
        ResponseStatus ProcessGetStream(Envelope &request, ::std::vector<Envelope> &output);

        ResponseStatus ProcessSubscribeStoreChanges(Envelope &request, ::std::vector<Envelope> &output);

        ResponseStatus ProcessSubscribeEnvelopes(Envelope &request, ::std::vector<Envelope> &output);

        ResponseStatus ProcessSubscribeKeepalive(Envelope &request, ::std::vector<Envelope> &output);

        /// Process a WriteEnvelope request
        ResponseStatus ProcessWriteEnvelope(Envelope &request, ::std::vector<Envelope> &output);

        void CallHandleSubscriptionRequest(SubscriptionInfo **subscription, ::std::vector<Envelope> &output);

        /// Checks that a path supplied by the client is valid
        ///
        /// Returns false if:
        ///
        ///  - the path is not a valid path
        ///  - the require_ values are true and that path component doesn't
        ///    exist in the path or on the store
        ///
        /// If the path does not validate, an error response is added to the
        /// output messages with an appropriate description of the failure.
        bool CheckPath(const ::std::string &path, ::std::vector<Envelope> &output, bool require_bucket = false, bool require_set = false, bool require_stream = false);

        /// Checks that the version in a message is THE version we support
        ///
        /// If it isn't, an error response is added to the output messages.
        bool CheckMessageVersion(uint32 seen_version, uint32 supported_version, ::std::vector<Envelope> &output);

        bool PopulateErrorResponse(::zippylog::protocol::response::ErrorCode code, ::std::string message, ::std::vector<Envelope> &msgs);

        ::zmq::context_t *ctx;
        ::std::string store_path;
        ::std::string logger_endpoint;
        ::std::string client_endpoint;

        ::zmq::socket_t * logger_sock;
        ::zmq::socket_t * socket;

        Store * store;

        ::std::string id;
        ::std::vector< ::std::string > current_request_identities;
        bool *active;

        /// Maximum number of bytes we're allowed to return per GetStreamSegment request
        uint32 get_stream_max_bytes;

        /// Maximum number of enevelopes we can return per GetStreamSegment request
        uint32 get_stream_max_envelopes;

        /// Maximum size (in kilobytes) a Lua state can allocate
        uint32 lua_memory_max;

        /// Subscription time-to-live in milliseconds
        uint32 subscription_ttl;

private:
        RequestProcessor(const RequestProcessor &orig);
        RequestProcessor & operator=(const RequestProcessor &orig);

};

} // namespace

#endif