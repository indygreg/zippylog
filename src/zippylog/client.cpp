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

#include <zippylog/client.hpp>

#include <zippylog/envelope.hpp>
#include <zippylog/protocol/request.pb.h>
#include <zippylog/store.hpp>
#include <zippylog/zeromq.hpp>
#include <zmq.hpp>

using ::std::invalid_argument;
using ::std::map;
using ::std::string;
using ::std::vector;
using ::zippylog::protocol::response::SubscriptionStartV1;
using ::zippylog::Store;
using ::zmq::context_t;
using ::zmq::message_t;
using ::zmq::pollitem_t;
using ::zmq::socket_t;

namespace zippylog {
namespace client {

/// Stores client state about requests sent that haven't seen a response.
///
/// This class is used internally by the client and should be of little
/// interest to others.
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
        cb_subscription(NULL),
        data(NULL)
    { }

    friend class Client;

protected:
    /// The ID of the issued request
    ::std::string id;

    ///@{
    /// Callbacks that handle processing each type of response
    PingCallback *                cb_ping;
    GetFeaturesCallback *         cb_features;
    StoreInfoCallback *           cb_store_info;
    BucketInfoCallback *          cb_bucket_info;
    StreamSetInfoCallback *       cb_stream_set_info;
    StreamInfoCallback *          cb_stream_info;
    StreamSegmentCallback *       cb_stream_segment;
    SubscriptionRequestCallback * cb_subscription;
    ///@}

    /// Information about subscription callbacks
    SubscriptionCallbackInfo callbacks;

    /// Custom data to pass to callback
    void *data;
};

/// Holds state of a single subscription for a single client
class Subscription {
public:
    Subscription();

    friend class Client;
protected:
    /// The subscription ID
    ::std::string id;

    /// Timer that keeps track of when the subscription expires.
    ///
    /// This is used to determine when to send the subscription keepalive
    platform::Timer expiration_timer;

    /// Describes what callbacks will handle subscription responses
    SubscriptionCallbackInfo cb;

