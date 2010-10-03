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

#include <zippylog/streamer.hpp>
#include <zippylog/protocol/request.pb.h>
#include <zippylog/protocol/response.pb.h>
#include <zippylog/zippylogd.pb.h>
#include <zippylog/zeromq.hpp>

using ::zippylog::protocol::response::SubscribeAck;
using ::zippylog::zippylogd::StreamerStartup;
using ::zippylog::zippylogd::StreamerShutdown;
using ::zippylog::zippylogd::StreamerSubscriptionExpired;
using ::zippylog::zippylogd::StreamerReceiveKeepalive;
using ::zippylog::zippylogd::StreamerRejectKeepaliveUnknownSubscription;
using ::zippylog::zippylogd::StreamerSubscriptionRenewedFromKeepalive;
using ::zippylog::zippylogd::StreamerErrorRenewingSubscription;
using ::zmq::message_t;

#define LOG_MESSAGE(msgvar, socketvar) { \
    msgvar.set_id(this->id); \
    Envelope logenvelope = Envelope(); \
    msgvar.add_to_envelope(&logenvelope); \
    zeromq::send_envelope(socketvar, logenvelope); \
}

namespace zippylog {
namespace server {

StoreChangeSubscription::StoreChangeSubscription()
{

}

SubscriptionInfo::SubscriptionInfo()
{
}

SubscriptionInfo::SubscriptionInfo(uint32 expiration_ttl)
{
    this->expiration_timer = Timer(expiration_ttl * 1000000);
    if (!this->expiration_timer.Start()) {
        throw "could not start expiration timer";
    }
}

Streamer::Streamer(Store *store,
                   context_t *zctx,
                   const string store_changes_endpoint,
                   const string client_endpoint,
                   const string subscriptions_endpoint,
                   const string subscription_updates_endpoint,
                   const string logging_endpoint,
                   uint32 subscription_ttl)
{
    this->store = store;
    this->zctx = zctx;
    this->store_changes_endpoint = store_changes_endpoint;
    this->client_endpoint = client_endpoint;
    this->subscriptions_endpoint = subscriptions_endpoint;
    this->subscription_updates_endpoint = subscription_updates_endpoint;
    this->logging_endpoint = logging_endpoint;
    this->subscription_ttl = subscription_ttl;

    this->changes_sock = NULL;
    this->client_sock = NULL;
    this->subscriptions_sock = NULL;
    this->subscription_updates_sock = NULL;
    this->logging_sock = NULL;

    platform::UUID uuid;
    if (!platform::CreateUUID(uuid)) {
        throw "could not create UUID";
    }

    this->id = string((const char *)&uuid, sizeof(uuid));
}

Streamer::~Streamer()
{
    if (this->changes_sock) delete this->changes_sock;
    if (this->client_sock) delete this->client_sock;
    if (this->subscriptions_sock) delete this->subscriptions_sock;
    if (this->subscription_updates_sock) delete this->subscription_updates_sock;
    if (this->logging_sock) delete this->logging_sock;
}

void Streamer::SetShutdownSemaphore(bool *active)
{
    if (!active) throw "pointer must not be NULL";

    if (!*active) throw "boolean being pointed to must be true";

    this->active = active;
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
        int rc = zmq::poll(&pollitems[0], 3, 250000);

        // process subscription updates first
        if (pollitems[2].revents & ZMQ_POLLIN) {
            if (!this->subscription_updates_sock->recv(&msg, 0)) {
                throw "weird";
            }

            Envelope e = Envelope(&msg);
            assert(e.number_messages() == 1);
            assert(e.message_namespace(0) == 1);

            uint32 type = e.message_type(0);

            if (type == protocol::request::SubscribeKeepalive::zippylog_enumeration) {
                protocol::request::SubscribeKeepalive *m =
                    (protocol::request::SubscribeKeepalive *)e.get_message(0);

                string id = m->id();
                delete m;

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
        map<string, SubscriptionInfo>::iterator iter = this->subscriptions.begin();
        for (; iter != this->subscriptions.end(); iter++) {
            if (iter->second.expiration_timer.Signaled()) {
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
            assert(e.number_messages() == 1);
            assert(e.message_namespace(0) == 1);
            assert(e.message_type(0) == protocol::request::SubscribeStoreChanges::zippylog_enumeration);

            protocol::request::SubscribeStoreChanges *m = (protocol::request::SubscribeStoreChanges *)e.get_message(0);

            StoreChangeSubscription subscription;

            for (int i = 0; i < m->path_size(); i++) {
                subscription.paths.push_back(m->path(i));
            }

            delete m;

            subscription.socket_identifiers = identities;

            // TODO create subscription identity properly
            platform::UUID uuid;
            if (!platform::CreateUUID(uuid)) {
                throw "could not create UUID";
            }
            subscription.id = string((const char *)&uuid, sizeof(uuid));

            this->store_change_subscriptions.push_back(subscription);

            SubscriptionInfo info = SubscriptionInfo(this->subscription_ttl);

            this->subscriptions[subscription.id] = info;

            // send ACK response to client
            SubscribeAck ack = SubscribeAck();
            ack.set_id(subscription.id);
            ack.set_ttl(this->subscription_ttl / 1000);
            Envelope response = Envelope();
            ack.add_to_envelope(&response);


            if (!zeromq::send_envelope(this->client_sock, identities, response)) {
                // TODO log error here
                assert(0);
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
            if (!this->store_change_subscriptions.size()) {
                continue;
            }

            Envelope e = Envelope(&msg);

            assert(e.number_messages());
            // TODO magic constant
            assert(e.message_namespace(0) == 1);

            uint32 message_type = e.message_type(0);
            switch (message_type) {
                case protocol::StoreChangeBucketAdded::zippylog_enumeration:
                case protocol::StoreChangeBucketDeleted::zippylog_enumeration:
                case protocol::StoreChangeStreamSetAdded::zippylog_enumeration:
                case protocol::StoreChangeStreamSetDeleted::zippylog_enumeration:
                case protocol::StoreChangeStreamAppended::zippylog_enumeration:
                case protocol::StoreChangeStreamAdded::zippylog_enumeration:
                case protocol::StoreChangeStreamDeleted::zippylog_enumeration:
                    // if no subscriptions to store changes, do nothing
                    if (!this->store_change_subscriptions.size()) break;

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

void Streamer::ProcessStoreChangeEnvelope(Envelope &e)
{
    // we obtain the full path and build a path from it
    // we then compare path prefixes of subscribers to see who gets it
    string bucket, stream_set, stream;

    switch (e.message_type(0)) {
        case protocol::StoreChangeBucketAdded::zippylog_enumeration:
        {
            protocol::StoreChangeBucketAdded *m = (protocol::StoreChangeBucketAdded *)e.get_message(0);
            bucket = m->bucket();
            delete m;
        }
            break;

        case protocol::StoreChangeBucketDeleted::zippylog_enumeration:
        {
            protocol::StoreChangeBucketDeleted *m = (protocol::StoreChangeBucketDeleted *)e.get_message(0);
            bucket = m->bucket();
            delete m;
        }
            break;

        case protocol::StoreChangeStreamSetAdded::zippylog_enumeration:
        {
            protocol::StoreChangeStreamSetAdded *m = (protocol::StoreChangeStreamSetAdded *)e.get_message(0);
            bucket = m->bucket();
            stream_set = m->stream_set();
            delete m;
        }
            break;

        case protocol::StoreChangeStreamSetDeleted::zippylog_enumeration:
        {
            protocol::StoreChangeStreamSetDeleted *m = (protocol::StoreChangeStreamSetDeleted *)e.get_message(0);
            bucket = m->bucket();
            stream_set = m->stream_set();
            delete m;
        }
            break;

        case protocol::StoreChangeStreamAppended::zippylog_enumeration:
        {
            protocol::StoreChangeStreamAppended *m = (protocol::StoreChangeStreamAppended *)e.get_message(0);
            bucket = m->bucket();
            stream_set = m->stream_set();
            stream = m->stream();
            delete m;
        }
            break;

        case protocol::StoreChangeStreamAdded::zippylog_enumeration:
        {
            protocol::StoreChangeStreamAdded *m = (protocol::StoreChangeStreamAdded *)e.get_message(0);
            bucket = m->bucket();
            stream_set = m->stream_set();
            stream = m->stream();
            delete m;
        }
            break;

        case protocol::StoreChangeStreamDeleted::zippylog_enumeration:
        {
            protocol::StoreChangeStreamDeleted *m = (protocol::StoreChangeStreamDeleted *)e.get_message(0);
            bucket = m->bucket();
            stream_set = m->stream_set();
            stream = m->stream();
            delete m;
        }
            break;
    }

    string path;
    if (!stream.empty()) {
        path = Store::StreamPath(bucket, stream_set, stream);
    }
    else if (!stream_set.empty()) {
        path = Store::StreamsetPath(bucket, stream_set);
    }
    else {
        path = Store::BucketPath(bucket);
    }

    // iterate over subscribers
    vector<StoreChangeSubscription>::iterator i;
    for (i = this->store_change_subscriptions.begin(); i != this->store_change_subscriptions.end(); i++) {

        // iterate over paths they are subscribed to
        vector<string>::iterator prefix;
        for (prefix = i->paths.begin(); prefix != i->paths.end(); i++) {
            // no way that will match
            if (prefix->length() > path.length()) continue;

            // if the subscribed prefix doesn't match the changed prefix
            if (path.substr(0, prefix->length()).compare(*prefix)) continue;

            // at this point, they must be subscribed, so we send them the event

            Envelope response = Envelope();
            protocol::response::SubscriptionStart start = protocol::response::SubscriptionStart();
            start.set_id(i->id);
            start.add_to_envelope(&response);

            if (!e.CopyMessage(0, response)) {
                throw "could not copy message to response envelope. weird";
            }

            zeromq::send_envelope(this->client_sock, i->socket_identifiers, response);

            // don't process this path any more for this subscriber
            break;
        }
    }
}

bool Streamer::HasSubscription(const string &id)
{
    map<string, SubscriptionInfo>::iterator iter = this->subscriptions.find(id);

    return iter != this->subscriptions.end();
}

bool Streamer::RenewSubscription(const string &id)
{
    map<string, SubscriptionInfo>::iterator iter = this->subscriptions.find(id);

    if (iter == this->subscriptions.end()) {
        return false;
    }

    return iter->second.expiration_timer.Start();
}

}} // namespaces
