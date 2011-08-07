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

#include <zippylog/device/server.hpp>

#include <zippylog/zeromq.hpp>

namespace zippylog {
namespace device {
namespace server {

using ::std::string;
using ::zmq::socket_t;

Watcher::Watcher(WatcherStartParams &params) :
    StoreWatcher(params.params),
    endpoint(params.socket_endpoint),
    socket(NULL)
{
    this->socket = new socket_t(*this->_ctx, ZMQ_PUSH);
    this->socket->connect(this->endpoint.c_str());
}

Watcher::~Watcher()
{
    if (this->socket) delete this->socket;
}

void Watcher::SendChangeMessage(Envelope &e)
{
    zeromq::send_envelope(this->logging_sock, e);
    zeromq::send_envelope(this->socket, e);
}

void Watcher::HandleAdded(string path, platform::FileStat &)
{
    string bucket, set, stream;
    if (!Store::ParsePath(path, bucket, set, stream)) return;

    // @todo need to validate streams are files, buckets/sets are directories

    Envelope e = Envelope();

    if (!stream.empty()) {
        protocol::StoreChangeStreamAddedV1 m;
        m.set_bucket(bucket);
        m.set_stream_set(set);
        m.set_stream(stream);
        m.add_to_envelope(&e);
        this->SendChangeMessage(e);
    }
    else if (!set.empty()) {
        protocol::StoreChangeStreamSetAddedV1 m;
        m.set_bucket(bucket);
        m.set_stream_set(set);
        m.add_to_envelope(&e);
        this->SendChangeMessage(e);
    }
    else {
        protocol::StoreChangeBucketAddedV1 m;
        m.set_bucket(bucket);
        m.add_to_envelope(&e);
        this->SendChangeMessage(e);
    }
}

void Watcher::HandleDeleted(string path)
{
    string bucket, set, stream;
    if (!Store::ParsePath(path, bucket, set, stream)) return;

    Envelope e = Envelope();

    if (!stream.empty()) {
        protocol::StoreChangeStreamDeletedV1 m;
        m.set_bucket(bucket);
        m.set_stream_set(set);
        m.set_stream(stream);
        m.add_to_envelope(&e);
        this->SendChangeMessage(e);
    }
    else if (!set.empty()) {
        protocol::StoreChangeStreamSetDeletedV1 m;
        m.set_bucket(bucket);
        m.set_stream_set(set);
        m.add_to_envelope(&e);
        this->SendChangeMessage(e);
    }
    else {
        protocol::StoreChangeBucketDeletedV1 m;
        m.set_bucket(bucket);
        m.add_to_envelope(&e);
        this->SendChangeMessage(e);
    }
}

void Watcher::HandleModified(string path, platform::FileStat &stat)
{
    string bucket, set, stream;
    if (!Store::ParsePath(path, bucket, set, stream)) return;

    Envelope e = Envelope();

    if (!stream.empty()) {
        protocol::StoreChangeStreamAppendedV1 m;
        m.set_bucket(bucket);
        m.set_stream_set(set);
        m.set_stream(stream);
        m.set_length(stat.size);
        m.add_to_envelope(&e);
        this->SendChangeMessage(e);
    }
    else {
        // what else is there? buckets and stream sets don't really modify, do they?
        throw Exception("how did we get here?");
    }
}

}}} // namespaces
