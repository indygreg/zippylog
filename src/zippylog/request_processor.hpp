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
#include <zippylog/device/device.hpp>
#include <zippylog/device/server.pb.h>
#include <zippylog/zeromq.hpp>

#include <zmq.hpp>

namespace zippylog {

/// Holds metadata necessary for crafting responses to requests
///
/// This class is used primarily as an aid for asynchronous response
/// generation. An instance of this class is passed to the static protocol
/// response generators in RequestProcessor to craft appropriate responses.
///
/// The class is designed as a black box. RequestProcessor is the only thing
/// that is interested in what's inside.
class ZIPPYLOG_EXPORT ResponseMetadata {
protected:
    /// Construct an instance of the metadata class
    ///
    /// Marked as non-public because only the RequestProcessor should be
    /// instantiating it.
    ResponseMetadata(Envelope const & e,
                     ::std::vector< ::std::string > &identities);

    /// Socket identities to route response
    ::std::vector< ::std::string > identities;

    /// Tags to put on the response envelope
    ::std::vector< ::std::string > tags;

    friend class RequestProcessor;
    friend class PluginRegistrationRequest;

private:
    /// Default constructor disabled
    ResponseMetadata() {}
};

/// Defines the type of a subscription
typedef enum {
    ENVELOPE = 1,
    STORE_CHANGE = 2,
    UNKNOWN = 3,
} SubscriptionType;

/// Details about a single subscription
///
/// These records are populated by the request processor. They are typically
/// passed along to a persisted state manager.
///
/// Even though the members are public, you likely won't be mucking with them
/// unless you are part of the core zippylog library.
///
/// @todo consider formalizing API
class ZIPPYLOG_EXPORT SubscriptionInfo {
public:
    SubscriptionInfo() : type(UNKNOWN)
    { };

    /// Creates an instance from a protocol buffer representation
    SubscriptionInfo(device::server::SubscriptionRecord const &m);

    /// Copies data from this instance to a protocol buffer message
    void ToProtocolBuffer(device::server::SubscriptionRecord &m);

    /// The type of subscription
    SubscriptionType type;

    /// Subscription id
    ::std::string id;

    /// Store paths subscribed to
    ::std::vector< ::std::string > paths;

    /// Lua code
    ::std::string lua_code;

    /// 0MQ socket identifiers that original subscription arrived on
    ///
    /// Used for return path routing
    ::std::vector< ::std::string > socket_identifiers;

    /// The namespaces we are interested in, for envelope subscriptions
    ::std::vector<uint32> envelope_filter_namespaces;

    /// The enumerations we are in, for envelope subscriptions
    ::std::vector< ::std::pair<uint32, uint32> > envelope_filter_enumerations;
};

/// Maintains state for a subscription response stream
///
/// See also RequestProcessor::SendSubscriptionEnvelopeResponse()
class ZIPPYLOG_EXPORT EnvelopeSubscriptionResponseState {
public:
    /// Construct a state for sending an envelope subscription response
    ///
    /// @param subscription Subscription record
    EnvelopeSubscriptionResponseState(SubscriptionInfo const &subscription) :
        finalized(false),
        id(subscription.id),
        identities(subscription.socket_identifiers),
        current_size(0)
    { }

    ~EnvelopeSubscriptionResponseState();

    /// Adds an envelope to the state
    ///
    /// This effectively marks the envelope as ready to send to the client
    void AddEnvelope(Envelope const &e);

    /// State that the final envelope has been seen
    inline void Finalize() { this->finalized = true; }

    /// Registers an error
    void RegisterError(protocol::response::ErrorCode code, ::std::string const &message);

protected:
    /// Whether the final envelope has been seen
    bool finalized;

    /// Subscription id
    ::std::string id;

    /// 0MQ socket identifiers associated with return path
    ::std::vector< ::std::string > identities;

    /// The current size of all 0MQ messages in the set
    uint32 current_size;

    /// Container for 0MQ messages
    ::std::vector< ::zmq::message_t * > messages;

    ///@{
    /// Limit constants
    ///
    /// @todo these should come from elsewhere
    static const uint32 max_size = 1048576;
    static const uint32 max_envelopes = 1024;
    ///@}

