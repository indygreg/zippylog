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
    active(params.active)
{
    if (!this->active) {
        throw "active parameter cannot be NULL";
    }

    if (!this->ctx) {
        throw "ctx parameter cannot be NULL";
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
        ::zippylog::request_processor::RunStart log = ::zippylog::request_processor::RunStart();
        LOG_MESSAGE(log, this->logger_sock);
    }

    zmq::pollitem_t pollitems[1];
    pollitems[0].events = ZMQ_POLLIN;
    pollitems[0].fd = 0;
    pollitems[0].revents = 0;
    pollitems[0].socket = *this->socket;

    while (*this->active) {
        // wait up to 250ms for a message to become available
        // if not, check the active semaphore and try again
        if (!zmq::poll(&pollitems[0], 1, 250000)) {
            continue;
        }

        // we have a message available, so we receive it
        vector<message_t *> msgs;
        this->current_request_identities.clear();
        if (!zeromq::receive_multipart_message(this->socket, this->current_request_identities, msgs)) {
            ::zippylog::request_processor::FailReceiveMessage log = ::zippylog::request_processor::FailReceiveMessage();
            LOG_MESSAGE(log, this->logger_sock);

            // TODO shouldn't this be part of the receive_multipart_message API?
            vector<message_t *>::iterator iter = msgs.begin();
            for (; iter != msgs.end(); iter++) {
                delete *iter;
            }

            // TODO we used to rebuild socket here. is this a good recovery strategy?
            // this exception is here so we can see if this actually happens
            // I suspect it doesn't
            throw "error receiving multipart message from worker sock";
        }

        vector<Envelope> response_envelopes;
        this->ProcessMessages(this->current_request_identities, msgs, response_envelopes);

        // input messages aren't needed any more, so we destroy them immediately
        vector<message_t *>::iterator iter = msgs.begin();
        for (; iter != msgs.end(); iter++) {
            delete *iter;
        }

        if (response_envelopes.size()) {
            if (!zeromq::send_envelopes(this->socket, this->current_request_identities, response_envelopes)) {
                throw "TODO log failure to send response envelopes";
            }
        }
    }

    ::zippylog::request_processor::RunStop log = ::zippylog::request_processor::RunStop();
    LOG_MESSAGE(log, this->logger_sock);

    return;
}

