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
using ::zippylog::lua::LuaState;

namespace zippylog {

PersistedStateManagerSubscriptionRecord::PersistedStateManagerSubscriptionRecord() { }

PersistedStateManagerSubscriptionRecord::PersistedStateManagerSubscriptionRecord(SubscriptionInfo const &subscription, uint32 ttl) :
    si(subscription),
    expiration_timer(ttl),
    l(NULL)
{ }

PersistedStateManagerSubscriptionRecord::~PersistedStateManagerSubscriptionRecord()
{
    if (this->l) delete this->l;
}

PersistedStateManager::PersistedStateManager(PersistedStateManagerStartParams const &params) :
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
    map<string, PersistedStateManagerSubscriptionRecord *>::iterator i = this->subscriptions.begin();
    for (; i != this->subscriptions.end(); ++i) {
        if (i->second) delete i->second;
        i->second = NULL;
    }

    if (this->store) delete this->store;
}

int32 PersistedStateManager::RemoveExpiredSubscriptions()
{
    int32 removed = 0;

    map<string, PersistedStateManagerSubscriptionRecord *>::iterator iter = this->subscriptions.begin();
    for (; iter != this->subscriptions.end(); iter++) {
        if (iter->second->expiration_timer.Signaled()) {
            this->subscriptions.erase(iter->first);
            removed++;
        }
    }

    return removed;
}

bool PersistedStateManager::HaveStoreChangeSubscriptions(std::string const &path) const
{
    map<string, PersistedStateManagerSubscriptionRecord *>::const_iterator i = this->subscriptions.begin();
    for (; i != this->subscriptions.end(); i++) {
        if (i->second->si.type != STORE_CHANGE) continue;

        if (!IsPathSubscribed(path, *i->second)) continue;

        return true;
    }

    return false;
}

bool PersistedStateManager::HaveStoreChangeSubscriptions() const
{
    map<string, PersistedStateManagerSubscriptionRecord *>::const_iterator i = this->subscriptions.begin();
    map<string, PersistedStateManagerSubscriptionRecord *>::const_iterator end = this->subscriptions.end();
    for (; i != end; ++i) {
        if (i->second->si.type == STORE_CHANGE)
            return true;
    }

    return false;
}

bool PersistedStateManager::HaveEnvelopeSubscription(string const &path) const
{
    map<string, PersistedStateManagerSubscriptionRecord *>::const_iterator i = this->subscriptions.begin();
    for (; i != this->subscriptions.end(); i++) {
        if (i->second->si.type != ENVELOPE) continue;

        if (!IsPathSubscribed(path, *i->second)) continue;

        return true;
    }

    return false;
}

bool PersistedStateManager::HasSubscription(string const &id) const
{
    map<string, PersistedStateManagerSubscriptionRecord *>::const_iterator iter = this->subscriptions.find(id);

    return iter != this->subscriptions.end();
}

void PersistedStateManager::RegisterSubscription(SubscriptionInfo const &subscription)
{
    if (subscription.id.empty())
        throw invalid_argument("subscription must have id defined");

    if (this->HasSubscription(subscription.id)) {
        throw Exception("subscription with that ID is already registered");
    }

    PersistedStateManagerSubscriptionRecord *record = new PersistedStateManagerSubscriptionRecord(subscription, this->subscription_ttl);

    this->subscriptions[subscription.id] = record;

    record->expiration_timer.Start(this->subscription_ttl);
}

bool PersistedStateManager::RenewSubscription(string const &id)
{
    vector<string> ids;
    ids.push_back(id);

    return this->RenewSubscriptions(ids);
}

bool PersistedStateManager::RenewSubscriptions(vector<string> const &ids)
{
    // we optimize for the case where the managed subscriptions set is larger
    // than the input list
    map<string, PersistedStateManagerSubscriptionRecord *>::iterator i = this->subscriptions.begin();
    map<string, PersistedStateManagerSubscriptionRecord *>::iterator end = this->subscriptions.end();

    for (; i != end; ++i) {
        vector<string>::const_iterator id = ids.begin();
        vector<string>::const_iterator id_end = ids.end();

        for (; id != id_end; ++id) {
            if (i->first == *id) {
                i->second->expiration_timer.Start(this->subscription_ttl);
                break;
            }
        }
    }

    return true;
}

void PersistedStateManager::ProcessStoreChangePathAdded(std::string const &path, PersistedStateManagerPathAddedCallback *cb, void *data)
{
    if (!cb) throw invalid_argument("callback parameter not defined");

    // remember we have this path
    if (Store::IsStreamPath(path)) {
        this->stream_read_offsets[path] = 0;
    }

    if (this->subscriptions.empty()) return;

    map<string, PersistedStateManagerSubscriptionRecord *>::iterator i = this->subscriptions.begin();
    for (; i != this->subscriptions.end(); i++) {
        if (i->second->si.type != STORE_CHANGE)
            continue;

        if (!IsPathSubscribed(path, *i->second))
            continue;

        /// @todo call callback
    }
}

void PersistedStateManager::ProcessStoreChangePathDeleted(std::string const &path, PersistedStateManagerPathDeletedCallback *cb, void *data)
{
    if (!cb) throw invalid_argument("callback parameter not defined");

    /// @todo should we remove traces of this path from internal state?

    if (this->subscriptions.empty()) return;

    map<string, PersistedStateManagerSubscriptionRecord *>::iterator i = this->subscriptions.begin();
    for (; i != this->subscriptions.end(); i++) {
        if (i->second->si.type != STORE_CHANGE)
            continue;

        if (!IsPathSubscribed(path, *i->second))
            continue;

        /// @todo call callback
    }
}

void PersistedStateManager::ProcessStoreChangeStreamAppended(string const &path, uint64 stream_length, PersistedStateManagerStreamAppendedCallback *cb, void *data)
{
    if (!cb) throw invalid_argument("callback not defined");

    this->stream_read_offsets[path] = stream_length;

    if (this->subscriptions.empty()) return;

    InputStream *is = this->store->GetInputStream(path);
    if (!is) {
        /// @todo log error somehow
        return;
    }

    map<string, PersistedStateManagerSubscriptionRecord *>::iterator i = this->subscriptions.begin();
    for (; i != this->subscriptions.end(); i++) {
        if (i->second->si.type != ENVELOPE)
            continue;

        if (!IsPathSubscribed(path, *i->second))
            continue;

        map<string, uint64>::iterator iter = this->stream_read_offsets.find(path);
        if (iter == this->stream_read_offsets.end()) {
            throw Exception("unknown stream encountered");
        }

        uint64 offset = iter->second;

        if (!is->SetAbsoluteOffset(offset)) {
            throw Exception("could not set stream offset");
        }

        EnvelopeSubscriptionResponseState state(i->second->si);

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
                    cb(i->second->si, state, data);
                    break;
                }

                if (filter_result.return_type != lua::EnvelopeFilterResult::BOOLTRUE)
                    continue;
            }

            state.AddEnvelope(env);
            cb(i->second->si, state, data);
        }
    }
}

bool PersistedStateManager::IsPathSubscribed(std::string const &path, PersistedStateManagerSubscriptionRecord const &subscription)
{
    vector<string>::const_iterator prefix = subscription.si.paths.begin();
    for (; prefix != subscription.si.paths.end(); prefix++) {
        if (prefix->length() > path.length())
                continue;

        // subscribed path doesn't match actual
        if (path.substr(0, prefix->length()).compare(*prefix))
            continue;

        return true;
    }

    return false;
}

} // namespace