    friend class RequestProcessor;
};

/// Represents a request to register a plugin
///
/// Plugin registration requests are normalized to this type and delivered
/// to the registration callback
class ZIPPYLOG_EXPORT PluginRegistrationRequest {
public:
    /// Construct a new, empty record
    PluginRegistrationRequest() { }

    /// Name plugin should be registered under
    ::std::string name;

    /// Lua code constituting the plugin
    ::std::string lua_code;

    /// 0MQ socket identities for response routing
    ::std::vector< ::std::string > identities;

    /// Metadata to help generate response
    ResponseMetadata md;
};

/// Return code from request processors routine
enum RequestProcessorResponseStatus {
    /// Processor is authoritative responder for this request.
    ///
    /// The Envelopes/messages set in a passed vector should be
    /// sent to the client
    AUTHORITATIVE = 1,

    /// Processor deferred/declined to send a response
    ///
    /// Caller should not send any response to client, as this will be
    /// done by some other process
    DEFERRED = 2,
};

/// This class represents the result of a request processor handler invocation
///
/// Instances of this class are returned by the various handler functions in
/// RequestProcessorImplementation classes.
class ZIPPYLOG_EXPORT RequestProcessorHandlerResult {
public:
    /// Construct a response that indicates we processed the request
    /// successfully
    static RequestProcessorHandlerResult MakeSynchronous();

    /// Construct a response that indicates an error was encountered
    static RequestProcessorHandlerResult MakeError(
        ::zippylog::protocol::response::ErrorCode code,
        ::std::string const & message);

    /// Construct a response that indicates deferred processing
    static RequestProcessorHandlerResult MakeDeferred();

    /// Construct a response for a synchronous result to a plugin status query
    ///
    /// The plugin states are returned to the request processor and sent as
    /// part of the response.
    static RequestProcessorHandlerResult MakePluginStatus(
        ::std::vector< ::zippylog::protocol::PluginStateV1 > &states);

    /// Construct a response that indicates synchronous handling of an
    /// accepted subscription.
    ///
    /// @param id ID of created subscription
    /// @param ttl TTL of new subscription
    static RequestProcessorHandlerResult MakeSubscriptionAccepted(
        ::std::string const &id,
        uint32 ttl);

    /// Construct a response that indicates sychronous handling of a rejected
    /// subscription.
    ///
    /// @param reason Why the subscription was rejected
    static RequestProcessorHandlerResult MakeSubscriptionRejected(
        ::std::string const &reason);

    /// Construct a response that indicates synchronous handling of a write
    /// request.
    ///
    /// @param written Number of envelopes that were written
    static RequestProcessorHandlerResult MakeWriteResult(uint32 written);

protected:
    /// Whether the response is deferred
    ///
    /// If true, do not send a response ourselves
    bool deferred;

    /// Whether an error was registered
    bool have_error;

    /// Error code
    ::zippylog::protocol::response::ErrorCode error_code;

    /// Error message
    ::std::string error_message;

    /// Indicates the response was related to subscriptions
    bool is_subscription;

    /// Indicates whether the response is related to envelope writing
    bool is_write;

    /// Id of accepted subscription
    ::std::string subscription_id;

    /// TTL of accepted subscription
    uint32 subscription_ttl;

    /// Number of envelopes written
    uint32 envelopes_written;

    friend class RequestProcessor;

private:
    /// Default constructor is disabled
    RequestProcessorHandlerResult();
};

/// Abstract base class that provides functionality for a RequestProcessor
///
/// RequestProcessor instances are associated with instances of classes
/// derived from this base class. During key events, the RequestProcessor
/// calls out to handlers in this class. The implementation takes care of
/// the details then tells the request processor what's going on.
///
/// Callbacks in this class generally have the option of servicing the request
/// synchronously or asynchronously. In other words, they can either perform
/// the operation immediately, while blocking the request processor from
/// sending a response. Or, they can defer immediate processing and tell the
/// request processor that another entity will take care of generating the
/// response.
///
/// In the case of asynchronous servicing, whatever eventually processes the
/// request will likely make a call to one of the static Send* functions on
/// RequestProcessor to generate a response and send that to the client. The
/// documentation for each handler should document the expected behavior.
///
/// It is a best practice to only synchronously perform actions if they can be
/// done quickly. This way, the request processor thread won't be blocked from
/// servicing additional requests. Of course, if the request processor and
/// whatever is eventually servicing the request are on the same thread, it
/// doesn't make sense to use asynchronous servicing.
class ZIPPYLOG_EXPORT RequestProcessorImplementation {
public:
    RequestProcessorImplementation() {}
    virtual ~RequestProcessorImplementation() {};