void RequestProcessor::ProcessMessages(vector<string> &identities, vector<message_t *> &input, vector<Envelope> &output)
{
    // we need to declare all our variables first b/c we use goto

    // always clear the output list so we don't accidentally send leftovers
    output.clear();

    char msg_version = 0;
    bool have_request_envelope = false;
    Envelope request_envelope;
    ResponseStatus result;
    bool successful_process = false;

    // TODO should we log?
    if (!input.size()) return;

    // we expect the first message to have an envelope of some kind
    if (!input[0]->size()) {
        ::zippylog::request_processor::ReceiveEmptyMessage log = ::zippylog::request_processor::ReceiveEmptyMessage();
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
        ::zippylog::request_processor::UnknownMessageVersion log = ::zippylog::request_processor::UnknownMessageVersion();
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
        ::zippylog::request_processor::EnvelopeParseFailure log = ::zippylog::request_processor::EnvelopeParseFailure();
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
        ::zippylog::request_processor::EmptyEnvelope log = ::zippylog::request_processor::EmptyEnvelope();
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
        ::zippylog::request_processor::InvalidMessageEnumeration log = ::zippylog::request_processor::InvalidMessageEnumeration();
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
        ::zippylog::request_processor::InvalidMessageEnumeration log = ::zippylog::request_processor::InvalidMessageEnumeration();
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

        case protocol::request::Get::zippylog_enumeration:
            result = this->ProcessGet(request_envelope, output);
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

        default:
        {
            ::zippylog::request_processor::UnknownRequestType log = ::zippylog::request_processor::UnknownRequestType();
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

RequestProcessor::ResponseStatus RequestProcessor::ProcessStoreInfo(Envelope &e, vector<Envelope> &output)
{
    ::zippylog::request_processor::BeginProcessStoreInfo logstart = ::zippylog::request_processor::BeginProcessStoreInfo();
    LOG_MESSAGE(logstart, this->logger_sock);

    protocol::request::GetStoreInfo *m = (protocol::request::GetStoreInfo *)e.GetMessage(0);
    if (!m) {
        throw "TODO handle error parsing GetStoreInfo request message";
    }

    if (!this->CheckMessageVersion(m->version(), 1, output)) return AUTHORITATIVE;

    protocol::StoreInfo info = protocol::StoreInfo();
    this->store->StoreInfo(info);

    ::zippylog::request_processor::EndProcessStoreInfo logend = ::zippylog::request_processor::EndProcessStoreInfo();
    LOG_MESSAGE(logend, this->logger_sock);

    Envelope out;
    info.add_to_envelope(&out);
    output.push_back(out);
    return AUTHORITATIVE;
}

RequestProcessor::ResponseStatus RequestProcessor::ProcessBucketInfo(Envelope &e, vector<Envelope> &output)
{
    ::zippylog::request_processor::BeginProcessBucketInfo logstart = ::zippylog::request_processor::BeginProcessBucketInfo();
    LOG_MESSAGE(logstart, this->logger_sock);

    protocol::request::GetBucketInfo *m = (protocol::request::GetBucketInfo *)e.GetMessage(0);
    if (!m) {
        throw "TODO handle error parsing GetBucketInfo request message";
    }

    if (!this->CheckMessageVersion(m->version(), 1, output)) return AUTHORITATIVE;

    if (!this->CheckPath(m->path(), output, true)) return AUTHORITATIVE;

    protocol::BucketInfo info = protocol::BucketInfo();
    string bucket;
    Store::ParseBucketPath(m->path(), bucket);
    this->store->BucketInfo(bucket, info);

    ::zippylog::request_processor::EndProcessBucketInfo logend = ::zippylog::request_processor::EndProcessBucketInfo();
    LOG_MESSAGE(logend, this->logger_sock);

    info.add_to_envelope(&e);
    return AUTHORITATIVE;
}

RequestProcessor::ResponseStatus RequestProcessor::ProcessStreamSetInfo(Envelope &e, vector<Envelope> &output)
{
    ::zippylog::request_processor::BeginProcessStreamSetInfo logstart = ::zippylog::request_processor::BeginProcessStreamSetInfo();
    LOG_MESSAGE(logstart, this->logger_sock);

    protocol::request::GetStreamSetInfo *m = (protocol::request::GetStreamSetInfo *)e.GetMessage(0);
    if (!m) {
        throw "TODO handle error parsing GetStreamSetInfo request message";
    }

    if (!this->CheckMessageVersion(m->version(), 1, output)) return AUTHORITATIVE;

    if (!this->CheckPath(m->path(), output, true, true)) return AUTHORITATIVE;

    string bucket, set;
    Store::ParseStreamSetPath(m->path(), bucket, set);

    protocol::StreamSetInfo info = protocol::StreamSetInfo();
    this->store->StreamsetInfo(bucket, set, info);

    ::zippylog::request_processor::EndProcessStreamSetInfo logend = ::zippylog::request_processor::EndProcessStreamSetInfo();
    LOG_MESSAGE(logend, this->logger_sock);

    info.add_to_envelope(&e);
    return AUTHORITATIVE;
}

RequestProcessor::ResponseStatus RequestProcessor::ProcessStreamInfo(Envelope &e, vector<Envelope> &output)
{
    ::zippylog::request_processor::BeginProcessStreamInfo logstart = ::zippylog::request_processor::BeginProcessStreamInfo();
    LOG_MESSAGE(logstart, this->logger_sock);

    protocol::request::GetStreamInfo *m = (protocol::request::GetStreamInfo *)e.GetMessage(0);
    if (!m) {
        throw "TODO handle error parsing GetStreamSetInfo request message";
    }

    if (!this->CheckMessageVersion(m->version(), 1, output)) return AUTHORITATIVE;
    if (!this->CheckPath(m->path(), output, true, true, true)) return AUTHORITATIVE;

    string bucket, set, stream;
    Store::ParsePath(m->path(), bucket, set, stream);

    protocol::StreamInfo info = protocol::StreamInfo();
    this->store->StreamInfo(bucket, set, stream, info);

    ::zippylog::request_processor::EndProcessStreamInfo logend = ::zippylog::request_processor::EndProcessStreamInfo();
    LOG_MESSAGE(logend, this->logger_sock);

    info.add_to_envelope(&e);
    return AUTHORITATIVE;
}

RequestProcessor::ResponseStatus RequestProcessor::ProcessGet(Envelope &request, vector<Envelope> &output)
{
    protocol::request::Get *get = (protocol::request::Get *)request.GetMessage(0);
    if (!get) {
        ::zippylog::request_processor::ReceiveInvalidGet log = ::zippylog::request_processor::ReceiveInvalidGet();
        LOG_MESSAGE(log, this->logger_sock);

        this->PopulateErrorResponse(
            protocol::response::UNKNOWN_REQUEST_TYPE,
            "error parsing get message... weird",
            output
        );
        return AUTHORITATIVE;
    }

    if (!get->has_path()) {
        ::zippylog::request_processor::ReceiveInvalidGet log = ::zippylog::request_processor::ReceiveInvalidGet();
        LOG_MESSAGE(log, this->logger_sock);

        this->PopulateErrorResponse(
            protocol::response::EMPTY_FIELD,
            "required field 'path' is empty",
            output
        );
        return AUTHORITATIVE;
    }

    if (!get->has_start_offset()) {
        ::zippylog::request_processor::ReceiveInvalidGet log = ::zippylog::request_processor::ReceiveInvalidGet();
        LOG_MESSAGE(log, this->logger_sock);

        this->PopulateErrorResponse(
            protocol::response::EMPTY_FIELD,
            "required field 'start_offset' is not defined",
            output
        );
        return AUTHORITATIVE;
    }

    // TODO perform additional stream verification

    InputStream *stream = this->store->GetInputStream(get->path());
    if (!stream) {
        ::zippylog::request_processor::GetInvalidStream log = ::zippylog::request_processor::GetInvalidStream();
        LOG_MESSAGE(log, this->logger_sock);

        this->PopulateErrorResponse(
            protocol::response::PATH_NOT_FOUND,
            "requested stream could not be found",
            output
        );
        return AUTHORITATIVE;
    }

    uint64 start_offset = get->start_offset();
    if (!stream->SetAbsoluteOffset(start_offset)) {
        throw "could not set stream offset";
    }

    // determine how much to fetch
    uint32 bytes_left = 256000; // TODO pull from server config

    // client can lower server default if it wants
    if (get->has_max_response_bytes() && get->max_response_bytes() < bytes_left) {
        bytes_left = get->max_response_bytes();
    }

    zippylog::Envelope m = zippylog::Envelope();

    uint32 envelope_size = stream->NextEnvelopeSize();
    // could not find envelope in stream at offset
    if (!envelope_size) {
        ::zippylog::request_processor::GetInvalidOffset log = ::zippylog::request_processor::GetInvalidOffset();
        LOG_MESSAGE(log, this->logger_sock);

        // TODO need better error code
        this->PopulateErrorResponse(
            protocol::response::PATH_NOT_FOUND,
            "no envelopes found at stream offset",
            output
        );
        return AUTHORITATIVE;
    }

    // we must have an envelope, so start the send sequence
    ::zippylog::request_processor::BeginProcessGet logstart = ::zippylog::request_processor::BeginProcessGet();
    LOG_MESSAGE(logstart, this->logger_sock);

    protocol::response::StreamSegmentStart segment_start = protocol::response::StreamSegmentStart();
    segment_start.set_path(get->path());
    segment_start.set_offset(get->start_offset());
    ::zippylog::Envelope env = ::zippylog::Envelope();
    segment_start.add_to_envelope(&env);

    // copy request tags to response for client association
    if (request.envelope.tag_size() >= 0) {
        for (int i = 0; i < request.envelope.tag_size(); i++) {
            env.envelope.add_tag(request.envelope.tag(i));
        }
    }

    output.push_back(env);

    uint32 bytes_read = 0;
    uint32 envelopes_read = 0;

    while (true) {
        if (!stream->ReadEnvelope(env, envelope_size)) break;

        output.push_back(env);

        bytes_read += envelope_size;
        envelopes_read++;

        if (bytes_left - envelope_size < 0) break;
        bytes_left -= envelope_size;

        envelope_size = stream->NextEnvelopeSize();
        if (!envelope_size) break;
    }

    protocol::response::StreamSegmentEnd segment_end = protocol::response::StreamSegmentEnd();
    segment_end.set_envelopes_sent(envelopes_read);
    segment_end.set_bytes_sent(bytes_read);
    segment_end.set_offset(start_offset + bytes_read);

    env = ::zippylog::Envelope();
    segment_end.add_to_envelope(&env);
    output.push_back(env);

    ::zippylog::request_processor:: EndProcessGet logend = ::zippylog::request_processor:: EndProcessGet();
    LOG_MESSAGE(logend, this->logger_sock);

    return AUTHORITATIVE;
}

RequestProcessor::ResponseStatus RequestProcessor::ProcessSubscribeStoreChanges(Envelope &request, vector<Envelope> &output)
{
    protocol::request::SubscribeStoreChanges *m =
        (protocol::request::SubscribeStoreChanges *)request.GetMessage(0);

    if (!m) {
        // TODO log error and send error response
        throw "TODO handle error";
    }

    // TODO validation

    return this->HandleSubscribeStoreChanges(request, output);
}

RequestProcessor::ResponseStatus RequestProcessor::ProcessSubscribeEnvelopes(Envelope &request, vector<Envelope> &output)
{
    // TODO validation

    return this->HandleSubscribeEnvelopes(request, output);
}

RequestProcessor::ResponseStatus RequestProcessor::ProcessSubscribeKeepalive(Envelope &request, vector<Envelope> &output)
{
    protocol::request::SubscribeKeepalive *m =
        (protocol::request::SubscribeKeepalive *)request.GetMessage(0);

    ::zippylog::request_processor::ForwardSubscribeKeepalive log = ::zippylog::request_processor::ForwardSubscribeKeepalive();
    log.set_subscription(m->id());
    LOG_MESSAGE(log, this->logger_sock);

    // TODO validation

    return this->HandleSubscribeKeepalive(request, output);
}

bool RequestProcessor::PopulateErrorResponse(protocol::response::ErrorCode code, string message, vector<Envelope> &msgs)
{
    ::zippylog::request_processor::SendErrorResponse log = ::zippylog::request_processor::SendErrorResponse();
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
