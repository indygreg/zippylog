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

#include <zippylog/device/streamer.hpp>
#include <zippylog/device/streamer.pb.h>
#include <zippylog/zeromq.hpp>

#define LOG_MESSAGE(msgvar, socketvar) { \
    msgvar.set_id(this->id); \
    Envelope logenvelope = Envelope(); \
    msgvar.add_to_envelope(&logenvelope); \
    zeromq::send_envelope(socketvar, logenvelope); \
}

using ::std::invalid_argument;
using ::std::map;
using ::std::string;
using ::std::vector;
using ::zippylog::lua::LuaState;
using ::zippylog::protocol::response::Error;
using ::zmq::message_t;
using ::zmq::socket_t;

using namespace ::zippylog::device::streamer;

namespace zippylog {
namespace device {

Streamer::Streamer(StreamerStartParams params) :
    store(NULL),
    zctx(params.ctx),
    changes_sock(NULL),
    client_sock(NULL),
    subscriptions_sock(NULL),
    subscription_updates_sock(NULL),
    logging_sock(NULL)
{
    this->store_changes_endpoint = params.store_changes_endpoint;
    this->client_endpoint = params.client_endpoint;
    this->subscriptions_endpoint = params.subscriptions_endpoint;
    this->subscription_updates_endpoint = params.subscription_updates_endpoint;
    this->logging_endpoint = params.logging_endpoint;
    this->subscription_ttl = params.subscription_ttl;
    this->lua_allow = params.lua_allow;
    this->lua_max_memory = params.lua_max_memory;

    if (!params.active) throw invalid_argument("active parameter cannot be NULL");

    this->active = params.active;

    platform::UUID uuid;
    platform::CreateUUID(uuid);

    this->id = string((const char *)&uuid, sizeof(uuid));

    this->store = Store::CreateStore(params.store_path);

    // populate stream offsets with current values
    vector<string> streams;
    this->store->StreamPaths(streams);

    for (vector<string>::iterator i = streams.begin(); i != streams.end(); i++) {
        int64 length;
        if (!this->store->StreamLength(*i, length)) continue;

        this->stream_read_offsets[*i] = length;
    }
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
    if (this->store) delete this->store;
}

void Streamer::Run()
{
    this->logging_sock = new socket_t(*this->zctx, ZMQ_PUSH);
    this->logging_sock->connect(this->logging_endpoint.c_str());

    {
        Create log;
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
        int result = zmq::poll(&pollitems[0], 3, 100000);

        // if we don't have data, perform house keeping and try again
        if (!result) {
            this->RemoveExpiredSubscriptions();
            continue;
        }

        // process subscription updates first
        if (pollitems[2].revents & ZMQ_POLLIN) {
            if (!this->subscription_updates_sock->recv(&msg, 0)) {
                throw Exception("error receiving 0MQ messages");
            }

            Envelope e = Envelope(msg.data(), msg.size());
            this->ProcessSubscriptionUpdate(e);
        }

        this->RemoveExpiredSubscriptions();

        // process new subscriptions
        if (pollitems[1].revents & ZMQ_POLLIN) {

            if (!this->subscriptions_sock->recv(&msg, 0)) {
                throw Exception("error receiving 0MQ message on subscriptions sock");
            }

            if (msg.size() != sizeof(SubscriptionInfo *)) {
                throw Exception("SubscriptionInfo message not a pointer!");
            }

            SubscriptionInfo **subscription = (SubscriptionInfo **)msg.data();

            this->subscriptions[(*subscription)->id] = *subscription;
        }

        // process store changes and send to subscribers
        if (pollitems[0].revents & ZMQ_POLLIN) {
            // @todo error checking
            this->changes_sock->recv(&msg, 0);

            this->ProcessStoreChangeMessage(msg);
        }
    }

    Destroy log;
    LOG_MESSAGE(log, this->logging_sock);
}

int Streamer::RemoveExpiredSubscriptions()
{
    int removed = 0;

    map<string, SubscriptionInfo *>::iterator iter = this->subscriptions.begin();
    for (; iter != this->subscriptions.end(); iter++) {
        if (iter->second->expiration_timer.Signaled()) {
            SubscriptionExpired log;
            log.set_subscription(iter->first);
            LOG_MESSAGE(log, this->logging_sock);
            this->subscriptions.erase(iter->first);
            removed++;
        }
    }

    return removed;
}

bool Streamer::ProcessSubscriptionUpdate(Envelope &e)
{
    if (e.MessageCount() != 1) return false;
    if (e.MessageNamespace(0) != ::zippylog::message_namespace) return false;

    uint32 type = e.MessageType(0);

    if (type == protocol::request::SubscribeKeepaliveV1::zippylog_enumeration) {
        protocol::request::SubscribeKeepaliveV1 *m =
            (protocol::request::SubscribeKeepaliveV1 *)e.GetMessage(0);
        if (!m) return false;

        for (int i = 0; i < m->id_size(); i++) {
            string id = m->id(i);

            ReceiveKeepalive log;
            log.set_subscription(id);
            LOG_MESSAGE(log, this->logging_sock);

            if (this->HasSubscription(id)) {
                if (this->RenewSubscription(id)) {
                    SubscriptionRenewedFromKeepalive log;
                    log.set_subscription(id);
                    LOG_MESSAGE(log, this->logging_sock);
                }
                else {
                    ErrorRenewingSubscription log;
                    log.set_subscription(id);
                    LOG_MESSAGE(log, this->logging_sock);
                }
            }
            else {
                RejectKeepaliveUnknownSubscription log;
                log.set_subscription(id);
                LOG_MESSAGE(log, this->logging_sock);
            }
        }
    }

    return true;
}

bool Streamer::ProcessStoreChangeMessage(message_t &msg)
{
    // if we don't have any subscriptions, do nothing
    if (!this->subscriptions.size()) return true;

    Envelope e;
    try { e = Envelope(msg.data(), msg.size()); }
    catch (DeserializeException ex) {
        throw Exception("TODO log deserialize error and continue");
    }

    if (!e.MessageCount()) return false;
    if (e.MessageNamespace(0) != ::zippylog::message_namespace) return false;

    uint32 message_type = e.MessageType(0);
    switch (message_type) {
        case protocol::StoreChangeBucketAddedV1::zippylog_enumeration:
        case protocol::StoreChangeBucketDeletedV1::zippylog_enumeration:
        case protocol::StoreChangeStreamSetAddedV1::zippylog_enumeration:
        case protocol::StoreChangeStreamSetDeletedV1::zippylog_enumeration:
        case protocol::StoreChangeStreamAppendedV1::zippylog_enumeration:
        case protocol::StoreChangeStreamAddedV1::zippylog_enumeration:
        case protocol::StoreChangeStreamDeletedV1::zippylog_enumeration:
            // if no subscriptions to store changes, do nothing
            if (!this->HaveStoreChangeSubscriptions()) return true;

            this->ProcessStoreChangeEnvelope(e);
            break;
        default:
            throw Exception("TODO log unknown store change message in streamer");
            break;
    }

    return true;
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
        case protocol::StoreChangeBucketAddedV1::zippylog_enumeration:
        {
            protocol::StoreChangeBucketAddedV1 *m = (protocol::StoreChangeBucketAddedV1 *)e.GetMessage(0);
            bucket = m->bucket();

            path = Store::BucketPath(bucket);
        }
            break;

        case protocol::StoreChangeBucketDeletedV1::zippylog_enumeration:
        {
            protocol::StoreChangeBucketDeletedV1 *m = (protocol::StoreChangeBucketDeletedV1 *)e.GetMessage(0);
            bucket = m->bucket();

            path = Store::BucketPath(bucket);
        }
            break;

        case protocol::StoreChangeStreamSetAddedV1::zippylog_enumeration:
        {
            protocol::StoreChangeStreamSetAddedV1 *m = (protocol::StoreChangeStreamSetAddedV1 *)e.GetMessage(0);
            bucket = m->bucket();
            stream_set = m->stream_set();

            path = Store::StreamsetPath(bucket, stream_set);
        }
            break;

        case protocol::StoreChangeStreamSetDeletedV1::zippylog_enumeration:
        {
            protocol::StoreChangeStreamSetDeletedV1 *m = (protocol::StoreChangeStreamSetDeletedV1 *)e.GetMessage(0);
            bucket = m->bucket();
            stream_set = m->stream_set();

            path = Store::StreamsetPath(bucket, stream_set);
        }
            break;

        case protocol::StoreChangeStreamAppendedV1::zippylog_enumeration:
        {
            protocol::StoreChangeStreamAppendedV1 *m = (protocol::StoreChangeStreamAppendedV1 *)e.GetMessage(0);
            bucket = m->bucket();
            stream_set = m->stream_set();
            stream = m->stream();
            stream_length = m->length();

            process_envelopes = true;
            path = Store::StreamPath(bucket, stream_set, stream);
        }
            break;

        case protocol::StoreChangeStreamAddedV1::zippylog_enumeration:
        {
            protocol::StoreChangeStreamAddedV1 *m = (protocol::StoreChangeStreamAddedV1 *)e.GetMessage(0);
            bucket = m->bucket();
            stream_set = m->stream_set();
            stream = m->stream();

            path = Store::StreamPath(bucket, stream_set, stream);

            this->stream_read_offsets[path] = 0;
        }
            break;

        case protocol::StoreChangeStreamDeletedV1::zippylog_enumeration:
        {
            protocol::StoreChangeStreamDeletedV1 *m = (protocol::StoreChangeStreamDeletedV1 *)e.GetMessage(0);
            bucket = m->bucket();
            stream_set = m->stream_set();
            stream = m->stream();

            path = Store::StreamPath(bucket, stream_set, stream);
        }
            break;
    }