    /// Handler to process a new subscription request
    ///
    /// This function is called after a subscription request has been
    /// received and validated. The handler receives a record that describes
    /// the subscription being desired.
    ///
    /// This handler has the option of servicing synchronously or
    /// asynchronously. In the case of synchronous operation, it should call
    /// RequestProcessorHandlerResult::MakeSubscriptionAccepted() or
    /// RequestProcessorHandlerResult::MakeSubscriptionRejected(). For
    /// asynchronous completion, it should return
    /// RequestProcessorHandlerResult::MakeDeferred() and the eventual handler
    /// should call RequestProcessor::SendSubscriptionAccepted().
    ///
    /// @param subscription Describes the subscription being made
    /// @return How the request was handled
    virtual RequestProcessorHandlerResult HandleSubscriptionRequest(
        SubscriptionInfo subscription) = 0;

    /// callback to handle a subscription keepalive
    virtual RequestProcessorHandlerResult HandleSubscribeKeepalive(
        Envelope &request,
        ::std::vector<Envelope> &output) = 0;

    /// Callback to handle writing of envelopes
    ///
    /// Receives the path we are writing to (already validated to be a
    /// stream set or stream). If a stream set, it is validated to exist.
    /// If a stream, it may not exist.
    ///
    /// The wait_for_write parameter defines whether we should wait for
    /// writes to complete before returning. This is only relevant for
    /// synchronous handlers.
    ///
    /// For synchronous handlers, the implementation should return
    /// RequestProcessorHandlerResult::MakeWriteResult().
    ///
    /// For asynchronous handlers, the implementation should return
    /// RequestProcessorHandlerResult::MakeDeferred() and ensure that
    /// ...
    /// @todo document asynchronous completion requirements
    ///
    /// @param path Path to write to
    /// @param to_write Set of envelopes to write to specified path
    /// @param wait_for_write Whether to wait for writes before returning
    ///
    virtual RequestProcessorHandlerResult HandleWriteEnvelopes(
        ::std::string const &path,
        ::std::vector<Envelope> &to_write,
        bool wait_for_write) = 0;

    /// Callback to handle registration of a plugin
    ///
    /// The implementation can register the plugin either synchronously or
    /// asynchronously.
    ///
    /// For synchronous registration, the returned object should be created
    /// with RequestProcessorHandlerResult::MakeSynchronous().
    ///
    /// For asynchronous registration, the returned object should be created
    /// with RequestProcessorHandlerResult::MakeDeferred() and the eventual
    /// handler should call RequestProcessor::SendPluginRegistrationAck() to
    /// send the expected protocol response to the client.
    ///
    /// In case of an error during asynchronous registration,
    /// RequestProcessor::SendErrorResponse() should be called.
    virtual RequestProcessorHandlerResult HandleRegisterPlugin(
        PluginRegistrationRequest const &r) = 0;

    /// Callback to handle plugin unregistering
    ///
    /// The implementation can unregister the plugin synchronously or
    /// asynchronously.
    ///
    /// @param name Name of plugin being unregistered
    /// @return How the request was handled
    virtual RequestProcessorHandlerResult HandleUnregisterPlugin(
        ::std::string const &name) = 0;

    /// Callback to handle obtaining plugin status
    ///
    /// The implementation can obtain state synchronously or asynchronously.
    ///
    /// In the case of synchronous functionality,
    /// RequestProcessorHandlerResult::MakePluginStatus() can be used to
    /// construct an appropriate return value.
    ///
    /// In the case of asynchronous operation,
    /// RequestProcessorHandlerResult::MakeDeferred() should be used and the
    /// eventual handler should call
    /// RequestProcessor::SendPluginStatusResponse().
    ///
    /// If the list of names is empty, the handler should obtain information
    /// on all plugins.
    ///
    /// @param names List of plugins to obtain status of
    /// @return How the request was handled
    virtual RequestProcessorHandlerResult HandleGetPluginStatus(
        ::std::vector< ::std::string > const &names) = 0;

private:
    RequestProcessorImplementation(RequestProcessorImplementation const &orig);
    RequestProcessorImplementation & operator=(RequestProcessorImplementation const &orig);
};

/// Used to construct a request processor
class ZIPPYLOG_EXPORT RequestProcessorStartParams {
public:
    RequestProcessorStartParams() :
        ctx(NULL),
        implementation(NULL),
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

