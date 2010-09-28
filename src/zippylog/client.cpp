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

#include <zippylog/client.hpp>

#include <zippylog/envelope.hpp>
#include <zippylog/protocol/request.pb.h>
#include <zippylog/protocol/response.pb.h>
#include <zippylog/zeromq.hpp>

namespace zippylog {
namespace client {

Client::Client(context_t *ctx, string connect)
{
    this->_sock = new socket_t(*ctx, ZMQ_XREQ);
    this->_sock->setsockopt(ZMQ_IDENTITY, "client", strlen("client"));
    this->_sock->connect(connect.c_str());
}

Client::~Client()
{
    delete this->_sock;
}

bool Client::StoreInfo(protocol::StoreInfo &info)
{
    Envelope e = Envelope();
    protocol::request::StoreInfo req = protocol::request::StoreInfo();
    req.add_to_envelope(&e);

    this->_send_envelope(e);

    Envelope e2 = Envelope();
    this->ReadFirstEnvelope(e2);
    Message *m = e2.get_message(0);

    info = *((protocol::StoreInfo *)m);

    return true;
}

bool Client::Get(const string path, uint64 start_offset, StreamSegment &segment)
{
    protocol::request::Get req = protocol::request::Get();
    req.set_path(path);
    req.set_start_offset(start_offset);
    Envelope e = Envelope();
    req.add_to_envelope(&e);

    if (!this->_send_envelope(e)) return false;

    return ReceiveAndProcessGet(segment);

    return false;
}

bool Client::Get(const string path, uint64 start_offset, uint32 max_response_bytes, StreamSegment &segment)
{
    protocol::request::Get req = protocol::request::Get();
    req.set_path(path);
    req.set_start_offset(start_offset);
    req.set_max_response_bytes(max_response_bytes);
    Envelope e = Envelope();
    req.add_to_envelope(&e);

    if (!this->_send_envelope(e)) return false;

    return ReceiveAndProcessGet(segment);

    return false;
}

bool Client::Get(const string path, uint64 start_offset, uint64 stop_offset, StreamSegment &segment)
{
    return Get(path, start_offset, (uint32)(stop_offset - start_offset), segment);
}

bool Client::SubscribeStoreChanges(const string path, SubscriptionCallback &cb)
{
    protocol::request::SubscribeStoreChanges req = protocol::request::SubscribeStoreChanges();
    req.add_path(path);

    Envelope e = Envelope();
    req.add_to_envelope(&e);

    if (!this->_send_envelope(e)) return false;

    Envelope response = Envelope();
    this->ReadFirstEnvelope(response);

    assert(response.message_namespace(0) == protocol::response::SubscribeAck::zippylog_namespace);
    assert(response.message_type(0) == protocol::response::SubscribeAck::zippylog_enumeration);

    protocol::response::SubscribeAck *ack = (protocol::response::SubscribeAck *)response.get_message(0);
    this->subscriptions[ack->id()] = cb;
    delete ack;

    return true;
}

bool Client::ReceiveAndProcessGet(StreamSegment &segment)
{
    Envelope header = Envelope();
    if (!this->ReadFirstEnvelope(header)) {
        this->ReadOutMultipart();
        return false;
    }

    // TODO more robust parsing
    protocol::response::StreamSegmentStart *start =
        (protocol::response::StreamSegmentStart *)header.get_message(0);

    segment.SetPath(start->path());
    segment.SetStartOffset(start->offset());

    vector<Envelope> envelopes;
    while (this->HasMore()) {
        Envelope e = Envelope();
        if (!this->ReadEnvelope(e)) {
            this->ReadOutMultipart();
            return false;
        }

        envelopes.push_back(e);
    }

    Envelope footer = envelopes[envelopes.size()-1];
    envelopes.pop_back();

    protocol::response::StreamSegmentEnd *end =
        (protocol::response::StreamSegmentEnd *)footer.get_message(0);

    segment.SetEndOffset(end->offset());
    segment.SetBytesSent(end->bytes_sent());
    segment.SetEnvelopesSent(end->envelopes_sent());

    for (size_t i = 0; i < envelopes.size(); i++) {
        segment.AddEnvelope(envelopes[i]);
    }

    return true;
}

bool Client::WaitAndProcessMessage()
{
    Envelope e = Envelope();
    if (!this->ReadFirstEnvelope(e)) return false;

    if (!e.number_messages()) return false;

    if (e.message_namespace(0) == 1) {
        // if this is in response to a subscription
        if (e.message_type(0) == protocol::response::SubscriptionStart::zippylog_enumeration) {
            protocol::response::SubscriptionStart *start = (protocol::response::SubscriptionStart *)e.get_message(0);

            SubscriptionCallback cb = this->subscriptions[start->id()];

            for (size_t i = 1; i < e.number_messages(); i++) {
                switch (e.message_type(i)) {
                    case protocol::StoreChangeBucketAdded::zippylog_enumeration:
                        if (cb.BucketAdded) {
                            protocol::StoreChangeBucketAdded *added = (protocol::StoreChangeBucketAdded *)e.get_message(i);
                            cb.BucketAdded(start->id(), *added);
                            delete added;
                        }
                        break;
                    case protocol::StoreChangeBucketDeleted::zippylog_enumeration:
                        if (cb.BucketDeleted) {
                            protocol::StoreChangeBucketDeleted *deleted = (protocol::StoreChangeBucketDeleted *)e.get_message(i);
                            cb.BucketDeleted(start->id(), *deleted);
                            delete deleted;
                        }
                        break;

                    case protocol::StoreChangeStreamSetAdded::zippylog_enumeration:
                        if (cb.StreamSetAdded) {
                            protocol::StoreChangeStreamSetAdded *added = (protocol::StoreChangeStreamSetAdded *)e.get_message(i);
                            cb.StreamSetAdded(start->id(), *added);
                            delete added;
                        }
                        break;

                    case protocol::StoreChangeStreamSetDeleted::zippylog_enumeration:
                        if (cb.StreamSetDeleted) {
                            protocol::StoreChangeStreamSetDeleted *deleted = (protocol::StoreChangeStreamSetDeleted *)e.get_message(i);
                            cb.StreamSetDeleted(start->id(), *deleted);
                            delete deleted;
                        }
                        break;

                    case protocol::StoreChangeStreamAdded::zippylog_enumeration:
                        if (cb.StreamAdded) {
                            protocol::StoreChangeStreamAdded *added = (protocol::StoreChangeStreamAdded *)e.get_message(i);
                            cb.StreamAdded(start->id(), *added);
                            delete added;
                        }
                        break;

                    case protocol::StoreChangeStreamDeleted::zippylog_enumeration:
                        if (cb.StreamDeleted) {
                            protocol::StoreChangeStreamDeleted *deleted = (protocol::StoreChangeStreamDeleted *)e.get_message(i);
                            cb.StreamDeleted(start->id(), *deleted);
                            delete deleted;
                        }
                        break;

                    case protocol::StoreChangeStreamAppended::zippylog_enumeration:
                        if (cb.StreamAppended) {
                            protocol::StoreChangeStreamAppended *appended = (protocol::StoreChangeStreamAppended *)e.get_message(i);
                            cb.StreamAppended(start->id(), *appended);
                            delete appended;
                        }
                        break;

                    default:
                        break;
                }
            }

        }
    }

    return true;
}

bool Client::HasMore()
{
    int64 more;
    size_t moresz = sizeof(more);

    this->_sock->getsockopt(ZMQ_RCVMORE, &more, &moresz);
    return more > 0;
}

bool Client::ReadFirstEnvelope(Envelope &envelope)
{
    message_t empty;
    if (!this->_sock->recv(&empty, 0)) return false;

    assert(empty.size() == 0);

    return this->ReadEnvelope(envelope);
}

bool Client::ReadEnvelope(zippylog::Envelope &envelope)
{
    message_t res;
    if (!this->_sock->recv(&res, 0)) return false;

    return envelope.merge_from_zmq_message(&res);
}

bool Client::ReadOutMultipart()
{
    while (this->HasMore()) {
        Envelope temp = Envelope();
        if (!this->ReadEnvelope(temp)) {
            return false;
        }
    }

    return true;
}

bool Client::_send_envelope(::zippylog::Envelope &envelope)
{
    return zeromq::send_envelope_xreq(this->_sock, envelope);
}

StreamSegment::StreamSegment()
{

}

StreamSegment::~StreamSegment()
{

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

SubscriptionCallback::SubscriptionCallback()
{
    this->BucketAdded = NULL;
    this->BucketDeleted = NULL;
    this->StreamSetAdded = NULL;
    this->StreamSetDeleted = NULL;
    this->StreamAdded = NULL;
    this->StreamDeleted = NULL;
    this->StreamAppended = NULL;
}

}} // namespaces
