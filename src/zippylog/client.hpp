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

#ifndef ZIPPYLOG_CLIENT_HPP_
#define ZIPPYLOG_CLIENT_HPP_

#include <zippylog/zippylog.h>
#include <zippylog/envelope.hpp>
#include <zippylog/protocol.pb.h>

#include <map>
#include <string>
#include <vector>
#include <zmq.hpp>

namespace zippylog {
namespace client {

using protocol::StoreInfo;
using ::std::map;
using ::std::string;
using ::std::vector;
using ::zmq::socket_t;
using ::zmq::context_t;
using ::zippylog::Envelope;

// represents a stream segment response (from Get requests)
class ZIPPYLOG_EXPORT StreamSegment {
    public:
        StreamSegment();
        ~StreamSegment();

        bool SetPath(const string path);
        bool SetStartOffset(uint64 offset);
        bool SetEndOffset(uint64 offset);
        bool SetBytesSent(uint32 count);
        bool SetEnvelopesSent(uint32 number);
        bool AddEnvelope(Envelope e);

        string Path;
        uint64 StartOffset;
        uint64 EndOffset;
        uint32 BytesSent;
        uint32 EnvelopesSent;
        vector<Envelope> Envelopes;

};

typedef void (__stdcall * StoreChangeStreamAddedCallback)(string, protocol::StoreChangeStreamAdded &);
typedef void (__stdcall * StoreChangeStreamDeletedCallback)(string, protocol::StoreChangeStreamDeleted &);
typedef void (__stdcall * StoreChangeStreamAppendedCallback)(string, protocol::StoreChangeStreamAppended &);
typedef void (__stdcall * StoreChangeBucketAddedCallback)(string, protocol::StoreChangeBucketAdded &);
typedef void (__stdcall * StoreChangeBucketDeletedCallback)(string, protocol::StoreChangeBucketDeleted &);
typedef void (__stdcall * StoreChangeStreamSetAddedCallback)(string, protocol::StoreChangeStreamSetAdded &);
typedef void (__stdcall * StoreChangeStreamSetDeletedCallback)(string, protocol::StoreChangeStreamSetDeleted &);

class ZIPPYLOG_EXPORT SubscriptionCallback {
public:
    SubscriptionCallback();


    StoreChangeStreamAddedCallback StreamAdded;
    StoreChangeStreamDeletedCallback StreamDeleted;
    StoreChangeStreamAppendedCallback StreamAppended;
    StoreChangeBucketAddedCallback BucketAdded;
    StoreChangeBucketDeletedCallback BucketDeleted;
    StoreChangeStreamSetAddedCallback StreamSetAdded;
    StoreChangeStreamSetDeletedCallback StreamSetDeleted;
};

class ZIPPYLOG_EXPORT Client {
    public:
        // establish a client and bind to the location specified
        Client(context_t *ctx, string bind);
        ~Client();

        bool StoreInfo(StoreInfo &info);
        bool Get(const string path, uint64 start_offset, StreamSegment &segment);
        bool Get(const string path, uint64 start_offset, uint64 stop_offset, StreamSegment &segment);
        bool Get(const string path, uint64 start_offset, uint32 max_response_bytes, StreamSegment &segment);

        bool SubscribeStoreChanges(const string path, SubscriptionCallback &callback);
        bool WaitAndProcessMessage();

    protected:
        socket_t *_sock;

        bool _send_envelope(Envelope &envelope);
        bool ReadEnvelope(Envelope &envelope);
        bool ReadFirstEnvelope(Envelope &envelope);
        bool ReceiveAndProcessGet(StreamSegment &segment);
        bool HasMore();
        bool ReadOutMultipart();

        map<string, SubscriptionCallback> subscriptions;
};

}} // namespaces

#endif