    /// Implementation of a request processor to use for event handling
    ///
    /// Ownership of memory is transferred to the request processor upon
    /// request processor construction.
    RequestProcessorImplementation *implementation;

    /// 0MQ endpoint to which to bind a XREP socket to receive client requests
    ::std::string client_endpoint;

    /// 0MQ endpoint to which to connect to send log envelopes
    ::std::string logger_endpoint;

    /// Semaphore indicating whether the processor should remain alive
    ///
    /// When this signals, the request processor will cease processing
    /// new messages and will return from its run routine.
    ::zippylog::platform::ConditionalWait *active;

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
/// This class contains all the logic for processing zippylog protocol
/// request and response messages. It implements core functionality for
/// parsing and verifying protocol requests as well as servicing of some
/// basic request types.
///
/// Each instance of a request processor is associated with an instance
/// of RequestProcessorImplementation. The implementation class contains
/// handlers which are called by the RequestProcessor to handle specific
/// events, such as subscription requests and handling of writes.
///
/// This core class can be thought of as a sanitization and filtering gateway
/// to more core logic. It will transparently convert 0MQ-based zippylog
/// protocol requests to C++ objects and vice-versa. This means you should
/// never have to touch the zippylog protocol anywhere outside of this class.
/// If you find yourself doing things protocol-related outside of this class,
/// you are likely doing it wrong.
///
/// Currently, we make the assumption that protocol requests arrive via
/// 0MQ sockets. Strictly speaking, this isn't very loosely coupled. However,
/// the various Process* functions don't expose 0MQ details, so it should be
/// possible to call into this class without serializing envelopes as 0MQ
/// messages. That being said, the constructor still requires the 0MQ
/// parameters (for now, at least).
///
/// In previous versions of the code, this was an abstract base class.
/// It was changed to RequestProcessorImplementation because of the desire
/// to not deal with inheritance.
///
/// @todo Remove device-like API
class ZIPPYLOG_EXPORT RequestProcessor : public ::zippylog::device::Device {
    public:
        /// Construct from parameters
        ///
        /// @param params Parameters to control behavior
        RequestProcessor(RequestProcessorStartParams &params);

        ~RequestProcessor();

        /// Perform processing activities
        ///
        /// Reads available client messages from the listening socket(s) and
        /// processes them.
        ///
        /// If no messages are available, waits up to the specified number of
        /// milliseconds and process any that arrive before that time window
        /// expires.
        ::zippylog::device::PumpResult Pump(int32 wait_milliseconds);

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
        void ProcessMessages(::zippylog::zeromq::MessageContainer &messages, ::std::vector<Envelope> &output);

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
        RequestProcessorResponseStatus ProcessRequest(Envelope &e, ::std::vector<Envelope> &output);

        /// Sends an error response
        ///
        /// @param sock Socket to send response through
        /// @param code Error code
        /// @param message Error message
        static bool SendErrorResponse(::zmq::socket_t &sock,
                                      ::zippylog::protocol::response::ErrorCode code,
                                      ::std::string const & message);


        /// Sends a subscription store change response for an added path
        ///
        /// @param sock Socket to send response through
        /// @param subscription Subscription record
        /// @param path Path that was added
        static bool SendSubscriptionStoreChangePathAddedResponse(::zmq::socket_t &sock,
                                                                 SubscriptionInfo const &subscription,
                                                                 ::std::string const &path);

        /// Sends a subscription store change response for a deleted path
        ///
        /// @param sock Socket to send response through
        /// @param subscription Subscription record
        /// @param path Path that was deleted
        static bool SendSubscriptionStoreChangePathDeletedResponse(::zmq::socket_t &sock,
                                                                   SubscriptionInfo const &subscription,
                                                                   ::std::string const &path);

