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

#include <zippylog/persisted_state_manager.hpp>

#include <vector>

using ::std::invalid_argument;
using ::std::map;
using ::std::string;
using ::std::vector;

namespace zippylog {

PersistedStateManager::PersistedStateManager(const PersistedStateManagerStartParams &params) :
    store_uri(params.store_uri),
    subscription_ttl(params.subscription_ttl),
    subscription_lua_allow(params.subscription_lua_allow),
    subscription_lua_memory_ceiling(params.subscription_lua_memory_ceiling),
    store(NULL)
{
    if (this->store_uri.empty()) throw invalid_argument("store URI must be defined");

    this->store = Store::CreateStore(this->store_uri);

    // populate stream offsets with current values
    vector<string> streams;
    this->store->StreamPaths(streams);

    for (vector<string>::iterator i = streams.begin(); i != streams.end(); i++) {
        int64 length;
        if (!this->store->StreamLength(*i, length)) continue;

        this->stream_read_offsets[*i] = length;
    }
}

PersistedStateManager::~PersistedStateManager()
{
    map<string, SubscriptionInfo *>::iterator i = this->subscriptions.begin();
    for (; i != this->subscriptions.end(); i++) {
        delete i->second;
    }
    this->subscriptions.clear();

    if (this->store) delete this->store;
}

int32 PersistedStateManager::RemoveExpiredSubscriptions()
{
    int32 removed = 0;

    map<string, SubscriptionInfo *>::iterator iter = this->subscriptions.begin();
    for (; iter != this->subscriptions.end(); iter++) {
        if (iter->second->expiration_timer.Signaled()) {
            this->subscriptions.erase(iter->first);
            removed++;
        }
    }

    return removed;
}

bool PersistedStateManager::HaveStoreChangeSubscriptions(const std::string &path)
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

bool PersistedStateManager::HaveStoreChangeSubscriptions()
{
    return this->HaveStoreChangeSubscriptions("/");
}

bool PersistedStateManager::HaveEnvelopeSubscription(const string &path)
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

bool PersistedStateManager::HasSubscription(const string &id)
{
    map<string, SubscriptionInfo *>::iterator iter = this->subscriptions.find(id);

    return iter != this->subscriptions.end();
}

bool PersistedStateManager::RenewSubscription(const string &id)
{
    map<string, SubscriptionInfo *>::iterator iter = this->subscriptions.find(id);

    if (iter == this->subscriptions.end()) {
        return false;
    }

    return iter->second->expiration_timer.Start();
}

void PersistedStateManager::RegisterSubscription(zippylog::SubscriptionInfo *subscription)
{
    if (!subscription) throw invalid_argument("subscription must be defined");

    if (this->HasSubscription(subscription->id)) {
        throw Exception("subscription with that ID is already registered");
    }

    this->subscriptions[subscription->id] = subscription;
}

/*
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

*/


} // namespace