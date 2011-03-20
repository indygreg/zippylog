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
using ::zippylog::protocol::response::SubscriptionStart;
using ::zippylog::Store;
using ::zmq::context_t;
using ::zmq::message_t;
using ::zmq::pollitem_t;
using ::zmq::socket_t;

namespace zippylog {
namespace client {

Client::Client(context_t *ctx, const string &endpoint) :
    client_sock(NULL),
    subscription_renewal_offset(5000000) // 5 seconds
{
    if (!ctx) {
        throw invalid_argument("ctx parameter cannot be NULL");
    }

    this->client_sock = new socket_t(*ctx, ZMQ_XREQ);
    this->client_sock->connect(endpoint.c_str());

    this->pollitem[0].events = ZMQ_POLLIN;
    this->pollitem[0].fd = 0;
    this->pollitem[0].socket = *this->client_sock;
}

Client::~Client()
{
    this->CancelAllSubscriptions();

    if (this->client_sock) delete this->client_sock;
}

bool Client::Ping(PingCallback *callback, void *data)
{
    if (!callback) {
        throw invalid_argument("callback parameter not defined");
    }

    Envelope e;
    protocol::request::Ping ping;
    ping.add_to_envelope(e);

    OutstandingRequest or;
    or.cb_ping = callback;
    or.data = data;

    return this->SendRequest(e, or);
}

bool Client::Ping(int32 timeout)
{
    Envelope e;
    protocol::request::Ping ping;
    ping.add_to_envelope(e);

    OutstandingRequest or;
    or.cb_ping = CallbackPing;

    return this->SendAndProcessSynchronousRequest(e, or, timeout);
}

void Client::CallbackPing(void *data)
{}

bool Client::StoreInfo(StoreInfoCallback * callback, void *data)
{
    if (!callback) {
        throw invalid_argument("callback parameter not defined");
    }

    Envelope e = Envelope();
    protocol::request::GetStoreInfo req = protocol::request::GetStoreInfo();
    req.set_version(1);
    req.add_to_envelope(&e);

    OutstandingRequest info = OutstandingRequest();
    info.cb_store_info = callback;
    info.data = data;

    return this->SendRequest(e, info);
}

bool Client::StoreInfo(protocol::StoreInfo &info, int32 timeout)
{
    Envelope e;
    protocol::request::GetStoreInfo req;
    req.set_version(1);
    req.add_to_envelope(&e);

    OutstandingRequest outr;
    outr.data = &info;
    outr.cb_store_info = CallbackStoreInfo;

    return this->SendAndProcessSynchronousRequest(e, outr, timeout);
}

void Client::CallbackStoreInfo(protocol::StoreInfo &info, void *data)
{
    protocol::StoreInfo *si = (protocol::StoreInfo *)data;

    si->CopyFrom(info);
}

bool Client::StreamInfo(const string &path, protocol::StreamInfo &info, int32 timeout)
{
    Envelope e;
    protocol::request::GetStreamInfo req;
    req.set_version(1);
    req.set_path(path);
    req.add_to_envelope(e);

    OutstandingRequest outr;
    outr.data = &info;
    outr.cb_stream_info = CallbackStreamInfo;

    return this->SendAndProcessSynchronousRequest(e, outr, timeout);
}

void Client::CallbackStreamInfo(protocol::StreamInfo &info, void *data)
{
    protocol::StreamInfo *si = (protocol::StreamInfo *)data;
    si->CopyFrom(info);
}

bool Client::Get(const string &path, uint64 start_offset, StreamSegmentCallback * callback, void *data)
{
    if (!callback) {
        throw invalid_argument("callback parameter not defined");
    }

    Envelope e = Envelope();
    protocol::request::GetStream req = protocol::request::GetStream();
    req.set_version(1);
    req.set_path(path);
    req.set_start_offset(start_offset);
    req.add_to_envelope(&e);

    OutstandingRequest info = OutstandingRequest();
    info.cb_stream_segment = callback;
    info.data = data;

    return this->SendRequest(e, info);
}

bool Client::Get(const string &path, uint64 start_offset, StreamSegment &segment, int32 timeout)
{
    protocol ::request::GetStream req;
    req.set_version(1);
    req.set_path(path);
    req.set_start_offset(start_offset);
    Envelope e;
    req.add_to_envelope(&e);

    OutstandingRequest outr;
    outr.data = &segment;
    outr.cb_stream_segment = CallbackStreamSegment;

    return this->SendAndProcessSynchronousRequest(e, outr, timeout);
}

void Client::CallbackStreamSegment(const string &path, uint64 offset, StreamSegment &segment, void *data)
{
    StreamSegment *s = (StreamSegment *)data;
    s->CopyFrom(segment);
}

bool Client::Get(const string &path, uint64 start_offset, uint32 max_response_bytes, StreamSegmentCallback * callback, void *data)
{
    if (!callback) {
        throw invalid_argument("callback parameter not defined");
    }

    Envelope e = Envelope();
    protocol::request::GetStream req = protocol::request::GetStream();
    req.set_version(1);
    req.set_path(path);
    req.set_start_offset(start_offset);
    req.set_max_response_bytes(max_response_bytes);
    req.add_to_envelope(&e);

    OutstandingRequest info = OutstandingRequest();
    info.cb_stream_segment = callback;
    info.data = data;

    return this->SendRequest(e, info);
}

bool Client::Get(const string &path, uint64 start_offset, uint64 stop_offset, StreamSegmentCallback * callback, void *data)
{
    return this->Get(path, start_offset, (uint32)(stop_offset - start_offset), callback, data);
}

bool Client::GetStream(const string &path, StreamFetchState &state, StreamSegmentCallback * callback, void * data, uint64 end_offset)
{
    if (!callback) {
        throw invalid_argument("callback parameter not defined");
    }

    if (!end_offset) {
        protocol::StreamInfo si;
        if (!this->StreamInfo(path, si, -1)) {
            return false;
        }

        end_offset = si.length();
    }

    // we've already fetched it all
    if (state.end_offset >= end_offset) return true;

    uint64 start_offset = state.end_offset;

    while (true) {
        StreamSegment segment;
        if (!this->Get(path, start_offset, segment, -1)) {
            return false;
        }

        if (!segment.BytesSent) break;

        callback(path, start_offset, segment, data);

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

    protocol::StoreInfo info;
    if (!this->StoreInfo(info, -1)) {
        return false;
    }

    for (int i = 0; i < info.bucket_size(); i++) {
        protocol::BucketInfo bi = info.bucket(i);

        for (int j = 0; j < bi.stream_set_size(); j++) {
            protocol::StreamSetInfo ssi = bi.stream_set(j);

            for (int k = 0; k < ssi.stream_size(); k++) {
                protocol::StreamInfo si = ssi.stream(k);

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

bool Client::SubscribeStoreChanges(const string &path, SubscriptionCallbackInfo &cb, void *data)
{
    // TODO validate path

    protocol::request::SubscribeStoreChanges req = protocol::request::SubscribeStoreChanges();
    req.set_version(1);
    req.add_path(path);

    Envelope e = Envelope();
    req.add_to_envelope(&e);

    OutstandingRequest info = OutstandingRequest();
    info.data = data;
    info.callbacks = cb;

    return this->SendRequest(e, info);
}

bool Client::SubscribeEnvelopes(const string &path, SubscriptionCallbackInfo &cb, void *data)
{
    // TODO validate path

    protocol::request::SubscribeEnvelopes req = protocol::request::SubscribeEnvelopes();
    req.set_version(1);
    req.add_path(path);
    Envelope e = Envelope();
    req.add_to_envelope(&e);

    OutstandingRequest info = OutstandingRequest();
    info.data = data;
    info.callbacks = cb;

    return this->SendRequest(e, info);
}

bool Client::SubscribeEnvelopes(const string &path, const string &lua, SubscriptionCallbackInfo &cb, void *data)
{
    // TODO validate

    protocol::request::SubscribeEnvelopes req = protocol::request::SubscribeEnvelopes();
    req.set_version(1);
    req.add_path(path);
    req.set_lua_code(lua);
    Envelope e = Envelope();
    req.add_to_envelope(&e);

    OutstandingRequest info = OutstandingRequest();
    info.data = data;
    info.callbacks = cb;

    return this->SendRequest(e, info);
}


bool Client::SendRequest(Envelope &e, OutstandingRequest &req)
{
    platform::UUID uuid;
    platform::CreateUUID(uuid);

    string id = string((const char *)&uuid, sizeof(uuid));

    e.envelope.add_tag(id);
    req.id = id;

    if (!zeromq::send_envelope_xreq(this->client_sock, e)) {
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
    e.envelope.add_tag(id);
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

    // TODO this can be done with fewer system calls
    do {
        // we wait up to 25 in each iteration
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
        case protocol::response::SubscriptionStart::zippylog_enumeration:
        {
            protocol::response::SubscriptionStart *start =
                (protocol::response::SubscriptionStart *)e.GetMessage(0);

            if (!this->ValidateSubscriptionStart(*start)) {
                return false;
            }

            messages.pop_back();
            return this->HandleSubscriptionResponse(e, *start, messages);
            break;
        }

        case protocol::StoreInfo::zippylog_enumeration:
        case protocol::response::StreamSegmentStart::zippylog_enumeration:
        case protocol::response::SubscribeAck::zippylog_enumeration:
            return this->HandleRequestResponse(e, messages);
            break;

        default:
            // TODO log here or something
            break;
    }

    return false;
}

bool Client::ValidateSubscriptionStart(SubscriptionStart &start)
{
    if (!start.has_id()) return false;

    return this->HasSubscription(start.id());
}

bool Client::HasSubscription(const string &id)
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

bool Client::CancelSubscription(const string &id)
{
    return false;
}

bool Client::HandleSubscriptionResponse(Envelope &e, SubscriptionStart &start, vector<message_t *> &messages)
{
    map<string, Subscription>::iterator iter = this->subscriptions.find(start.id());
    // subscription could have disappeared since it was validated
    if (iter == this->subscriptions.end()) return false;

    SubscriptionCallbackInfo cb = iter->second.cb;

    for (int i = 1; i < e.MessageCount(); i++) {
        switch (e.MessageType(i)) {
            case protocol::StoreChangeBucketAdded::zippylog_enumeration:
                if (cb.BucketAdded) {
                    protocol::StoreChangeBucketAdded *added = (protocol::StoreChangeBucketAdded *)e.GetMessage(i);
                    cb.BucketAdded(start.id(), *added, iter->second.data);
                }
                break;
            case protocol::StoreChangeBucketDeleted::zippylog_enumeration:
                if (cb.BucketDeleted) {
                    protocol::StoreChangeBucketDeleted *deleted = (protocol::StoreChangeBucketDeleted *)e.GetMessage(i);
                    cb.BucketDeleted(start.id(), *deleted, iter->second.data);
                }
                break;

            case protocol::StoreChangeStreamSetAdded::zippylog_enumeration:
                if (cb.StreamSetAdded) {
                    protocol::StoreChangeStreamSetAdded *added = (protocol::StoreChangeStreamSetAdded *)e.GetMessage(i);
                    cb.StreamSetAdded(start.id(), *added, iter->second.data);
                }
                break;

            case protocol::StoreChangeStreamSetDeleted::zippylog_enumeration:
                if (cb.StreamSetDeleted) {
                    protocol::StoreChangeStreamSetDeleted *deleted = (protocol::StoreChangeStreamSetDeleted *)e.GetMessage(i);
                    cb.StreamSetDeleted(start.id(), *deleted, iter->second.data);
                }
                break;

            case protocol::StoreChangeStreamAdded::zippylog_enumeration:
                if (cb.StreamAdded) {
                    protocol::StoreChangeStreamAdded *added = (protocol::StoreChangeStreamAdded *)e.GetMessage(i);
                    cb.StreamAdded(start.id(), *added, iter->second.data);
                }
                break;

            case protocol::StoreChangeStreamDeleted::zippylog_enumeration:
                if (cb.StreamDeleted) {
                    protocol::StoreChangeStreamDeleted *deleted = (protocol::StoreChangeStreamDeleted *)e.GetMessage(i);
                    cb.StreamDeleted(start.id(), *deleted, iter->second.data);
                }
                break;

            case protocol::StoreChangeStreamAppended::zippylog_enumeration:
                if (cb.StreamAppended) {
                    protocol::StoreChangeStreamAppended *appended = (protocol::StoreChangeStreamAppended *)e.GetMessage(i);
                    cb.StreamAppended(start.id(), *appended, iter->second.data);
                }
                break;

            default:
                break;
        }
    }

    if (!cb.Envelope) return true;

    // for now, assume additional messages envelopes that were streamed
    for (size_t i = 0; i < messages.size(); i++) {
        Envelope env = Envelope(messages[i]->data(), messages[i]->size());

        cb.Envelope(start.id(), env, iter->second.data);
    }

    return true;
}

bool Client::HandleRequestResponse(Envelope &e, vector<message_t *> &messages)
{
    // all requests are tagged with a request identifier, so ignore any responses
    // not for a known tag

    if (!e.envelope.tag_size()) {
        throw Exception("received response with no tag to match to request");
    }

    string id = e.envelope.tag(0);

    // try to find the outstanding request
    map<string, OutstandingRequest>::iterator iter = this->outstanding.find(id);

    // this is most weird
    // TODO is an exception always correct? what about a client that restarts
    // with same identity?
    if (iter == this->outstanding.end()) {
        throw Exception("received a response to an unknown outstanding request");
    }

    OutstandingRequest req = iter->second;

    // at this point, the outstanding request entry isn't consulted, so we
    // dispose of it
    this->outstanding.erase(iter);

    switch (e.MessageType(0)) {
        case protocol::StoreInfo::zippylog_enumeration:
        {
            assert(req.cb_store_info);
            protocol::StoreInfo *info = (protocol::StoreInfo *)e.GetMessage(0);

            req.cb_store_info(*info, req.data);

            return true;
        }

        case protocol::StreamInfo::zippylog_enumeration:
        {
            assert(req.cb_stream_info);
            protocol::StreamInfo *info = (protocol::StreamInfo *)e.GetMessage(0);

            req.cb_stream_info(*info, req.data);

            return true;
        }

        case protocol::response::StreamSegmentStart::zippylog_enumeration:
        {
            assert(req.cb_stream_segment);

            // TODO more robust parsing
            protocol::response::StreamSegmentStart *start =
                (protocol::response::StreamSegmentStart *)e.GetMessage(0);

            StreamSegment segment;
            segment.SetPath(start->path());
            segment.SetStartOffset(start->offset());

            void * start_addr = (void *)((const char *)(messages[messages.size()-1]->data()) + 1);

            Envelope footer(start_addr, messages[messages.size()-1]->size() - 1);

            protocol::response::StreamSegmentEnd *end =
                (protocol::response::StreamSegmentEnd *)footer.GetMessage(0);

            segment.SetEndOffset(end->offset());
            segment.SetBytesSent(end->bytes_sent());
            segment.SetEnvelopesSent(end->envelopes_sent());

            for (size_t i = 1; i < messages.size() - 2; i++) {
                start_addr = (void *)((const char *)(messages[i]->data()) + 1);
                Envelope payload(start_addr, messages[i]->size() - 1);
                segment.AddEnvelope(payload);
            }

            req.cb_stream_segment(start->path(), start->offset(), segment, req.data);

            return true;
        }

        case protocol::response::SubscribeAck::zippylog_enumeration:
        {
            protocol::response::SubscribeAck *ack =
                (protocol::response::SubscribeAck *)e.GetMessage(0);

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

            return true;
        }
    }

    return false;
}

bool Client::RenewSubscriptions(bool force)
{
    map<string, Subscription>::iterator iter = this->subscriptions.begin();

    bool result = true;

    for (; iter != this->subscriptions.end(); iter++) {
        if (!force && !iter->second.expiration_timer.Signaled()) continue;

        // reset the timer
        iter->second.expiration_timer.Start();

        protocol::request::SubscribeKeepalive msg = protocol::request::SubscribeKeepalive();
        msg.set_id(iter->first);

        Envelope e = Envelope();
        msg.add_to_envelope(&e);

        if (!zeromq::send_envelope_xreq(this->client_sock, e)) {
            result = false;
        }
    }

    return result;
}

bool Client::HaveOutstandingRequest(string &id)
{
    map<string, OutstandingRequest>::iterator iter = this->outstanding.find(id);

    return iter != this->outstanding.end();
}


StreamSegment::StreamSegment()
{

}

StreamSegment::~StreamSegment()
{

}

bool StreamSegment::CopyFrom(const StreamSegment &orig)
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
