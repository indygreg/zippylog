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

#include <zippylog/request_processor.hpp>

#include <zippylog/message.pb.h>
#include <zippylog/protocol.pb.h>
#include <zippylog/protocol/request.pb.h>
#include <zippylog/protocol/response.pb.h>
#include <zippylog/request_processor.pb.h>
#include <zippylog/zeromq.hpp>

namespace zippylog {

#define LOG_MESSAGE(msgvar, socketvar) { \
    msgvar.set_id(this->id); \
    Envelope logenvelope = Envelope(); \
    msgvar.add_to_envelope(&logenvelope); \
    zeromq::send_envelope(socketvar, logenvelope); \
}

/// This handy macro obtains the index'th message of an envelope and stores
/// stores it in a variable, varname. If we can't obtain the message, we record
/// an error in the output vector<Envelope> variable and goto LOG_END.
#define OBTAIN_MESSAGE(type, varname, envelope, index) \
    type * varname = (type *)envelope.GetMessage(index); \
    if (!varname) { \
        ::zippylog::request_processor::EnvelopeEmbeddedMessageParseFailure log = ::zippylog::request_processor::EnvelopeEmbeddedMessageParseFailure(); \
        LOG_MESSAGE(log, this->logger_sock); \
        \
        this->PopulateErrorResponse( \
            protocol::response::UNKNOWN_REQUEST_TYPE, \
            "error parsing message in envelope", \
            output \
        ); \
        goto LOG_END; \
    }

using ::std::invalid_argument;
using ::std::string;
using ::std::vector;
using ::zmq::message_t;
using ::zmq::socket_t;

RequestProcessor::RequestProcessor(RequestProcessorStartParams &params) :
    ctx(params.ctx),
    store_path(params.store_path),
    logger_endpoint(params.logger_endpoint),
    client_endpoint(params.client_endpoint),
    logger_sock(NULL),
    socket(NULL),
    store(NULL),
    active(params.active),
    get_stream_max_bytes(params.get_stream_max_bytes),
    get_stream_max_envelopes(params.get_stream_max_envelopes)
{
    if (!this->active) {
        throw invalid_argument("active parameter cannot be NULL");
    }

    if (!this->ctx) {
        throw invalid_argument("ctx parameter cannot be NULL");
    }

    this->store = Store::CreateStore(params.store_path);

    platform::UUID uuid;
    platform::CreateUUID(uuid);
    this->id = string((const char *)&uuid, sizeof(uuid));

    this->logger_sock = new socket_t(*this->ctx, ZMQ_PUSH);
    this->logger_sock->connect(this->logger_endpoint.c_str());

    this->socket = new socket_t(*this->ctx, ZMQ_XREP);
    this->socket->connect(this->client_endpoint.c_str());

    ::zippylog::request_processor::Create log = ::zippylog::request_processor::Create();
    LOG_MESSAGE(log, this->logger_sock);
}

RequestProcessor::~RequestProcessor()
{
    ::zippylog::request_processor::Destroy log = ::zippylog::request_processor::Destroy();
    LOG_MESSAGE(log, this->logger_sock);

    if (this->logger_sock) delete this->logger_sock;
    if (this->socket) delete this->socket;
    if (this->store) delete this->store;
}

void RequestProcessor::Run()
{
    {
        ::zippylog::request_processor::RunStart log;
        LOG_MESSAGE(log, this->logger_sock);
    }

    while (*this->active) {
        this->Pump(250000);
    }

    ::zippylog::request_processor::RunStop log;
    LOG_MESSAGE(log, this->logger_sock);

    return;
}

