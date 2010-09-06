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

#include <pblog/client.hpp>

#include <pblog/envelope.hpp>
#include <pblog/protocol/request.pb.h>
#include <pblog/protocol/response.pb.h>

namespace pblog {
namespace client {

Client::Client(context_t *ctx, string connect)
{
    this->_sock = new socket_t(*ctx, ZMQ_REQ);
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

    int64 more;
    size_t moresz = sizeof(more);
    message_t res;

    while (true) {
        bool result = this->_sock->recv(&res, 0);
        if (!result) {
            printf("error receiving\n");
            break;
        }

        printf("received message with %d bytes\n", res.size());
        this->_sock->getsockopt(ZMQ_RCVMORE, &more, &moresz);

        if (!more) break;
    }

    Envelope e2 = Envelope(&res);
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

bool Client::ReceiveAndProcessGet(StreamSegment &segment)
{
    Envelope header = Envelope();
    if (!this->ReadEnvelope(header)) {
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

bool Client::HasMore()
{
    int64 more;
    size_t moresz = sizeof(more);

    this->_sock->getsockopt(ZMQ_RCVMORE, &more, &moresz);
    return more > 0;
}

bool Client::ReadEnvelope(pblog::Envelope &envelope)
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

bool Client::_send_envelope(::pblog::Envelope &envelope)
{
    string buffer;
    envelope.envelope.SerializeToString(&buffer);
    message_t *req = new message_t(buffer.length());
    memcpy(req->data(), (void *)buffer.c_str(), buffer.length());
    return this->_sock->send(*req, 0);
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

}} // namespaces