    // pre-load an input stream if we need to
    InputStream *is = NULL;
    if (process_envelopes && this->HaveEnvelopeSubscription(path)) {
        is = this->store->GetInputStream(path);
        if (!is) {
            throw Exception("could not obtain input stream");
            return;
        }
    }

    // iterate over all the subscribers
    // @todo address runaway resource consumption when many subscriptions
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
                RequestProcessor::SendSubscriptionStoreChangeResponse(
                    *this->client_sock,
                    *i->second,
                    e
                );

                // don't process this path any more for this subscriber
                break;
            }
            // envelopes are a little more challenging
            else if (process_envelopes && i->second->type == i->second->ENVELOPE) {
                map<string, uint64>::iterator iter = this->stream_read_offsets.find(path);
                assert(iter != this->stream_read_offsets.end());

                uint64 offset = iter->second;

                if (!is->SetAbsoluteOffset(offset)) {
                    throw Exception("could not set stream offset");
                }

                EnvelopeSubscriptionResponseState state(this->client_sock, *i->second);

                while (offset < stream_length) {
                    Envelope env;
                    uint32 read;
                    if (!is->ReadEnvelope(env, read)) {
                        break;
                    }
                    offset += read;

                    // run envelope through Lua
                    if (i->second->l && i->second->l->HasSubscriptionEnvelopeFilter()) {
                        lua::EnvelopeFilterResult filter_result;
                        if (!i->second->l->ExecuteSubscriptionEnvelopeFilter(env, path, filter_result)) {
                            throw Exception("envelope filter not executed. very weird");
                        }

                        if (!filter_result.execution_success) {
                            state.RegisterError(protocol::response::LUA_ERROR, filter_result.lua_error);
                            RequestProcessor::SendSubscriptionEnvelopeResponse(state);
                            break;
                        }

                        if (filter_result.return_type != lua::EnvelopeFilterResult::BOOLTRUE)
                            continue;
                    }

                    state.AddEnvelope(e);
                    RequestProcessor::SendSubscriptionEnvelopeResponse(state);
                }

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