int RequestProcessor::Pump(long wait)
{
    zmq::pollitem_t pollitems[1];
    pollitems[0].events = ZMQ_POLLIN;
    pollitems[0].fd = 0;
    pollitems[0].revents = 0;
    pollitems[0].socket = *this->socket;

    if (!zmq::poll(&pollitems[0], 1, wait)) {
        return 0;
    }

    // we have a message available, so we receive it
    vector<message_t *> msgs;
    this->current_request_identities.clear();
    if (!zeromq::receive_multipart_message(this->socket, this->current_request_identities, msgs)) {
        ::zippylog::request_processor::FailReceiveMessage log;
        LOG_MESSAGE(log, this->logger_sock);

        // TODO shouldn't this be part of the receive_multipart_message API?
        vector<message_t *>::iterator iter = msgs.begin();
        for (; iter != msgs.end(); iter++) {
            delete *iter;
        }

        // TODO we used to rebuild socket here. is this a good recovery strategy?
        // this exception is here so we can see if this actually happens
        // I suspect it doesn't
        throw Exception("error receiving multipart message from worker sock");
    }

    vector<Envelope> response_envelopes;
    this->ProcessMessages(this->current_request_identities, msgs, response_envelopes);

    // input messages aren't needed any more, so we destroy them immediately
    vector<message_t *>::iterator iter = msgs.begin();
    for (; iter != msgs.end(); iter++) {
        delete *iter;
    }

    if (response_envelopes.size()) {
        int flags = response_envelopes.size() > 1 ? ZMQ_SNDMORE : 0;

        int result = zeromq::SendEnvelope(*this->socket, this->current_request_identities, response_envelopes[0], true, flags);
        if (result == -1) {
            throw Exception("Serialization error in response envelope");
        }
        else if (!result) {
            throw Exception("Send failure for response");
        }

        if (flags) {
            int i = 0;
            vector<Envelope>::iterator itor = response_envelopes.begin()++;
            for (; itor < response_envelopes.end(); itor++) {
                flags = itor == response_envelopes.end() - 1 ? 0 : ZMQ_SNDMORE;

                result = zeromq::SendEnvelope(*this->socket, *itor, true, flags);
                if (result == -1) {
                    throw Exception("Serialization error in response envelope");
                }
                else if (!result) {
                    throw Exception("Send failure for response envelope");
                }
            }
        }
    }

    return 1;
}

void RequestProcessor::ProcessMessages(vector<string> &, vector<message_t *> &input, vector<Envelope> &output)
{
    // always clear the output list so we don't accidentally send leftovers
    output.clear();

    // we need to declare all our variables first b/c we use goto
    char msg_version = 0;
    bool have_request_envelope = false;
    Envelope request_envelope;
    ResponseStatus result;
    bool successful_process = false;

    // TODO should we log?
    if (!input.size()) return;

    // we expect the first message to have an envelope of some kind
    if (!input[0]->size()) {
        ::zippylog::request_processor::ReceiveEmptyMessage log;
        LOG_MESSAGE(log, this->logger_sock);

        this->PopulateErrorResponse(
            protocol::response::EMPTY_MESSAGE,
            "empty 0MQ message received",
            output
        );
        goto CREATE_OUTPUT;
    }

    // the first byte of the message is the message format version
    // we currently only support 1, as it is the only defined version
    memcpy(&msg_version, input[0]->data(), sizeof(msg_version));

    if (msg_version != 0x01) {
        ::zippylog::request_processor::UnknownMessageVersion log;
        LOG_MESSAGE(log, this->logger_sock);

        this->PopulateErrorResponse(
            protocol::response::UNKNOWN_MESSAGE_FORMAT_VERSION,
            "protocol message version not understood. server only supports version 1",
            output
        );
        goto CREATE_OUTPUT;
    }

    // this is where we'd switch based on version
    // we only have version 1 now, so no branching is necessary

    // we expect to see data after the version byte. if we don't, something
    // is wrong
    if (input[0]->size() < 2) {
        this->PopulateErrorResponse(
            protocol::response::PROTOCOL_NO_ENVELOPE,
            "protocol version 1 0MQ message received without an envelope",
            output
        );
        goto CREATE_OUTPUT;
    }

    try {
        request_envelope = Envelope(
            // we can't do math on sizeless void
            (void *)((char *)input[0]->data() + 1),
            input[0]->size() - 1
        );
    } catch ( ... ) {
        // we should only get an exception on parse error
        ::zippylog::request_processor::EnvelopeParseFailure log;
        LOG_MESSAGE(log, this->logger_sock);

        this->PopulateErrorResponse(
            protocol::response::ENVELOPE_PARSE_FAILURE,
            "error parsing envelope",
            output
        );
        goto CREATE_OUTPUT;
    }

    have_request_envelope = true;
    try {
        result = this->ProcessRequest(request_envelope, output);
    // TODO catch various types of exceptions
    } catch ( ... ) {
        this->PopulateErrorResponse(
            protocol::response::GENERAL_ERROR_PROCESSING,
            "exception when processing request",
            output
        );
        goto CREATE_OUTPUT;
    }

    successful_process = true;

    CREATE_OUTPUT:

    // if we couldn't process the request without throwing an exception, we
    // need to handle things ourselves
    if (!successful_process) {
        // if we have a request envelope and there were tags and we have an
        // output envelope, copy the tags over
        if (have_request_envelope && request_envelope.envelope.tag_size() && output.size()) {
            for (int i = 0; i < request_envelope.envelope.tag_size(); i++) {
                output[0].envelope.add_tag(request_envelope.envelope.tag(i));
            }
        }

        // not much more to do
        return;
    }

    if (result == AUTHORITATIVE) {
        assert(output.size());

        return;
    }

    return;
}

