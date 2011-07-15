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

bool PersistedStateManager::HaveStoreChangeSubscriptions(const std::string &path) const
{
    map<string, SubscriptionInfo *>::const_iterator i = this->subscriptions.begin();
    for (; i != this->subscriptions.end(); i++) {
        if (i->second->type != i->second->STORE_CHANGE) continue;

        if (!IsPathSubscribed(path, *i->second)) continue;

        return true;
    }

    return false;
}

bool PersistedStateManager::HaveStoreChangeSubscriptions() const
{
    return this->HaveStoreChangeSubscriptions("/");
}

bool PersistedStateManager::HaveEnvelopeSubscription(const string &path) const
{
    map<string, SubscriptionInfo *>::const_iterator i = this->subscriptions.begin();
    for (; i != this->subscriptions.end(); i++) {
        if (i->second->type != i->second->ENVELOPE) continue;

        if (!IsPathSubscribed(path, *i->second)) continue;

        return true;
    }

    return false;
}

bool PersistedStateManager::HasSubscription(const string &id) const
{
    map<string, SubscriptionInfo *>::const_iterator iter = this->subscriptions.find(id);

    return iter != this->subscriptions.end();
}

void PersistedStateManager::RegisterSubscription(zippylog::SubscriptionInfo *subscription)
{
    if (!subscription) throw invalid_argument("subscription must be defined");

    if (this->HasSubscription(subscription->id)) {
        throw Exception("subscription with that ID is already registered");
    }

    this->subscriptions[subscription->id] = subscription;
}

bool PersistedStateManager::RenewSubscription(const string &id)
{
    vector<string> ids;
    ids.push_back(id);

    return this->RenewSubscriptions(ids);
}

bool PersistedStateManager::RenewSubscriptions(const vector<string> &ids)
{
    for (size_t i = 0; i < ids.size(); i++) {
        string id = ids[i];

        map<string, SubscriptionInfo *>::iterator iter = this->subscriptions.find(id);

        if (iter == this->subscriptions.end()) {
            continue;
        }

        iter->second->expiration_timer.Start();
    }

    return true;
}

void PersistedStateManager::ProcessStoreChangePathAdded(const std::string &path, PersistedStateManagerPathAddedCallback *cb, void *data)
{
    if (!cb) throw invalid_argument("callback parameter not defined");

    // remember we have this path
    if (Store::IsStreamPath(path)) {
        this->stream_read_offsets[path] = 0;
    }

    if (this->subscriptions.empty()) return;

    map<string, SubscriptionInfo *>::iterator i = this->subscriptions.begin();
    for (; i != this->subscriptions.end(); i++) {
        if (i->second->type != i->second->STORE_CHANGE)
            continue;

        if (!IsPathSubscribed(path, *i->second))
            continue;

        /// @todo call callback
    }
}

void PersistedStateManager::ProcessStoreChangePathDeleted(const std::string &path, PersistedStateManagerPathDeletedCallback *cb, void *data)
{
    if (!cb) throw invalid_argument("callback parameter not defined");

    /// @todo should we remove traces of this path from internal state?

    if (this->subscriptions.empty()) return;

    map<string, SubscriptionInfo *>::iterator i = this->subscriptions.begin();
    for (; i != this->subscriptions.end(); i++) {
        if (i->second->type != i->second->STORE_CHANGE)
            continue;

        if (!IsPathSubscribed(path, *i->second))
            continue;

        /// @todo call callback
    }
}

void PersistedStateManager::ProcessStoreChangeStreamAppended(const string &path, uint64 stream_length, PersistedStateManagerStreamAppendedCallback *cb, void *data)
{
    if (!cb) throw invalid_argument("callback not defined");

    this->stream_read_offsets[path] = stream_length;

    if (this->subscriptions.empty()) return;

    InputStream *is = this->store->GetInputStream(path);
    if (!is) {
        /// @todo log error somehow
        return;
    }

    map<string, SubscriptionInfo *>::iterator i = this->subscriptions.begin();
    for (; i != this->subscriptions.end(); i++) {
        if (i->second->type != i->second->ENVELOPE)
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

        EnvelopeSubscriptionResponseState state(*i->second);

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
                    cb(*i->second, state, data);
                    break;
                }

                if (filter_result.return_type != lua::EnvelopeFilterResult::BOOLTRUE)
                    continue;
            }

            state.AddEnvelope(env);
            cb(*i->second, state, data);
        }
    }
}

bool PersistedStateManager::IsPathSubscribed(const std::string &path, const zippylog::SubscriptionInfo &subscription)
{
    vector<string>::const_iterator prefix = subscription.paths.begin();
    for (; prefix != subscription.paths.end(); prefix++) {
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