        /// Sends a subscription envelope response
        ///
        /// To optimize envelope sending and prevent excessive buffering, a
        /// helper class is used to control output. The pattern is as follows:
        ///
        ///  -# Create an EnvelopeSubscriptionResponseState instance
        ///  -# Mark an envelope to be sent by calling AddEnvelope() on this state
        ///  -# Call RequestProcessor::SendSubscriptionEnvelopeResponse()
        ///  -# Repeat above two steps
        ///  -# When you've reached the final envelope, call Finalize() on the state
        ///  -#  Call SendSubscriptionEnvelopeResponse() one last time
        ///
        /// Internally, this function buffers envelopes until a capacity has
        /// been reached and flushes them as necessary. This allows streaming
        /// code to send many envelopes without having to worry about all the
        /// details.
        ///
        /// @param sock Socket to send response through
        /// @param state State keeping track of what to send
        static bool SendSubscriptionEnvelopeResponse(::zmq::socket_t &sock,
                                                     EnvelopeSubscriptionResponseState &state);

        /// Sends a plugin registration ack response
        ///
        /// This is typically called by asynchronous plugin registration
        /// handlers.
        ///
        /// If the function returns false, there is not much the caller can do
        /// expect log the error.
        ///
        /// @param sock Socket to send response to
        /// @param md Metadata for response generation
        /// @param name Name of plugin that was registered
        /// @return Whether response sent without error
        static bool SendPluginRegistrationAck(::zmq::socket_t &sock,
                                              ResponseMetadata const &md,
                                              const ::std::string &name);
    protected:
        /// Implement device interface
        void OnRunStart();
        void OnRunFinish();

        /// Process a ping request
        ///
        /// @param request Request envelope
        /// @param output Container for response envelopes
        /// @return Processing result
        RequestProcessorResponseStatus ProcessPing(Envelope &request, ::std::vector<Envelope> &output);

        /// Process a GetFeatures request
        ///
        /// @param request Envelope containing request
        /// @param output Set of response envelopes
        /// @return Processing result
        RequestProcessorResponseStatus ProcessFeatures(Envelope &request, ::std::vector<Envelope> &output);

        /// Process a GetStoreInfo request and populate the passed envelope with the response
        ///
        /// This function is typically called only by ProcessRequest().
        ///
        /// @param request Request envelope
        /// @param output Container for response envelopes
        /// @return Processing result
        RequestProcessorResponseStatus ProcessStoreInfo(Envelope &request, ::std::vector<Envelope> &output);

        /// Process a bucket info request
        ///
        /// @param request Request envelope
        /// @param output Container for response envelopes
        /// @return Processing result
        RequestProcessorResponseStatus ProcessBucketInfo(Envelope &request, ::std::vector<Envelope> &output);

        /// Process a stream set request
        ///
        /// @param request Request envelope
        /// @param output Container for response envelopes
        /// @return Processing result
        RequestProcessorResponseStatus ProcessStreamSetInfo(Envelope &request, ::std::vector<Envelope> &output);

        /// Process a stream info request
        ///
        /// @param request Request envelope
        /// @param output Container for response envelopes
        /// @return Processing result
        RequestProcessorResponseStatus ProcessStreamInfo(Envelope &request, ::std::vector<Envelope> &output);

        /// Process a GetStream request
        ///
        /// @param request Request envelope
        /// @param output Container for response envelopes
        /// @return Processing result
        RequestProcessorResponseStatus ProcessGetStream(Envelope &request, ::std::vector<Envelope> &output);

        /// Process a store change subscription request
        /// @param request Request envelope
        /// @param output Container for response envelopes
        /// @return Processing result
        RequestProcessorResponseStatus ProcessSubscribeStoreChanges(Envelope &request, ::std::vector<Envelope> &output);

        /// Process a subscribe envelopes request
        ///
        /// @param request Request envelope
        /// @param output Container for response envelopes
        /// @return Processing result
        RequestProcessorResponseStatus ProcessSubscribeEnvelopes(Envelope &request, ::std::vector<Envelope> &output);

        /// Process a subscribe keepalive request
        ///
        /// @param request Request envelope
        /// @param output Container for response envelopes
        /// @return Processing result
        RequestProcessorResponseStatus ProcessSubscribeKeepalive(Envelope &request, ::std::vector<Envelope> &output);