RequestProcessor::ResponseStatus RequestProcessor::ProcessRequest(Envelope &request_envelope, vector<Envelope> &output)
{
    RequestProcessor::ResponseStatus result;
    uint32 request_type; // forward declare b/c of goto

    if (request_envelope.envelope.message_size() < 1) {
        ::zippylog::request_processor::EmptyEnvelope log;
        LOG_MESSAGE(log, this->logger_sock);

        this->PopulateErrorResponse(
            protocol::response::EMPTY_ENVELOPE,
            "envelope contains no messages",
            output
        );
        result = AUTHORITATIVE;
        goto SEND_RESPONSE;
    }

    if (request_envelope.envelope.message_namespace_size() < 1 || request_envelope.envelope.message_type_size() < 1) {
        ::zippylog::request_processor::InvalidMessageEnumeration log;
        LOG_MESSAGE(log, this->logger_sock);

        this->PopulateErrorResponse(
            protocol::response::MISSING_ENUMERATIONS,
            "message received without namespace or type enumerations",
            output
        );
        result = AUTHORITATIVE;
        goto SEND_RESPONSE;
    }

    /* must be in the zippylog namespace */
    if (request_envelope.envelope.message_namespace(0) != ::zippylog::message_namespace) {
        ::zippylog::request_processor::InvalidMessageEnumeration log;
        LOG_MESSAGE(log, this->logger_sock);

        this->PopulateErrorResponse(
            protocol::response::INVALID_MESSAGE_NAMESPACE,
            "message namespace is not zippylog's",
            output
        );
        result = AUTHORITATIVE;
        goto SEND_RESPONSE;
    }

    request_type = request_envelope.envelope.message_type(0);
    switch (request_type) {
        case protocol::request::Ping::zippylog_enumeration:
            result = this->ProcessPing(request_envelope, output);
            break;

        case protocol::request::GetFeatures::zippylog_enumeration:
            result = this->ProcessFeatures(request_envelope, output);
            break;

        case protocol::request::GetStoreInfo::zippylog_enumeration:
            result = this->ProcessStoreInfo(request_envelope, output);
            break;

        case protocol::request::GetBucketInfo::zippylog_enumeration:
            result = this->ProcessBucketInfo(request_envelope, output);
            break;

        case protocol::request::GetStreamSetInfo::zippylog_enumeration:
            result = this->ProcessStreamSetInfo(request_envelope, output);
            break;

        case protocol::request::GetStreamInfo::zippylog_enumeration:
            result = this->ProcessStreamInfo(request_envelope, output);
            break;

        case protocol::request::GetStream::zippylog_enumeration:
            result = this->ProcessGetStream(request_envelope, output);
            break;

        case protocol::request::SubscribeStoreChanges::zippylog_enumeration:
            result = this->ProcessSubscribeStoreChanges(request_envelope, output);
            break;

        case protocol::request::SubscribeEnvelopes::zippylog_enumeration:
            result = this->ProcessSubscribeEnvelopes(request_envelope, output);
            break;

        case protocol::request::SubscribeKeepalive::zippylog_enumeration:
            result = this->ProcessSubscribeKeepalive(request_envelope, output);
            break;

        case protocol::request::WriteEnvelope::zippylog_enumeration:
            result = this->ProcessWriteEnvelope(request_envelope, output);
            break;

        default:
        {
            ::zippylog::request_processor::UnknownRequestType log;
            log.set_enumeration(request_type);
            LOG_MESSAGE(log, this->logger_sock);

            this->PopulateErrorResponse(
                protocol::response::UNKNOWN_REQUEST_TYPE,
                "server does not know how to process the request",
                output
            );
            result = AUTHORITATIVE;
            break;
        }
    }

SEND_RESPONSE:
    if (result == AUTHORITATIVE) {
        assert(output.size() > 0);

        // tags associated with request attached to response so client
        // can associate a single response to a request
        if (request_envelope.envelope.tag_size() >= 0) {
            for (int i = 0; i < request_envelope.envelope.tag_size(); i++) {
                output[0].envelope.add_tag(request_envelope.envelope.tag(i));
            }
        }
    }

    return result;
}

