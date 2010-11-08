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

#include <zippylog/zippylogd/streamer.hpp>
#include <zippylog/protocol/request.pb.h>
#include <zippylog/protocol/response.pb.h>
#include <zippylog/zippylogd.pb.h>
#include <zippylog/zeromq.hpp>

#define LOG_MESSAGE(msgvar, socketvar) { \
    msgvar.set_id(this->id); \
    Envelope logenvelope = Envelope(); \
    msgvar.add_to_envelope(&logenvelope); \
    zeromq::send_envelope(socketvar, logenvelope); \
}

namespace zippylog {
namespace zippylogd {

using ::zippylog::lua::LuaState;
using ::zippylog::protocol::response::SubscribeAck;
using ::zippylog::zippylogd::StreamerStartup;
using ::zippylog::zippylogd::StreamerShutdown;
using ::zippylog::zippylogd::StreamerSubscriptionExpired;
using ::zippylog::zippylogd::StreamerReceiveKeepalive;
using ::zippylog::zippylogd::StreamerRejectKeepaliveUnknownSubscription;
using ::zippylog::zippylogd::StreamerSubscriptionRenewedFromKeepalive;
using ::zippylog::zippylogd::StreamerErrorRenewingSubscription;
using ::zmq::message_t;

EnvelopeSubscription::EnvelopeSubscription() {}
SubscriptionInfo::SubscriptionInfo() : l(NULL) {}

SubscriptionInfo::SubscriptionInfo(uint32 expiration_ttl)
    : l(NULL)
{
    // milliseconds to microseconds
    if (!this->expiration_timer.Start(expiration_ttl * 1000)) {
        throw "could not start expiration timer";
    }
}

SubscriptionInfo::~SubscriptionInfo()
{
    if (this->l) delete this->l;
}

StreamerStartParams::StreamerStartParams() : active(NULL) { }

Streamer::Streamer(StreamerStartParams params) :
    changes_sock(NULL),
    client_sock(NULL),
    subscriptions_sock(NULL),
    subscription_updates_sock(NULL),
    logging_sock(NULL)
{
    this->store = params.store;
    this->zctx = params.ctx;
    this->store_changes_endpoint = params.store_changes_endpoint;
    this->client_endpoint = params.client_endpoint;
    this->subscriptions_endpoint = params.subscriptions_endpoint;
    this->subscription_updates_endpoint = params.subscription_updates_endpoint;
    this->logging_endpoint = params.logging_endpoint;
    this->subscription_ttl = params.subscription_ttl;
    this->lua_allow = params.lua_allow;
    this->lua_max_memory = params.lua_max_memory;

    if (!params.active) throw "active parameter cannot be NULL";

    this->active = params.active;

    platform::UUID uuid;
    platform::CreateUUID(uuid);

    this->id = string((const char *)&uuid, sizeof(uuid));

    // populate stream offsets with current values
    vector<string> streams;
    this->store->StreamPaths(streams);

    for (vector<string>::iterator i = streams.begin(); i != streams.end(); i++) {
        int64 length;
        if (!this->store->StreamLength(*i, length)) continue;

        this->stream_read_offsets[*i] = length;
    }
}

Streamer::Streamer(const Streamer &orig)
{
    throw "copy constructor not implemented for Streamer";
}

Streamer & Streamer::operator=(const Streamer &orig)
{
    throw "assignment operator not available for Streamer";
}

Streamer::~Streamer()
{
    map<string, SubscriptionInfo *>::iterator i = this->subscriptions.begin();
    for (; i != this->subscriptions.end(); i++) {
        delete i->second;
    }
    this->subscriptions.clear();

    if (this->changes_sock) delete this->changes_sock;
    if (this->client_sock) delete this->client_sock;
    if (this->subscriptions_sock) delete this->subscriptions_sock;
    if (this->subscription_updates_sock) delete this->subscription_updates_sock;
    if (this->logging_sock) delete this->logging_sock;
}

void Streamer::Run()
{
    this->logging_sock = new socket_t(*this->zctx, ZMQ_PUSH);
    this->logging_sock->connect(this->logging_endpoint.c_str());

    {
        StreamerStartup log = StreamerStartup();
        LOG_MESSAGE(log, this->logging_sock);
    }

    // subscribe to store change notifications
    this->changes_sock = new socket_t(*this->zctx, ZMQ_SUB);
    this->changes_sock->setsockopt(ZMQ_SUBSCRIBE, NULL, 0);
    this->changes_sock->connect(this->store_changes_endpoint.c_str());

    // establish sending socket
    this->client_sock = new socket_t(*this->zctx, ZMQ_PUSH);
    this->client_sock->connect(this->client_endpoint.c_str());

    // receive client subscriptions
    this->subscriptions_sock = new socket_t(*this->zctx, ZMQ_PULL);
    this->subscriptions_sock->connect(this->subscriptions_endpoint.c_str());

    // receive client updates
    this->subscription_updates_sock = new socket_t(*this->zctx, ZMQ_SUB);
    this->subscription_updates_sock->connect(this->subscription_updates_endpoint.c_str());
    this->subscription_updates_sock->setsockopt(ZMQ_SUBSCRIBE, "", 0);

    zmq::pollitem_t pollitems[3];
    pollitems[0].events = ZMQ_POLLIN;
    pollitems[0].socket = *this->changes_sock;
    pollitems[0].fd = 0;
    pollitems[0].revents = 0;

    pollitems[1].events = ZMQ_POLLIN;
    pollitems[1].socket = *this->subscriptions_sock;
    pollitems[1].fd = 0;
    pollitems[1].revents = 0;

    pollitems[2].events = ZMQ_POLLIN;
    pollitems[2].socket = *this->subscription_updates_sock;
    pollitems[2].fd = 0;
    pollitems[2].revents = 0;

    while (*this->active) {
        zmq::message_t msg;

        // wait for a message to process
        int rc = zmq::poll(&pollitems[0], 3, 100000);

        // process subscription updates first
        if (pollitems[2].revents & ZMQ_POLLIN) {
            if (!this->subscription_updates_sock->recv(&msg, 0)) {
                throw "weird";
            }

            Envelope e = Envelope(&msg);
            assert(e.MessageCount() == 1);
            assert(e.MessageNamespace(0) == 1);

            uint32 type = e.MessageType(0);

            if (type == protocol::request::SubscribeKeepalive::zippylog_enumeration) {
                protocol::request::SubscribeKeepalive *m =
                    (protocol::request::SubscribeKeepalive *)e.GetMessage(0);

                string id = m->id();

                StreamerReceiveKeepalive log = StreamerReceiveKeepalive();
                log.set_subscription(id);
                LOG_MESSAGE(log, this->logging_sock);

                if (this->HasSubscription(id)) {
                    if (this->RenewSubscription(id)) {
                        StreamerSubscriptionRenewedFromKeepalive log = StreamerSubscriptionRenewedFromKeepalive();
                        log.set_subscription(id);
                        LOG_MESSAGE(log, this->logging_sock);
                    }
                    else {
                        StreamerErrorRenewingSubscription log = StreamerErrorRenewingSubscription();
                        log.set_subscription(id);
                        LOG_MESSAGE(log, this->logging_sock);
                    }
                }
                else {
                    StreamerRejectKeepaliveUnknownSubscription log = StreamerRejectKeepaliveUnknownSubscription();
                    log.set_subscription(id);
                    LOG_MESSAGE(log, this->logging_sock);
                }
            }
        }

        // unsubscribe any expired subscribers
        map<string, SubscriptionInfo *>::iterator iter = this->subscriptions.begin();
        for (; iter != this->subscriptions.end(); iter++) {
            if (iter->second->expiration_timer.Signaled()) {
                StreamerSubscriptionExpired log = StreamerSubscriptionExpired();
                log.set_subscription(iter->first);
                LOG_MESSAGE(log, this->logging_sock);
                this->subscriptions.erase(iter);
                break;
            }
        }

        // process new subscriptions
        if (pollitems[1].revents & ZMQ_POLLIN) {
            vector<string> identities;
            vector<message_t *> msgs;

            if (!zeromq::receive_multipart_message(this->subscriptions_sock, identities, msgs)) {
                // TODO log error here
                continue;
            }

            assert(msgs.size() > 0);

            Envelope e = Envelope(msgs[0]);
            assert(e.MessageCount() == 1);
            assert(e.MessageNamespace(0) == 1);

            uint32 message_type = e.MessageType(0);

            switch (message_type) {
                case protocol::request::SubscribeStoreChanges::zippylog_enumeration:
                    this->ProcessSubscribeStoreChanges(e, identities, msgs);
                    break;

                case protocol::request::SubscribeEnvelopes::zippylog_enumeration:
                    this->ProcessSubscribeEnvelopes(e, identities, msgs);
                    break;

                default:
                    // TODO log here
                    break;
            }

            for (vector<message_t *>::iterator msg = msgs.begin(); msg != msgs.end(); msg++) {
                delete *msg;
            }
            msgs.clear();

        }

        // process store changes and send to subscribers
        if (pollitems[0].revents & ZMQ_POLLIN) {
            this->changes_sock->recv(&msg, 0);

            // if we don't have any subscriptions, do nothing
            if (!this->subscriptions.size()) continue;

            Envelope e = Envelope(&msg);

            assert(e.MessageCount());
            // TODO magic constant
            assert(e.MessageNamespace(0) == 1);

            uint32 message_type = e.MessageType(0);
            switch (message_type) {
                case protocol::StoreChangeBucketAdded::zippylog_enumeration:
                case protocol::StoreChangeBucketDeleted::zippylog_enumeration:
                case protocol::StoreChangeStreamSetAdded::zippylog_enumeration:
                case protocol::StoreChangeStreamSetDeleted::zippylog_enumeration:
                case protocol::StoreChangeStreamAppended::zippylog_enumeration:
                case protocol::StoreChangeStreamAdded::zippylog_enumeration:
                case protocol::StoreChangeStreamDeleted::zippylog_enumeration:
                    // if no subscriptions to store changes, do nothing
                    if (!this->HaveStoreChangeSubscriptions()) break;

                    this->ProcessStoreChangeEnvelope(e);
                    break;
                default:
                    // WTF mate?
                    break;
            }
        }
    }

    StreamerShutdown log = StreamerShutdown();
    LOG_MESSAGE(log, this->logging_sock);
}

void Streamer::ProcessSubscribeStoreChanges(Envelope &e, vector<string> &identities, vector<message_t *> &msgs)
{
    protocol::request::SubscribeStoreChanges *m =
        (protocol::request::SubscribeStoreChanges *)e.GetMessage(0);

    SubscriptionInfo * subscription = new SubscriptionInfo(this->subscription_ttl);
    subscription->type = SubscriptionInfo::STORE_CHANGE;

    for (int i = 0; i < m->path_size(); i++) {
        subscription->paths.push_back(m->path(i));
    }

    subscription->socket_identifiers = identities;

    platform::UUID uuid;
    platform::CreateUUID(uuid);
    string id = string((const char *)&uuid, sizeof(uuid));

    this->subscriptions[id] = subscription;

    this->SendSubscriptionAck(id, e, identities);
}

void Streamer::ProcessSubscribeEnvelopes(Envelope &e, vector<string> &identities, vector<message_t *> &msgs)
{
    protocol::request::SubscribeEnvelopes *m =
        (protocol::request::SubscribeEnvelopes *)e.GetMessage(0);

    platform::UUID uuid;
    platform::CreateUUID(uuid);
    string id = string((const char *)&uuid, sizeof(uuid));

    SubscriptionInfo *subscription = new SubscriptionInfo(this->subscription_ttl);
    subscription->type = subscription->ENVELOPE;

    for (int i = 0; i < m->path_size(); i++) {
        subscription->paths.push_back(m->path(i));
    }

    subscription->socket_identifiers = identities;

    if (m->has_lua_code()) {
        subscription->l = new LuaState();
        subscription->l->SetMemoryCeiling(this->lua_max_memory);

        if (!subscription->l->LoadLuaCode(m->lua_code())) {
            delete subscription;

            // TODO send error response instead
            throw "error loading user-supplied code";
        }
    }

    subscription->envelope_subscription = EnvelopeSubscription();
    this->subscriptions[id] = subscription;

    this->SendSubscriptionAck(id, e, identities);
}

void Streamer::ProcessStoreChangeEnvelope(Envelope &e)
{
    // we obtain the full path and build a path from it
    // we then compare path prefixes of subscribers to see who gets it
    string bucket, stream_set, stream;

    bool process_envelopes = false;
    uint64 stream_length = 0;

    string path;

    switch (e.MessageType(0)) {
        case protocol::StoreChangeBucketAdded::zippylog_enumeration:
        {
            protocol::StoreChangeBucketAdded *m = (protocol::StoreChangeBucketAdded *)e.GetMessage(0);
            bucket = m->bucket();

            path = Store::BucketPath(bucket);
        }
            break;

        case protocol::StoreChangeBucketDeleted::zippylog_enumeration:
        {
            protocol::StoreChangeBucketDeleted *m = (protocol::StoreChangeBucketDeleted *)e.GetMessage(0);
            bucket = m->bucket();

            path = Store::BucketPath(bucket);
        }
            break;

        case protocol::StoreChangeStreamSetAdded::zippylog_enumeration:
        {
            protocol::StoreChangeStreamSetAdded *m = (protocol::StoreChangeStreamSetAdded *)e.GetMessage(0);
            bucket = m->bucket();
            stream_set = m->stream_set();

            path = Store::StreamsetPath(bucket, stream_set);
        }
            break;

        case protocol::StoreChangeStreamSetDeleted::zippylog_enumeration:
        {
            protocol::StoreChangeStreamSetDeleted *m = (protocol::StoreChangeStreamSetDeleted *)e.GetMessage(0);
            bucket = m->bucket();
            stream_set = m->stream_set();

            path = Store::StreamsetPath(bucket, stream_set);
        }
            break;

        case protocol::StoreChangeStreamAppended::zippylog_enumeration:
        {
            protocol::StoreChangeStreamAppended *m = (protocol::StoreChangeStreamAppended *)e.GetMessage(0);
            bucket = m->bucket();
            stream_set = m->stream_set();
            stream = m->stream();
            stream_length = m->length();

            process_envelopes = true;
            path = Store::StreamPath(bucket, stream_set, stream);
        }
            break;

        case protocol::StoreChangeStreamAdded::zippylog_enumeration:
        {
            protocol::StoreChangeStreamAdded *m = (protocol::StoreChangeStreamAdded *)e.GetMessage(0);
            bucket = m->bucket();
            stream_set = m->stream_set();
            stream = m->stream();

            path = Store::StreamPath(bucket, stream_set, stream);

            this->stream_read_offsets[path] = 0;
        }
            break;

        case protocol::StoreChangeStreamDeleted::zippylog_enumeration:
        {
            protocol::StoreChangeStreamDeleted *m = (protocol::StoreChangeStreamDeleted *)e.GetMessage(0);
            bucket = m->bucket();
            stream_set = m->stream_set();
            stream = m->stream();

            path = Store::StreamPath(bucket, stream_set, stream);
        }
            break;
    }

    // pre-load an input stream if we need to
    InputStream is;
    if (process_envelopes && this->HaveEnvelopeSubscription(path)) {
        if (!this->store->GetInputStream(path, is)) {
            throw "could not obtain input stream";
            return;
        }
    }

    // iterate over all the subscribers
    map<string, SubscriptionInfo *>::iterator i = this->subscriptions.begin();
    for (; i != this->subscriptions.end(); i++) {
        // for each path they are subscribed to
        vector<string>::iterator prefix = i->second->paths.begin();
        for (; prefix != i->second->paths.end(); prefix++) {
            // no way it will match
            if (prefix->length() > path.length()) continue;

            // if prefix doesn't match, move on
            if (path.substr(0, prefix->length()).compare(*prefix)) continue;

            // at this point, the subscription matches

            // the case of store changes is simple
            if (i->second->type == i->second->STORE_CHANGE) {
                Envelope response = Envelope();
                protocol::response::SubscriptionStart start = protocol::response::SubscriptionStart();
                start.set_id(i->first);
                start.add_to_envelope(&response);

                if (!e.CopyMessage(0, response)) {
                    throw "could not copy message to response envelope. weird";
                }

                zeromq::send_envelope(this->client_sock, i->second->socket_identifiers, response);

                // don't process this path any more for this subscriber
                break;
            }
            // envelopes are a little more challenging
            else if (process_envelopes && i->second->type == i->second->ENVELOPE) {
                map<string, uint64>::iterator iter = this->stream_read_offsets.find(path);
                assert(iter != this->stream_read_offsets.end());

                uint64 offset = iter->second;

                is.Seek(offset);

                Envelope response = Envelope();
                protocol::response::SubscriptionStart start = protocol::response::SubscriptionStart();
                start.set_id(i->first);
                start.add_to_envelope(&response);

                zeromq::send_envelope_more(this->client_sock, i->second->socket_identifiers, response);

                while (offset < stream_length) {
                    Envelope env;
                    uint32 read;
                    if (!is.ReadEnvelope(env, read)) {
                        break;
                    }
                    offset += read;

                    // run envelope through Lua
                    if (i->second->l) {
                        if (!i->second->l->HasEnvelopeFilter()) {
                            zeromq::send_envelope_more(this->client_sock, env);
                            continue;
                        }

                        // else

                    }
                    else {
                        zeromq::send_envelope_more(this->client_sock, env);
                    }
                }

                message_t empty(0);
                this->client_sock->send(empty);

                // stop processing this subscription since the stream has been addressed
                break;
            }
        }
    }

    if (process_envelopes) {
        this->stream_read_offsets[path] = stream_length;
    }
}

bool Streamer::HaveEnvelopeSubscription(const string &path)
{
    map<string, SubscriptionInfo *>::iterator i = this->subscriptions.begin();
    for (; i != this->subscriptions.end(); i++) {
        if (i->second->type != i->second->ENVELOPE) continue;

        vector<string>::iterator prefix = i->second->paths.begin();
        for (; prefix != i->second->paths.end(); prefix++) {
            if (prefix->length() > path.length()) continue;
            if (path.substr(0, prefix->length()).compare(*prefix)) continue;

            return true;
        }
    }

    return false;
}

bool Streamer::HaveStoreChangeSubscriptions()
{
    return this->HaveStoreChangeSubscriptions("/");
}

bool Streamer::HaveStoreChangeSubscriptions(const string &path)
{
    map<string, SubscriptionInfo *>::iterator i = this->subscriptions.begin();
    for (; i != this->subscriptions.end(); i++) {
        if (i->second->type != i->second->STORE_CHANGE) continue;

        vector<string>::iterator prefix = i->second->paths.begin();
        for (; prefix != i->second->paths.end(); prefix++) {
            if (prefix->length() > path.length()) continue;
            if (path.substr(0, prefix->length()).compare(*prefix)) continue;

            return true;
        }
    }

    return false;
}

void Streamer::SendSubscriptionAck(const string &id, Envelope &e, vector<string> &identities)
{
    SubscribeAck ack = SubscribeAck();
    ack.set_id(id);
    ack.set_ttl(this->subscription_ttl);
    Envelope response = Envelope();

    // copy tags to response because that's what the protocol does
    for (size_t i = 0; i < e.envelope.tag_size(); i++) {
        response.envelope.add_tag(e.envelope.tag(i));
    }

    ack.add_to_envelope(&response);

    if (!zeromq::send_envelope(this->client_sock, identities, response)) {
        // TODO log error here
        assert(0);
    }
}

bool Streamer::HasSubscription(const string &id)
{
    map<string, SubscriptionInfo *>::iterator iter = this->subscriptions.find(id);

    return iter != this->subscriptions.end();
}

bool Streamer::RenewSubscription(const string &id)
{
    map<string, SubscriptionInfo *>::iterator iter = this->subscriptions.find(id);

    if (iter == this->subscriptions.end()) {
        return false;
    }

    return iter->second->expiration_timer.Start();
}

}} // namespaces