        /// Process a SubscribeCancel request
        ///
        /// @param request Request envelope
        /// @param output Container for response, if authoritative
        /// @return Processing result
        RequestProcessorResponseStatus ProcessSubscribeCancel(Envelope &request, ::std::vector<Envelope> &output);

        /// Process a WriteEnvelope request
        ///
        /// @param request Request envelope
        /// @param output Container for response envelopes
        /// @return Processing result
        RequestProcessorResponseStatus ProcessWriteEnvelope(Envelope &request, ::std::vector<Envelope> &output);

        /// Process a RegisterPlugin request
        ///
        /// @param request Request envelope
        /// @param output Container for response
        /// @return Processing result
        RequestProcessorResponseStatus ProcessRegisterPlugin(Envelope &request, ::std::vector<Envelope> &output);

        /// Process an UnregisterPlugin request
        ///
        /// @param request Request envelope
        /// @param output Container for response
        /// @return Processing result
        RequestProcessorResponseStatus ProcessUnregisterPlugin(Envelope &request, ::std::vector<Envelope> &output);

        /// Process a PluginStatus request
        ///
        /// @param request Request envelope
        /// @param output Container for response
        /// @return Processing result
        RequestProcessorResponseStatus ProcessPluginStatus(Envelope &request, ::std::vector<Envelope> &output);

        /// Calls the handler to process a request related to subscriptions
        ///
        /// @param subscription Describes the subscription
        /// @param output Container for response
        void CallHandleSubscriptionRequest(SubscriptionInfo &subscription, ::std::vector<Envelope> &output);

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
        ///
        /// @param path zippylog path to validate
        /// @param output Container to hold error message on failure
        /// @param require_bucket Whether to require a bucket in the path
        /// @param require_set Whether to require a stream set in the path
        /// @param require_stream Whether to require a stream in the path
        /// @return Whether the path validated
        bool CheckPath(::std::string const &path,
                       ::std::vector<Envelope> &output,
                       bool require_bucket = false,
                       bool require_set = false,
                       bool require_stream = false);

        /// Checks that the version in a message is THE version we support
        ///
        /// If it isn't, an error response is added to the output messages.
        ///
        /// @param seen_version Message version seen
        /// @param supported_version Specific version we support
        /// @param output Container for response envelopes
        /// @return Whether the version is supported
        bool CheckMessageVersion(uint32 seen_version, uint32 supported_version, ::std::vector<Envelope> &output);

        /// Populates an error response to a request
        ///
        /// This is called whenever we wish to send an error to the client.
        ///
        /// @param code The error code for the error condition
        /// @param message Human readable message describing the error
        /// @param msgs Container for response envelopes
        /// @return Whether we completed without error
        bool PopulateErrorResponse(::zippylog::protocol::response::ErrorCode code, ::std::string message, ::std::vector<Envelope> &msgs);

        /// 0MQ context for internal sockets
        ::zmq::context_t *ctx;

        /// Provider of callbacks for handling
        RequestProcessorImplementation *impl;

        /// The path to the store
        ::std::string store_path;

        /// The 0MQ endpoint the logger should send messages to
        ::std::string logger_endpoint;

        /// The 0MQ endpoint on which to receive client requests
        ::std::string client_endpoint;

        /// PUSH socket to send log messages on
        ::zmq::socket_t * logger_sock;

        /// XREP socket that communicates with client
        ::zmq::socket_t * socket;

        /// The store we are bound to
        Store * store;

        /// Unique identifier for this instance
        ::std::string id;

        /// 0MQ socket identities for the current request
        ::std::vector< ::std::string > current_request_identities;

        /// Maximum number of bytes we're allowed to return per GetStreamSegment request
        uint32 get_stream_max_bytes;

        /// Maximum number of enevelopes we can return per GetStreamSegment request
        uint32 get_stream_max_envelopes;

        /// Maximum size (in kilobytes) a Lua state can allocate
        uint32 lua_memory_max;

        /// Subscription time-to-live in milliseconds
        uint32 subscription_ttl;

private:
        RequestProcessor(RequestProcessor const &orig);
        RequestProcessor & operator=(RequestProcessor const &orig);

};

} // namespace

#endif