RequestProcessor::ResponseStatus RequestProcessor::ProcessPing(Envelope &e, vector<Envelope> &output)
{
    protocol::response::Pong pong;
    Envelope out;
    pong.add_to_envelope(out);
    output.push_back(out);

    return AUTHORITATIVE;
}

RequestProcessor::ResponseStatus RequestProcessor::ProcessFeatures(Envelope &e, vector<Envelope> &output)
{
    protocol::response::FeatureSpecification response;

    // we currently support version 1 only
    response.add_supported_message_version(1);

    Envelope out;
    response.add_to_envelope(out);
    output.push_back(out);

    return AUTHORITATIVE;
}

RequestProcessor::ResponseStatus RequestProcessor::ProcessStoreInfo(Envelope &e, vector<Envelope> &output)
{
    ::zippylog::request_processor::BeginProcessStoreInfo logstart;
    LOG_MESSAGE(logstart, this->logger_sock);

    OBTAIN_MESSAGE(protocol::request::GetStoreInfo, m, e, 0);

    if (!this->CheckMessageVersion(m->version(), 1, output)) return AUTHORITATIVE;

    {
        protocol::StoreInfo info = protocol::StoreInfo();
        this->store->StoreInfo(info);

        Envelope out;
        info.add_to_envelope(&out);
        output.push_back(out);
    }

LOG_END:

    ::zippylog::request_processor::EndProcessStoreInfo logend;
    LOG_MESSAGE(logend, this->logger_sock);

    return AUTHORITATIVE;
}