    /// Arbitrary data to pass to callbacks
    void *data;
};


Client::Client(context_t *ctx, string const &endpoint) :
    client_sock(NULL),
    subscription_renewal_offset(5000000), // 5 seconds
    exec_thread(NULL),
    run_flag(NULL)
{
    if (!ctx) {
        throw invalid_argument("ctx parameter cannot be NULL");
    }

    this->client_sock = new socket_t(*ctx, ZMQ_XREQ);

    // do we need to set socket identity since we are using XREQ?

    this->client_sock->connect(endpoint.c_str());

    this->pollitem[0].events = ZMQ_POLLIN;
    this->pollitem[0].fd = 0;
    this->pollitem[0].socket = *this->client_sock;
}

Client::~Client()
{
    if (this->exec_thread) {
        this->exec_thread->Abort();
        delete this->exec_thread;
    }

    this->CancelAllSubscriptions();

    if (this->client_sock) delete this->client_sock;
}

bool Client::Ping(PingCallback *callback, void *data)
{
    if (!callback) {
        throw invalid_argument("callback parameter not defined");
    }

    Envelope e;
    protocol::request::PingV1 ping;
    ping.add_to_envelope(e);

    OutstandingRequest outr;
    outr.cb_ping = callback;
    outr.data = data;

    return this->SendRequest(e, outr);
}

bool Client::Ping(int32 timeout)
{
    Envelope e;
    protocol::request::PingV1 ping;
    ping.add_to_envelope(e);

    OutstandingRequest outr;
    outr.cb_ping = CallbackPing;

    return this->SendAndProcessSynchronousRequest(e, outr, timeout);
}

void Client::CallbackPing(Client *, void *)
{}

bool Client::GetFeatures(GetFeaturesCallback *callback, void *data)
{
    if (!callback) {
        throw invalid_argument("callback parameter not defined");
    }

    Envelope e;
    protocol::request::GetFeaturesV1 m;
    m.add_to_envelope(e);

    OutstandingRequest r;
    r.cb_features = callback;
    r.data = data;

    return this->SendRequest(e, r);
}

bool Client::GetFeatures(protocol::response::FeatureSpecificationV1 &features, int32 timeout)
{
    Envelope e;
    protocol::request::GetFeaturesV1 m;
    m.add_to_envelope(e);

    OutstandingRequest r;
    r.cb_features = CallbackFeatures;
    r.data = (void *)&features;

    return this->SendAndProcessSynchronousRequest(e, r, timeout);
}

void Client::CallbackFeatures(Client *, protocol::response::FeatureSpecificationV1 &response, void *data)
{
    protocol::response::FeatureSpecificationV1 *features = (protocol::response::FeatureSpecificationV1 *)data;

    features->CopyFrom(response);
}

bool Client::GetStoreInfo(StoreInfoCallback * callback, void *data)
{
    if (!callback) {
        throw invalid_argument("callback parameter not defined");
    }

    Envelope e = Envelope();
    protocol::request::GetStoreInfoV1 req;
    req.add_to_envelope(&e);

    OutstandingRequest info = OutstandingRequest();
    info.cb_store_info = callback;
    info.data = data;

    return this->SendRequest(e, info);
}

bool Client::GetStoreInfo(protocol::StoreInfoV1 &info, int32 timeout)
{
    Envelope e;
    protocol::request::GetStoreInfoV1 req;
    req.add_to_envelope(&e);

    OutstandingRequest outr;
    outr.data = &info;
    outr.cb_store_info = CallbackStoreInfo;

    return this->SendAndProcessSynchronousRequest(e, outr, timeout);
}

void Client::CallbackStoreInfo(Client *, protocol::StoreInfoV1 &info, void *data)
{
    protocol::StoreInfoV1 *si = (protocol::StoreInfoV1 *)data;

    si->CopyFrom(info);
}

bool Client::GetBucketInfo(string const &path, BucketInfoCallback *callback, void*data)
{
    if (!callback) {
        throw invalid_argument("callback parameter not defined");
    }

    if (!Store::IsBucketPath(path)) {
        throw invalid_argument("path is not a valid bucket path");
    }

    Envelope e;
    protocol::request::GetBucketInfoV1 m;
    m.set_path(path);
    m.add_to_envelope(e);

    OutstandingRequest r;
    r.cb_bucket_info = callback;
    r.data = data;

    return this->SendRequest(e, r);
}

bool Client::GetBucketInfo(string const &path, protocol::BucketInfoV1 &info, int32 timeout)
{
    if (!Store::IsBucketPath(path)) {
        throw invalid_argument("path is not a valid bucket path");
    }

    Envelope e;
    protocol::request::GetBucketInfoV1 m;
    m.set_path(path);
    m.add_to_envelope(e);

    OutstandingRequest r;
    r.data = &info;
    r.cb_bucket_info = CallbackBucketInfo;

    return this->SendAndProcessSynchronousRequest(e, r, timeout);
}

void Client::CallbackBucketInfo(Client *, protocol::BucketInfoV1 &info, void *data)
{
    protocol::BucketInfoV1 *bi = (protocol::BucketInfoV1 *)data;
    bi->CopyFrom(info);
}

bool Client::GetStreamSetInfo(string const &path, StreamSetInfoCallback *callback, void *data)
{
    if (!callback) {
        throw invalid_argument("callback is not defined");
    }

    if (!Store::IsStreamSetPath(path)) {
        throw invalid_argument("path is not a path to a stream set");
    }

    Envelope e;
    protocol::request::GetStreamSetInfoV1 m;
    m.set_path(path);
    m.add_to_envelope(e);

    OutstandingRequest r;
    r.cb_stream_set_info = callback;
    r.data = data;

    return this->SendRequest(e, r);
}

bool Client::GetStreamSetInfo(string const &path, protocol::StreamSetInfoV1 &info, int32 timeout)
{
    if (!Store::IsStreamSetPath(path)) {
        throw invalid_argument("path is not a valid path to stream set");
    }

    Envelope e;
    protocol::request::GetStreamSetInfoV1 m;
    m.set_path(path);
    m.add_to_envelope(e);

    OutstandingRequest r;
    r.data = &info;
    r.cb_stream_set_info = CallbackStreamSetInfo;

    return this->SendAndProcessSynchronousRequest(e, r, timeout);
}

void Client::CallbackStreamSetInfo(Client *, protocol::StreamSetInfoV1 &info, void *data)
{
    protocol::StreamSetInfoV1 *si = (protocol::StreamSetInfoV1 *)data;
    si->CopyFrom(info);
}

bool Client::GetStreamInfo(string const &path, StreamInfoCallback *callback, void *data)
{
    if (!callback) {
        throw invalid_argument("callback is not defined");
    }

    if (!Store::IsStreamPath(path)) {
        throw invalid_argument("path is not a path to a stream");
    }

    Envelope e;
    protocol::request::GetStreamInfoV1 m;
    m.set_path(path);
    m.add_to_envelope(e);

    OutstandingRequest r;
    r.cb_stream_info = callback;
    r.data = data;

    return this->SendRequest(e, r);
}

bool Client::GetStreamInfo(string const &path, protocol::StreamInfoV1 &info, int32 timeout)
{
    Envelope e;
    protocol::request::GetStreamInfoV1 req;
    req.set_path(path);
    req.add_to_envelope(e);

    OutstandingRequest outr;
    outr.data = &info;
    outr.cb_stream_info = CallbackStreamInfo;

    return this->SendAndProcessSynchronousRequest(e, outr, timeout);
}

void Client::CallbackStreamInfo(Client *, protocol::StreamInfoV1 &info, void *data)
{
    protocol::StreamInfoV1 *si = (protocol::StreamInfoV1 *)data;
    si->CopyFrom(info);
}

bool Client::GetStreamSegment(string const &path, uint64 start_offset, StreamSegmentCallback * callback, void *data)
{
    if (!callback) {
        throw invalid_argument("callback parameter not defined");
    }

    Envelope e = Envelope();
    protocol::request::GetStreamSegmentV1 req;
    req.set_path(path);
    req.set_start_offset(start_offset);
    req.add_to_envelope(&e);

    OutstandingRequest info = OutstandingRequest();
    info.cb_stream_segment = callback;
    info.data = data;

    return this->SendRequest(e, info);
}

bool Client::GetStreamSegment(string const &path, uint64 start_offset, StreamSegment &segment, int32 timeout)
{
    protocol ::request::GetStreamSegmentV1 req;
    req.set_path(path);
    req.set_start_offset(start_offset);
    Envelope e;
    req.add_to_envelope(&e);

    OutstandingRequest outr;
    outr.data = &segment;
    outr.cb_stream_segment = CallbackStreamSegment;

    return this->SendAndProcessSynchronousRequest(e, outr, timeout);
}

void Client::CallbackStreamSegment(Client *, string const &, uint64, StreamSegment &segment, void *data)
{
    StreamSegment *s = (StreamSegment *)data;
    s->CopyFrom(segment);
}

bool Client::GetStreamSegment(string const &path, uint64 start_offset, uint32 max_response_bytes, StreamSegmentCallback * callback, void *data)
{
    if (!callback) {
        throw invalid_argument("callback parameter not defined");
    }

    Envelope e = Envelope();
    protocol::request::GetStreamSegmentV1 req;
    req.set_path(path);
    req.set_start_offset(start_offset);
    req.set_max_response_bytes(max_response_bytes);
    req.add_to_envelope(&e);

    OutstandingRequest info = OutstandingRequest();
    info.cb_stream_segment = callback;
    info.data = data;

    return this->SendRequest(e, info);
}

bool Client::GetStreamSegment(string const &path, uint64 start_offset, uint64 stop_offset, StreamSegmentCallback * callback, void *data)
{
    return this->GetStreamSegment(path, start_offset, (uint32)(stop_offset - start_offset), callback, data);
}

bool Client::GetStream(string const &path, StreamFetchState &state, StreamSegmentCallback * callback, void * data, uint64 end_offset)
{
    if (!callback) {
        throw invalid_argument("callback parameter not defined");
    }

    if (!end_offset) {
        protocol::StreamInfoV1 si;
        if (!this->GetStreamInfo(path, si, -1)) {
            return false;
        }

        end_offset = si.length();
    }

    // we've already fetched it all
    if (state.end_offset >= end_offset) return true;

    uint64 start_offset = state.end_offset;

    while (true) {
        StreamSegment segment;
        if (!this->GetStreamSegment(path, start_offset, segment, -1)) {
            return false;
        }

        if (!segment.BytesSent) break;

        callback(this, path, start_offset, segment, data);

        start_offset = segment.EndOffset;

        if (start_offset >= end_offset) break;
    }

    state.end_offset = start_offset;

    return true;
}

bool Client::Mirror(StoreMirrorState &state, StreamSegmentCallback * callback, void * data)
{
    if (!callback) {
        throw invalid_argument("callback parameter not defined");
    }

    protocol::StoreInfoV1 info;
    if (!this->GetStoreInfo(info, -1)) {
        return false;
    }

    for (int i = 0; i < info.bucket_size(); i++) {
        protocol::BucketInfoV1 bi = info.bucket(i);

        for (int j = 0; j < bi.stream_set_size(); j++) {
            protocol::StreamSetInfoV1 ssi = bi.stream_set(j);

            for (int k = 0; k < ssi.stream_size(); k++) {
                protocol::StreamInfoV1 si = ssi.stream(k);

                string path = Store::StreamPath(bi.path(), ssi.path(), si.path());

                map<string, StreamFetchState>::iterator itor = state.states.find(path);

                if (itor == state.states.end()) {
                    StreamFetchState fetch_state;
                    fetch_state.end_offset = 0;

                    state.states[path] = fetch_state;
                }

                if (!this->GetStream(path, state.states[path], callback, data, si.length())) {
                    return false;
                }
            }
        }
    }

    return true;
}

bool Client::SubscribeStoreChanges(vector<string> const &paths, SubscriptionCallbackInfo &callbacks, SubscriptionRequestResult &result, int32 timeout)
{
    protocol::request::SubscribeStoreChangesV1 m;

    for (size_t i = 0; i < paths.size(); i++) {
        if (!Store::ValidatePath(paths[i])) {
            throw invalid_argument("path is not valid");
        }

        m.add_path(paths[i]);
    }

    Envelope e;
    m.add_to_envelope(e);

    OutstandingRequest r;
    r.callbacks = callbacks;
    r.data = &result;
    r.cb_subscription = CallbackSubscription;

    return this->SendAndProcessSynchronousRequest(e, r, timeout);
}

bool Client::SubscribeStoreChanges(string const &path, SubscriptionCallbackInfo &callbacks, SubscriptionRequestResult &result, int32 timeout)
{
    vector<string> paths;
    paths.push_back(path);
    return this->SubscribeStoreChanges(paths, callbacks, result, timeout);
}

bool Client::SubscribeStoreChanges(SubscriptionCallbackInfo &callbacks, SubscriptionRequestResult &result, int32 timeout)
{
    vector<string> paths;
    return this->SubscribeStoreChanges(paths, callbacks, result, timeout);
}

void Client::CallbackSubscription(Client *, protocol::response::SubscriptionAcceptAckV1 &ack, void *data)
{
    SubscriptionRequestResult *result = (SubscriptionRequestResult *)data;
    result->id = ack.id();
    result->ttl = ack.ttl();
    result->result = SubscriptionRequestResult::ACCEPTED;
}

bool Client::SubscribeEnvelopes(string const &path, SubscriptionCallbackInfo &cb, void *data)
{
    return this->SubscribeEnvelopes(path, "", cb, data);
}

bool Client::SubscribeEnvelopes(string const &path, string const &lua, SubscriptionCallbackInfo &cb, void *data)
{
    // @todo validate

    protocol::request::SubscribeEnvelopesV1 req;
    req.add_path(path);

    if (!lua.empty()) {
        req.set_lua_code(lua);
    }

    Envelope e = Envelope();
    req.add_to_envelope(&e);

    OutstandingRequest info = OutstandingRequest();
    info.data = data;
    info.callbacks = cb;

    return this->SendRequest(e, info);
}


bool Client::SendRequest(Envelope &e, OutstandingRequest &req)
{
    string id = platform::CreateUUID(false);

    e.AddTag(id);
    req.id = id;

    vector<string> identities;

    if (!zeromq::SendEnvelope(*this->client_sock, identities, e, true, 0)) {
        return false;
    }

    this->outstanding[id] = req;

    return true;
}

bool Client::SendAndProcessSynchronousRequest(Envelope &e, OutstandingRequest &req, int32 timeout)
{
    platform::UUID uuid;
    platform::CreateUUID(uuid);

    string id = string((const char *)&uuid, sizeof(uuid));
    e.AddTag(id);
    req.id = id;

    vector<string> identities;

    if (!zeromq::SendEnvelope(*this->client_sock, identities, e, true, 0)) {
        return false;
    }

    this->outstanding[id] = req;

    platform::Timer timer(timeout);
    timer.Start();

    bool result = false;
    bool have_timer = timeout > 0;

    // @todo this can be done with fewer system calls
    do {
        // we wait up to 25ms in each iteration
        this->Pump(25000);

        // this must mean we processed it
        if (!this->HaveOutstandingRequest(id)) {
            result = true;
            break;
        }
    } while (!have_timer || (have_timer && !timer.Signaled()));

    return result;
}

int Client::Pump(int32 timeout)
{
    if (::zmq::poll(this->pollitem, 1, timeout) > 0) {
        return this->ProcessPendingMessage() ? 1 : -1;
    }

    return 0;
}

bool Client::ProcessPendingMessage()
{
    vector<message_t *> messages;
    if (!zeromq::receive_multipart_message(this->client_sock, messages)) {
        return false;
    }

    bool result = this->ProcessResponseMessage(messages);

    for (vector<message_t *>::iterator i = messages.begin(); i != messages.end(); i++) {
        delete *i;
    }

    return result;
}

bool Client::ProcessResponseMessage(vector<message_t *> &messages)
{
    // first message is an empty message. we delete it
    assert(messages.size() > 1);
    delete messages[0];
    messages.erase(messages.begin());

    Envelope e(*messages[0], 1);

    // that's most weird
    if (e.MessageCount() < 1) {
        return false;
    }

    // first message is always in the zippylog namespace
    if (e.MessageNamespace(0) != 1) {
        return false;
    }

    // two major branches follow the division in message types:
    //   1) those that follow strict request-reply
    //   2) subscription responses

    switch (e.MessageType(0)) {
        // beginning of data in response to a subscription
        case protocol::response::SubscriptionStartV1::zippylog_enumeration:
        {
            protocol::response::SubscriptionStartV1 *start =
                (protocol::response::SubscriptionStartV1 *)e.GetMessage(0);

            if (!this->ValidateSubscriptionStart(*start)) {
                return false;
            }

            messages.pop_back();
            return this->HandleSubscriptionResponse(e, *start, messages);
            break;
        }

        case protocol::response::PongV1::zippylog_enumeration:
        case protocol::StoreInfoV1::zippylog_enumeration:
        case protocol::response::StreamSegmentStartV1::zippylog_enumeration:
        case protocol::response::SubscriptionAcceptAckV1::zippylog_enumeration:
            return this->HandleRequestResponse(e, messages);
            break;

        default:
            // @todo log here or something
            break;
    }

    return false;
}

bool Client::ValidateSubscriptionStart(SubscriptionStartV1 &start)
{
    if (!start.has_id()) return false;

    return this->HasSubscription(start.id());
}

bool Client::HasSubscription(string const &id)
{
    map<string, Subscription>::iterator iter = this->subscriptions.find(id);

    return iter != this->subscriptions.end();
}

bool Client::CancelAllSubscriptions()
{
    map<string, Subscription>::iterator iter = this->subscriptions.begin();
    for (; iter != subscriptions.end(); iter++) {
        this->CancelSubscription(iter->first);
    }

    return true;
}

bool Client::CancelSubscription(string const &)
{
    return false;
}

bool Client::HandleSubscriptionResponse(Envelope &e, SubscriptionStartV1 &start, vector<message_t *> &messages)
{
    map<string, Subscription>::iterator iter = this->subscriptions.find(start.id());
    // subscription could have disappeared since it was validated
    if (iter == this->subscriptions.end()) return false;

    SubscriptionCallbackInfo cb = iter->second.cb;

    for (int i = 1; i < e.MessageCount(); i++) {
        switch (e.MessageType(i)) {
            case protocol::StoreChangeBucketAddedV1::zippylog_enumeration:
                if (cb.BucketAdded) {
                    protocol::StoreChangeBucketAddedV1 *added = (protocol::StoreChangeBucketAddedV1 *)e.GetMessage(i);
                    cb.BucketAdded(this, start.id(), *added, iter->second.data);
                }
                break;
            case protocol::StoreChangeBucketDeletedV1::zippylog_enumeration:
                if (cb.BucketDeleted) {
                    protocol::StoreChangeBucketDeletedV1 *deleted = (protocol::StoreChangeBucketDeletedV1 *)e.GetMessage(i);
                    cb.BucketDeleted(this, start.id(), *deleted, iter->second.data);
                }
                break;

            case protocol::StoreChangeStreamSetAddedV1::zippylog_enumeration:
                if (cb.StreamSetAdded) {
                    protocol::StoreChangeStreamSetAddedV1 *added = (protocol::StoreChangeStreamSetAddedV1 *)e.GetMessage(i);
                    cb.StreamSetAdded(this, start.id(), *added, iter->second.data);
                }
                break;

            case protocol::StoreChangeStreamSetDeletedV1::zippylog_enumeration:
                if (cb.StreamSetDeleted) {
                    protocol::StoreChangeStreamSetDeletedV1 *deleted = (protocol::StoreChangeStreamSetDeletedV1 *)e.GetMessage(i);
                    cb.StreamSetDeleted(this, start.id(), *deleted, iter->second.data);
                }
                break;

            case protocol::StoreChangeStreamAddedV1::zippylog_enumeration:
                if (cb.StreamAdded) {
                    protocol::StoreChangeStreamAddedV1 *added = (protocol::StoreChangeStreamAddedV1 *)e.GetMessage(i);
                    cb.StreamAdded(this, start.id(), *added, iter->second.data);
                }
                break;

            case protocol::StoreChangeStreamDeletedV1::zippylog_enumeration:
                if (cb.StreamDeleted) {
                    protocol::StoreChangeStreamDeletedV1 *deleted = (protocol::StoreChangeStreamDeletedV1 *)e.GetMessage(i);
                    cb.StreamDeleted(this, start.id(), *deleted, iter->second.data);
                }
                break;

            case protocol::StoreChangeStreamAppendedV1::zippylog_enumeration:
                if (cb.StreamAppended) {
                    protocol::StoreChangeStreamAppendedV1 *appended = (protocol::StoreChangeStreamAppendedV1 *)e.GetMessage(i);
                    cb.StreamAppended(this, start.id(), *appended, iter->second.data);
                }
                break;

            default:
                break;
        }
    }

    if (!cb.EnvelopeReceived) return true;

    // for now, assume additional messages envelopes that were streamed
    for (size_t i = 0; i < messages.size(); i++) {
        Envelope env = Envelope(messages[i]->data(), messages[i]->size());

        cb.EnvelopeReceived(this, start.id(), env, iter->second.data);
    }

    return true;
}

bool Client::HandleRequestResponse(Envelope &e, vector<message_t *> &messages)
{
    // all requests are tagged with a request identifier, so ignore any responses
    // not for a known tag

    if (!e.TagSize()) {
        throw Exception("received response with no tag to match to request");
    }

    string id = e.GetTag(0);

    // try to find the outstanding request
    map<string, OutstandingRequest>::iterator iter = this->outstanding.find(id);

    // this is most weird
    // @todo is an exception always correct? what about a client that restarts
    // with same identity?
    if (iter == this->outstanding.end()) {
        throw Exception("received a response to an unknown outstanding request");
    }

    OutstandingRequest req = iter->second;

    // at this point, the outstanding request entry isn't consulted, so we
    // dispose of it
    this->outstanding.erase(iter);

    switch (e.MessageType(0)) {
        case protocol::response::PongV1::zippylog_enumeration:
        {
            assert(req.cb_ping);
            req.cb_ping(this, req.data);

            return true;
        }

        case protocol::StoreInfoV1::zippylog_enumeration:
        {
            assert(req.cb_store_info);
            protocol::StoreInfoV1 *info = (protocol::StoreInfoV1 *)e.GetMessage(0);

            req.cb_store_info(this, *info, req.data);

            return true;
        }

        case protocol::BucketInfoV1::zippylog_enumeration:
        {
            assert(req.cb_bucket_info);
            protocol::BucketInfoV1 *info = (protocol::BucketInfoV1 *)e.GetMessage(0);

            req.cb_bucket_info(this, *info, req.data);

            return true;
        }

        case protocol::StreamSetInfoV1::zippylog_enumeration:
        {
            assert(req.cb_stream_set_info);
            protocol::StreamSetInfoV1 *info = (protocol::StreamSetInfoV1 *)e.GetMessage(0);

            req.cb_stream_set_info(this, *info, req.data);

            return true;
        }

        case protocol::StreamInfoV1::zippylog_enumeration:
        {
            assert(req.cb_stream_info);
            protocol::StreamInfoV1 *info = (protocol::StreamInfoV1 *)e.GetMessage(0);

            req.cb_stream_info(this, *info, req.data);

            return true;
        }

        case protocol::response::StreamSegmentStartV1::zippylog_enumeration:
        {
            assert(req.cb_stream_segment);

            // @todo more robust parsing
            protocol::response::StreamSegmentStartV1 *start =
                (protocol::response::StreamSegmentStartV1 *)e.GetMessage(0);

            StreamSegment segment;
            segment.SetPath(start->path());
            segment.SetStartOffset(start->offset());

            void * start_addr = (void *)((const char *)(messages[messages.size()-1]->data()) + 1);

            Envelope footer(start_addr, messages[messages.size()-1]->size() - 1);

            protocol::response::StreamSegmentEndV1 *end =
                (protocol::response::StreamSegmentEndV1 *)footer.GetMessage(0);

            segment.SetEndOffset(end->offset());
            segment.SetBytesSent(end->bytes_sent());
            segment.SetEnvelopesSent(end->envelopes_sent());

            for (size_t i = 1; i < messages.size() - 2; i++) {
                start_addr = (void *)((const char *)(messages[i]->data()) + 1);
                Envelope payload(start_addr, messages[i]->size() - 1);
                segment.AddEnvelope(payload);
            }

            req.cb_stream_segment(this, start->path(), start->offset(), segment, req.data);

            return true;
        }

        case protocol::response::SubscriptionAcceptAckV1::zippylog_enumeration:
        {
            protocol::response::SubscriptionAcceptAckV1 *ack =
                (protocol::response::SubscriptionAcceptAckV1 *)e.GetMessage(0);

            string id = ack->id();

            Subscription sub;
            sub.cb = req.callbacks;
            sub.id = id;
            sub.data = req.data;

            uint32 ttl = ack->ttl() * 1000;
            if (ttl < this->subscription_renewal_offset) {
                throw Exception("subscription TTL less than configured time offset");
            }

            sub.expiration_timer.Start(ttl - this->subscription_renewal_offset);

            this->subscriptions[id] = sub;

            assert(req.cb_subscription);
            req.cb_subscription(this, *ack, req.data);

            return true;
        }
    }

    return false;
}

bool Client::RenewSubscriptions(bool force)
{
    map<string, Subscription>::iterator iter = this->subscriptions.begin();

    protocol::request::SubscribeKeepaliveV1 msg;

    for (; iter != this->subscriptions.end(); iter++) {
        if (!force && !iter->second.expiration_timer.Signaled()) continue;

        // reset the timer
        iter->second.expiration_timer.Start();

        msg.add_id(iter->first);
    }

    Envelope e = Envelope();
    msg.add_to_envelope(&e);

    return zeromq::send_envelope_xreq(this->client_sock, e);
}

bool Client::HaveOutstandingRequest(string &id)
{
    map<string, OutstandingRequest>::iterator iter = this->outstanding.find(id);

    return iter != this->outstanding.end();
}

void Client::Run(bool *active)
{
    if (!active) {
        throw invalid_argument("parameter cannot be NULL");
    }

    while (*active) {
        this->Pump(100000);
    }
}

void Client::RunAsync(bool *active)
{
    if (this->exec_thread) {
        throw Exception("client thread already running");
    }

    this->run_flag = active;

    this->exec_thread = new platform::Thread(Client::AsyncStart, this);
}

void * Client::AsyncStart(void *data)
{
    try {
        Client *c = (Client *)data;
        c->Run(c->run_flag);
    }
    catch (::std::exception e) {
        string error = e.what();
        return (void *)1;
    }

    return (void *)0;
}

StreamSegment::StreamSegment()
{

}

StreamSegment::~StreamSegment()
{

}

bool StreamSegment::CopyFrom(StreamSegment const &orig)
{
    this->BytesSent = orig.BytesSent;
    this->EndOffset = orig.EndOffset;
    this->Envelopes = orig.Envelopes;
    this->EnvelopesSent = orig.EnvelopesSent;
    this->Path = orig.Path;
    this->StartOffset = orig.StartOffset;

    return true;
}

bool StreamSegment::SetPath(const string path)
{
    this->Path = path;
    return true;
}

bool StreamSegment::SetStartOffset(uint64 offset)
{
    this->StartOffset = offset;
    return true;
}

bool StreamSegment::SetEndOffset(uint64 offset)
{
    this->EndOffset = offset;
    return true;
}

bool StreamSegment::SetBytesSent(uint32 count)
{
    this->BytesSent = count;
    return true;
}

bool StreamSegment::SetEnvelopesSent(uint32 number)
{
    this->EnvelopesSent = number;
    return true;
}

bool StreamSegment::AddEnvelope(Envelope e)
{
    this->Envelopes.push_back(e);
    return true;
}

Subscription::Subscription() : expiration_timer()
{

}

}} // namespaces