RequestProcessor::ResponseStatus RequestProcessor::ProcessBucketInfo(Envelope &e, vector<Envelope> &output)
{
    ::zippylog::request_processor::BeginProcessBucketInfo logstart;
    LOG_MESSAGE(logstart, this->logger_sock);

    string bucket;
    protocol::BucketInfo info = protocol::BucketInfo();
    Envelope response;

    OBTAIN_MESSAGE(protocol::request::GetBucketInfo, m, e, 0);

    if (!this->CheckMessageVersion(m->version(), 1, output)) goto LOG_END;

    if (!m->has_path()) {
        this->PopulateErrorResponse(
            protocol::response::EMPTY_FIELD,
            "required field 'path' missing",
            output
        );
        goto LOG_END;
    }

    if (!this->CheckPath(m->path(), output, true)) goto LOG_END;

    Store::ParseBucketPath(m->path(), bucket);
    this->store->BucketInfo(bucket, info);
    info.add_to_envelope(&response);
    output.push_back(response);

LOG_END:

    ::zippylog::request_processor::EndProcessBucketInfo logend;
    LOG_MESSAGE(logend, this->logger_sock);

    return AUTHORITATIVE;
}

RequestProcessor::ResponseStatus RequestProcessor::ProcessStreamSetInfo(Envelope &e, vector<Envelope> &output)
{
    ::zippylog::request_processor::BeginProcessStreamSetInfo logstart;
    LOG_MESSAGE(logstart, this->logger_sock);

    string bucket, set;
    protocol::StreamSetInfo info = protocol::StreamSetInfo();
    Envelope response;

    OBTAIN_MESSAGE(protocol::request::GetStreamSetInfo, m, e, 0);

    if (!this->CheckMessageVersion(m->version(), 1, output)) goto LOG_END;

    if (!m->has_path()) {
        this->PopulateErrorResponse(
            protocol::response::EMPTY_FIELD,
            "required field 'path' missing",
            output
        );
        goto LOG_END;
    }

    if (!this->CheckPath(m->path(), output, true, true)) goto LOG_END;

    Store::ParseStreamSetPath(m->path(), bucket, set);
    this->store->StreamsetInfo(bucket, set, info);
    info.add_to_envelope(&response);
    output.push_back(response);

LOG_END:
    ::zippylog::request_processor::EndProcessStreamSetInfo logend;
    LOG_MESSAGE(logend, this->logger_sock);

    return AUTHORITATIVE;
}

RequestProcessor::ResponseStatus RequestProcessor::ProcessStreamInfo(Envelope &e, vector<Envelope> &output)
{
    ::zippylog::request_processor::BeginProcessStreamInfo logstart;
    LOG_MESSAGE(logstart, this->logger_sock);

    string bucket, set, stream;
    protocol::StreamInfo info = protocol::StreamInfo();
    Envelope response;

    OBTAIN_MESSAGE(protocol::request::GetStreamInfo, m, e, 0);

    if (!this->CheckMessageVersion(m->version(), 1, output)) goto LOG_END;

    if (!m->has_path()) {
        this->PopulateErrorResponse(
            protocol::response::EMPTY_FIELD,
            "required field 'path' missing",
            output
        );
        goto LOG_END;
    }

    if (!this->CheckPath(m->path(), output, true, true, true)) goto LOG_END;

    Store::ParsePath(m->path(), bucket, set, stream);
    this->store->StreamInfo(bucket, set, stream, info);
    info.add_to_envelope(&response);
    output.push_back(response);

LOG_END:

    ::zippylog::request_processor::EndProcessStreamInfo logend;
    LOG_MESSAGE(logend, this->logger_sock);

    return AUTHORITATIVE;
}

RequestProcessor::ResponseStatus RequestProcessor::ProcessGetStream(Envelope &request, vector<Envelope> &output)
{
    {
        ::zippylog::request_processor::BeginProcessGetStream log;
        LOG_MESSAGE(log, this->logger_sock);
    }

    protocol::request::GetStream *get = (protocol::request::GetStream *)request.GetMessage(0);
    if (!get) {
        ::zippylog::request_processor::ReceiveInvalidGet log;
        LOG_MESSAGE(log, this->logger_sock);

        this->PopulateErrorResponse(
            protocol::response::UNKNOWN_REQUEST_TYPE,
            "error parsing get message... weird",
            output
        );
        goto LOG_END;
    }

    if (!this->CheckMessageVersion(get->version(), 1, output)) goto LOG_END;

    if (!get->has_path()) {
        ::zippylog::request_processor::ReceiveInvalidGet log;
        LOG_MESSAGE(log, this->logger_sock);

        this->PopulateErrorResponse(
            protocol::response::EMPTY_FIELD,
            "required field 'path' is not set",
            output
        );
        goto LOG_END;
    }

    if (!this->CheckPath(get->path(), output, true, true, true)) goto LOG_END;

    if (!get->has_start_offset()) {
        ::zippylog::request_processor::ReceiveInvalidGet log;
        LOG_MESSAGE(log, this->logger_sock);

        this->PopulateErrorResponse(
            protocol::response::EMPTY_FIELD,
            "required field 'start_offset' is not set",
            output
        );
        goto LOG_END;
    }

    {
        InputStream *stream = this->store->GetInputStream(get->path());
        if (!stream) {
            // we've already verified the stream exists, so there are one of two possibilities:
            //   1) it was deleted between then and now
            //   2) error fetching stream
            // TODO react accordingly
            ::zippylog::request_processor::GetInvalidStream log;
            LOG_MESSAGE(log, this->logger_sock);

            this->PopulateErrorResponse(
                protocol::response::PATH_NOT_FOUND,
                "requested stream could not be found",
                output
            );
            goto LOG_END;
        }

        uint64 start_offset = get->start_offset();
        if (start_offset && !stream->SetAbsoluteOffset(start_offset)) {
            throw Exception("TODO handle error setting offset properly");
        }

        // determine how much to fetch
        uint32 bytes_left = this->get_stream_max_bytes;
        uint32 envelopes_left = this->get_stream_max_envelopes;

        // client can lower server default if it wants
        if (get->has_max_response_bytes() && get->max_response_bytes() < bytes_left) {
            bytes_left = get->max_response_bytes();
        }

        if (get->has_max_response_envelopes() && get->max_response_envelopes() < envelopes_left) {
            envelopes_left = get->max_response_envelopes();
        }

        zippylog::Envelope m = zippylog::Envelope();

        uint32 envelope_size = stream->NextEnvelopeSize();
        ::zippylog::Envelope env;
        // could not find envelope in stream at offset
        if (!envelope_size || !stream->ReadEnvelope(env, envelope_size)) {
            ::zippylog::request_processor::GetInvalidOffset log;
            LOG_MESSAGE(log, this->logger_sock);

            // TODO need better error code
            this->PopulateErrorResponse(
                protocol::response::INVALID_STREAM_OFFSET,
                "no envelopes found at requested stream offset",
                output
            );
            goto LOG_END;
        }

        // we must have an envelope, so start the send sequence
        {
            Envelope start_envelope;
            protocol::response::StreamSegmentStart segment_start;
            segment_start.set_path(get->path());
            segment_start.set_offset(get->start_offset());
            segment_start.add_to_envelope(start_envelope);

            // copy request tags to response for client association
            if (request.envelope.tag_size() >= 0) {
                for (int i = 0; i < request.envelope.tag_size(); i++) {
                    start_envelope.envelope.add_tag(request.envelope.tag(i));
                }
            }

            output.push_back(start_envelope);
        }

        output.push_back(env);

        uint32 bytes_read = envelope_size;
        uint32 envelopes_read = 1;
        envelopes_left--;

        if (bytes_read < bytes_left && envelopes_left) {
            bytes_left -= envelope_size;

            while (true) {
                envelope_size = stream->NextEnvelopeSize();
                if (!envelope_size) break;

                // TODO we should be resilient and handle stream errors somehow
                if (!stream->ReadEnvelope(env, envelope_size)) break;

                output.push_back(env);

                bytes_read += envelope_size;
                envelopes_read++;

                if (envelope_size > bytes_left) break;
                if (envelopes_left-- == 1) break;
                bytes_left -= envelope_size;
            }
        }

        protocol::response::StreamSegmentEnd segment_end;
        segment_end.set_envelopes_sent(envelopes_read);
        segment_end.set_bytes_sent(bytes_read);
        // TODO not all stores might share this concept of offsets
        segment_end.set_offset(stream->CurrentEnvelopeOffset());

        env = ::zippylog::Envelope();
        segment_end.add_to_envelope(&env);
        output.push_back(env);
    }

LOG_END:

    ::zippylog::request_processor:: EndProcessGetStream logend;
    LOG_MESSAGE(logend, this->logger_sock);

    return AUTHORITATIVE;
}

RequestProcessor::ResponseStatus RequestProcessor::ProcessSubscribeStoreChanges(Envelope &request, vector<Envelope> &output)
{
    OBTAIN_MESSAGE(protocol::request::SubscribeStoreChanges, m, request, 0);

    // TODO validation

LOG_END:

    return this->HandleSubscribeStoreChanges(request, output);
}

RequestProcessor::ResponseStatus RequestProcessor::ProcessSubscribeEnvelopes(Envelope &request, vector<Envelope> &output)
{
    // TODO validation

    return this->HandleSubscribeEnvelopes(request, output);
}

RequestProcessor::ResponseStatus RequestProcessor::ProcessSubscribeKeepalive(Envelope &request, vector<Envelope> &output)
{
    OBTAIN_MESSAGE(protocol::request::SubscribeKeepalive, m, request, 0);

    {
        ::zippylog::request_processor::ForwardSubscribeKeepalive log;
        log.set_subscription(m->id());
        LOG_MESSAGE(log, this->logger_sock);
    }
    // TODO validation

LOG_END:

    return this->HandleSubscribeKeepalive(request, output);
}

RequestProcessor::ResponseStatus RequestProcessor::ProcessWriteEnvelope(Envelope &request, vector<Envelope> &output)
{
    {
        ::zippylog::request_processor::BeginProcessWriteEnvelope log;
        LOG_MESSAGE(log, this->logger_sock);
    }

    OBTAIN_MESSAGE(protocol::request::WriteEnvelope, m, request, 0)

    if (!this->CheckMessageVersion(m->version(), 1, output)) goto LOG_END;

    if (!m->has_path()) {
        this->PopulateErrorResponse(
            protocol::response::EMPTY_FIELD,
            "required field 'path' missing",
            output
        );
        goto LOG_END;
    }

    // Path validation is a little tricky for write requests.
    // If the path is to a stream set, we require the set to exist. This
    // forces clients to look before they leap. But, they only need to look
    // once, assuming the set doesn't get deleted. For streams paths, we create
    // the path as necessary.
    {
        string path = m->path();
        string bucket, set, stream;
        if (!Store::ParsePath(path, bucket, set, stream)) {
            this->PopulateErrorResponse(
                protocol::response::INVALID_PATH,
                "invalid path: " + path,
                output
            );
            goto LOG_END;
        }

        if (!stream.length()) {
            if (!set.length()) {
                this->PopulateErrorResponse(
                    protocol::response::INVALID_PATH,
                    "path not to a stream set or stream: " + path,
                    output
                );
                goto LOG_END;
            }
            if (!this->CheckPath(path, output, true, true, false)) goto LOG_END;
        }
    }

    // there must be envelopes to write!
    if (m->envelope_size() < 1) {
        this->PopulateErrorResponse(
            protocol::response::EMPTY_FIELD,
            "no envelopes specified in request",
            output
        );
        goto LOG_END;
    }

    // at this point, we've validated the path is to an existing stream set or
    // a stream (we don't care which)
    {
        bool send_ack = m->acknowledge();
        bool synchronous = m->synchronous();

        // synchronous implies send_ack
        if (synchronous) send_ack = true;

        vector<Envelope> envs;
        for (int i = 0; i < m->envelope_size(); i++) {
            const string s = m->envelope(i);

            // catch parse failures and log then move on
            try { envs.push_back(Envelope(s.data(), s.size())); }
            catch (DeserializeException e) {
                ::zippylog::request_processor::EnvelopeParseFailure log;
                log.set_data(s);
                LOG_MESSAGE(log, this->logger_sock);
            }
        }

        int written = 0;

        if (envs.size()) {
            written = this->HandleWriteEnvelopes(m->path(), envs, synchronous);
        }

        if (send_ack) {
            protocol::response::WriteAck ack = protocol::response::WriteAck();
            if (written >= 0) {
                ack.set_envelopes_written(written);
            }
            else {
                // TODO should have error field in ack
                throw Exception("TODO implement error reporting on failed write");
            }

            Envelope e;
            ack.add_to_envelope(e);
            output.push_back(e);
        }
    }

LOG_END:

    ::zippylog::request_processor::EndProcessWriteEnvelope logend;
    LOG_MESSAGE(logend, this->logger_sock);

    // if we have data in the output, we are authoritative
    // else, we assume the user requested no ack, so we have no response
    if (output.size()) {
        return AUTHORITATIVE;
    }
    return DEFERRED;
}

bool RequestProcessor::PopulateErrorResponse(protocol::response::ErrorCode code, string message, vector<Envelope> &msgs)
{
    ::zippylog::request_processor::SendErrorResponse log;
    log.set_message(message);
    LOG_MESSAGE(log, this->logger_sock);

    protocol::response::Error error = protocol::response::Error();
    error.set_code(code);
    error.set_msg(message);
    Envelope response = zippylog::Envelope();
    error.add_to_envelope(&response);
    msgs.push_back(response);

    return true;
}

bool RequestProcessor::CheckPath(const string &path, vector<Envelope> &output, bool require_bucket, bool require_set, bool require_stream)
{
    string bucket, set, stream;

    if (!Store::ParsePath(path, bucket, set, stream)) {
        this->PopulateErrorResponse(
            protocol::response::INVALID_PATH,
            "supplied path does not parse",
            output
        );
        return false;
    }

    if (require_bucket && !bucket.length()) {
        this->PopulateErrorResponse(
            protocol::response::INVALID_PATH,
            "path does not contain a bucket component",
            output
        );
        return false;
    }

    if (require_set && !set.length()) {
        this->PopulateErrorResponse(
            protocol::response::INVALID_PATH,
            "path does not contain a stream set component",
            output
        );
        return false;
    }

    if (require_stream && !stream.length()) {
        this->PopulateErrorResponse(
            protocol::response::INVALID_PATH,
            "path does not contain a stream component",
            output
        );
        return false;
    }

    if (require_bucket && !this->store->BucketExists(bucket)) {
        this->PopulateErrorResponse(
            protocol::response::PATH_NOT_FOUND,
            "bucket does not exist",
            output
        );
        return false;
    }

    if (require_set && !this->store->StreamSetExists(bucket, set)) {
        this->PopulateErrorResponse(
            protocol::response::PATH_NOT_FOUND,
            "stream set does not exist",
            output
        );
        return false;
    }

    if (require_stream && !this->store->StreamExists(bucket, set, stream)) {
        this->PopulateErrorResponse(
            protocol::response::PATH_NOT_FOUND,
            "stream does not exist",
            output
        );
        return false;
    }

    return true;
}

bool RequestProcessor::CheckMessageVersion(uint32 seen_version, uint32 supported_version, ::std::vector<Envelope> &output)
{
    if (seen_version != supported_version) {
        this->PopulateErrorResponse(
            protocol::response::UNSUPPORTED_OPERATION_MESSAGE_VERSION,
            "the version of the message operation is not supported by the server",
            output
        );
        return false;
    }

    return true;
}

} // namespace
