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
#include <zippylog/protocol/request.pb.h>

#include <gtest/gtest.h>
#include <zmq.hpp>

using namespace ::zippylog;

using ::std::invalid_argument;
using ::std::string;
using ::std::vector;
using ::zmq::message_t;

#define EXPECT_ENVELOPE_MESSAGE(index, msg) { \
    Envelope e = output[index]; \
    ASSERT_TRUE(e.MessageCount() > 0); \
    uint32 expected = msg::zippylog_namespace; \
    ASSERT_EQ(expected, e.MessageNamespace(0)); \
    expected = msg::zippylog_enumeration; \
    ASSERT_EQ(expected, e.MessageType(0)); \
}

/// request processor implementation that we use to test things
class TestRequestProcessor : public RequestProcessor
{
public:
    TestRequestProcessor(RequestProcessorStartParams &params) :
      RequestProcessor(params),
      subscribe_store_changes_count(0),
      handle_subscribe_envelopes_count(0),
      handle_subscribe_keepalive_count(0),
      write_envelopes_count(0)
    { }

    int subscribe_store_changes_count;
    int handle_subscribe_envelopes_count;
    int handle_subscribe_keepalive_count;
    int write_envelopes_count;

protected:
    void Reset()
    {
        this->subscribe_store_changes_count = 0;
        this->handle_subscribe_envelopes_count = 0;
        this->handle_subscribe_keepalive_count = 0;
        this->write_envelopes_count = 0;
    }

    HandleSubscriptionResult HandleSubscriptionRequest(SubscriptionInfo subscription) {
        if (subscription.type == ENVELOPE) {
            this->handle_subscribe_envelopes_count++;
        }
        else if (subscription.type == STORE_CHANGE) {
            this->subscribe_store_changes_count++;
        }

        HandleSubscriptionResult result;
        result.result = HandleSubscriptionResult::ACCEPTED;
        result.id = "XX";

        return result;
    }

    ResponseStatus HandleSubscribeKeepalive(Envelope &, vector<Envelope> &)
    {
        this->handle_subscribe_keepalive_count++;
        return AUTHORITATIVE;
    }

    int HandleWriteEnvelopes(const string &, vector<Envelope> &to_write, bool)
    {
        this->write_envelopes_count += to_write.size();
        return to_write.size();
    }
};

/// Request processor tester
///
/// This verifies that the core request processor properly handles all
/// zippylog protocol request types. It only verifies incoming protocol
/// message and the validation component associated with it are working
/// properly.
class RequestProcessorTest : public ::testing::Test {
protected:
    TestRequestProcessor *p;
    bool active;
    ::zmq::context_t ctx;
    ::zmq::socket_t logger;
    ::zmq::socket_t client;
    Store *store;

    RequestProcessorTest() :
        p(NULL),
        active(true),
        ctx(1),
        logger(ctx, ZMQ_PULL),
        client(ctx, ZMQ_XREQ),
        store(NULL)
    {
        this->logger.bind("inproc://logger");
        this->client.bind("inproc://client");
        this->store = Store::CreateStore("simpledirectory://test/stores/00-simple");
    }

    ~RequestProcessorTest()
    {
        if (this->p) delete this->p;
        if (this->store) delete this->store;
    }

    void ResetProcessor(const string store_path = "simpledirectory://test/stores/00-simple")
    {
        if (this->p) delete this->p;
        if (this->store) delete this->store;

        RequestProcessorStartParams params;
        params.active = &this->active;
        params.store_path = store_path;
        params.ctx = &this->ctx;
        params.logger_endpoint = "inproc://logger";
        params.client_endpoint = "inproc://client";

        this->p = new TestRequestProcessor(params);
        this->store = Store::CreateStore(store_path);
    }

    void ExpectErrorResponse(RequestProcessor::ResponseStatus result, protocol::response::ErrorCode code, vector<Envelope> &msgs)
    {
        EXPECT_TRUE(::zippylog::RequestProcessor::AUTHORITATIVE == result);
        this->ExpectErrorResponse(code, msgs);
    }

    void ExpectErrorResponse(protocol::response::ErrorCode code, vector<Envelope> &msgs)
    {
        ASSERT_EQ(1, msgs.size());

        Envelope response = msgs[0];
        ASSERT_EQ(1, response.MessageCount());

        uint32 expected = protocol::response::Error::zippylog_namespace;
        ASSERT_EQ(expected, response.MessageNamespace(0));
        expected = protocol::response::Error::zippylog_enumeration;
        ASSERT_EQ(expected, response.MessageType(0));

        protocol::response::Error *m = (protocol::response::Error *)response.GetMessage(0);
        ASSERT_TRUE(m != NULL);
        ASSERT_TRUE(m->has_code());
        ASSERT_TRUE(m->has_msg());
        ASSERT_EQ(code, m->code());
    }

    void SetUp()
    {
        this->ResetProcessor();
    }
};

TEST_F(RequestProcessorTest, ConstructorParameterValidation)
{
    RequestProcessorStartParams params;
    bool active;
    ::zmq::context_t ctx(1);

    EXPECT_THROW(TestRequestProcessor p(params), invalid_argument);

    params.active = &active;
    EXPECT_THROW(TestRequestProcessor p(params), invalid_argument);

    params.active = NULL;
    params.ctx = &ctx;
    EXPECT_THROW(TestRequestProcessor p(params), invalid_argument);

    params.active = &active;
    EXPECT_THROW(TestRequestProcessor p(params), invalid_argument);
}

// this test verifies our core message processing routine is robust and functional
TEST_F(RequestProcessorTest, ProcessMessages)
{
    protocol::response::ErrorCode code;

    vector<string> identities;
    identities.push_back("identityA");
    identities.push_back("identityB");

    vector<message_t *> input;
    vector<Envelope> output;

    // no input == no output
    this->p->ProcessMessages(identities, input, output);
    ASSERT_EQ(0, output.size());

    // empty initial message
    message_t m(0);
    input.push_back(&m);
    code = protocol::response::EMPTY_MESSAGE;
    this->p->ProcessMessages(identities, input, output);
    this->ExpectErrorResponse(code, output);
    output.clear();

    // bad version
    m.rebuild(1);
    *(char *)(m.data()) = 0;
    code = protocol::response::UNKNOWN_MESSAGE_FORMAT_VERSION;
    this->p->ProcessMessages(identities, input, output);
    this->ExpectErrorResponse(code, output);
    output.clear();

    m.rebuild(1);
    *(char *)(m.data()) = 0x02;
    code = protocol::response::UNKNOWN_MESSAGE_FORMAT_VERSION;
    this->p->ProcessMessages(identities, input, output);
    this->ExpectErrorResponse(code, output);
    output.clear();

    // no data after version
    m.rebuild(1);
    *(char *)(m.data()) = 0x01;
    code = protocol::response::PROTOCOL_NO_ENVELOPE;
    this->p->ProcessMessages(identities, input, output);
    this->ExpectErrorResponse(code, output);
    output.clear();

    // bad envelope after version
    m.rebuild(10);
    *(char *)(m.data()) = 0x01;
    code = protocol::response::ENVELOPE_PARSE_FAILURE;
    this->p->ProcessMessages(identities, input, output);
    this->ExpectErrorResponse(code, output);
    output.clear();

    // working store info request
    protocol::request::GetStoreInfoV1 gsi;
    Envelope request;
    gsi.add_to_envelope(request);
    ASSERT_TRUE(request.ToProtocolZmqMessage(m));
    this->p->ProcessMessages(identities, input, output);
    ASSERT_EQ(1, output.size());
    Envelope response = output[0];
    ASSERT_EQ(1, response.MessageCount());
    EXPECT_ENVELOPE_MESSAGE(0, protocol::StoreInfoV1);
    output.clear();
}

TEST_F(RequestProcessorTest, EmptyEnvelope)
{
    Envelope e;

    vector<Envelope> output;
    RequestProcessor::ResponseStatus result = this->p->ProcessRequest(e, output);
    this->ExpectErrorResponse(result, protocol::response::EMPTY_ENVELOPE, output);
}

TEST_F(RequestProcessorTest, InvalidMessageNamespace)
{
    Envelope e;
    zippylog::message::Envelope m;
    EXPECT_TRUE(e.AddMessage(m, 23523, 32145));

    vector<Envelope> output;
    RequestProcessor::ResponseStatus result = this->p->ProcessRequest(e, output);
    this->ExpectErrorResponse(result, protocol::response::INVALID_MESSAGE_NAMESPACE, output);
}

TEST_F(RequestProcessorTest, Ping)
{
    protocol::request::PingV1 ping;
    Envelope e;
    ping.add_to_envelope(e);

    vector<Envelope> output;
    RequestProcessor::ResponseStatus result = this->p->ProcessRequest(e, output);
    EXPECT_TRUE(::zippylog::RequestProcessor::AUTHORITATIVE == result);
    ASSERT_EQ(1, output.size());
    Envelope response = output[0];
    EXPECT_EQ(1, response.MessageCount());
    EXPECT_ENVELOPE_MESSAGE(0, protocol::response::PongV1);
}

TEST_F(RequestProcessorTest, GetFeatures)
{
    protocol::request::GetFeaturesV1 m;
    Envelope e;
    m.add_to_envelope(e);

    vector<Envelope> output;
    RequestProcessor::ResponseStatus result = this->p->ProcessRequest(e, output);
    EXPECT_TRUE(::zippylog::RequestProcessor::AUTHORITATIVE == result);

    EXPECT_EQ(1, output.size());
    Envelope response = output[0];
    EXPECT_EQ(1, response.MessageCount());
    EXPECT_ENVELOPE_MESSAGE(0, protocol::response::FeatureSpecificationV1);
    protocol::response::FeatureSpecificationV1 *features = (protocol::response::FeatureSpecificationV1 *)response.GetMessage(0);
    ASSERT_TRUE(features != NULL);

    EXPECT_EQ(1, features->supported_message_formats_size());
    EXPECT_EQ(1, features->supported_message_formats(0));

    EXPECT_EQ(13, features->supported_request_types_size());
    EXPECT_EQ(13, features->supported_request_names_size());
}

TEST_F(RequestProcessorTest, GetStoreInfo)
{
    protocol::request::GetStoreInfoV1 m;
    Envelope e;
    m.add_to_envelope(&e);

    vector<Envelope> output;
    RequestProcessor::ResponseStatus result = this->p->ProcessRequest(e, output);
    EXPECT_TRUE(::zippylog::RequestProcessor::AUTHORITATIVE == result);

    EXPECT_EQ(1, output.size());
    Envelope response = output[0];
    EXPECT_EQ(1, response.MessageCount());
    EXPECT_ENVELOPE_MESSAGE(0, protocol::StoreInfoV1);
    protocol::StoreInfoV1 *r = (protocol::StoreInfoV1 *)response.GetMessage(0);
    ASSERT_TRUE(r != NULL);
    EXPECT_EQ(2, r->bucket_size());

    protocol::StoreInfoV1 esi;
    EXPECT_TRUE(this->store->StoreInfo(esi));
    EXPECT_STREQ(esi.SerializeAsString().c_str(), r->SerializeAsString().c_str());
}

TEST_F(RequestProcessorTest, GetBucketInfo)
{
    vector<Envelope> output;

    // path does not contain bucket
    protocol::request::GetBucketInfoV1 m2;
    m2.set_path("/");
    Envelope e2;
    m2.add_to_envelope(&e2);
    this->ExpectErrorResponse(this->p->ProcessRequest(e2, output), protocol::response::INVALID_PATH, output);
    output.clear();

    // this should work
    protocol::request::GetBucketInfoV1 m3;
    m3.set_path("/bucketA");
    Envelope e3;
    m3.add_to_envelope(&e3);
    ASSERT_TRUE(::zippylog::RequestProcessor::AUTHORITATIVE == this->p->ProcessRequest(e3, output));
    ASSERT_EQ(1, output.size());
    Envelope response = output[0];
    EXPECT_ENVELOPE_MESSAGE(0, protocol::BucketInfoV1);
    protocol::BucketInfoV1 *m = (protocol::BucketInfoV1 *)response.GetMessage(0);
    ASSERT_TRUE(m != NULL);
    protocol::BucketInfoV1 ebi;
    ASSERT_TRUE(this->store->BucketInfo("bucketA", ebi));
    ASSERT_STREQ(ebi.SerializeAsString().c_str(), m->SerializeAsString().c_str());
    output.clear();

    // bucket doesn't exist
    protocol::request::GetBucketInfoV1 m4;
    m4.set_path("/DOESNOTEXIST");
    Envelope e4;
    m4.add_to_envelope(&e4);
    this->ExpectErrorResponse(this->p->ProcessRequest(e4, output), protocol::response::PATH_NOT_FOUND, output);
    output.clear();
}

TEST_F(RequestProcessorTest, GetStreamSetInfo)
{
    // path does not contain bucket nor stream set
    {
        protocol::request::GetStreamSetInfoV1 req;
        req.set_path("/");
        Envelope e;
        req.add_to_envelope(e);
        vector<Envelope> output;
        this->ExpectErrorResponse(this->p->ProcessRequest(e, output), protocol::response::INVALID_PATH, output);
    }

    // path contains bucket but not stream set
    {
        protocol::request::GetStreamSetInfoV1 req;
        req.set_path("/bucketA");
        Envelope e;
        req.add_to_envelope(e);
        vector<Envelope> output;
        this->ExpectErrorResponse(this->p->ProcessRequest(e, output), protocol::response::INVALID_PATH, output);
    }

    // this should work
    {
        protocol::request::GetStreamSetInfoV1 req;
        req.set_path("/bucketA/set0");
        Envelope e;
        req.add_to_envelope(e);
        vector<Envelope> output;
        ASSERT_TRUE(::zippylog::RequestProcessor::AUTHORITATIVE == this->p->ProcessRequest(e, output));
        ASSERT_EQ(1, output.size());
        Envelope response = output[0];
        EXPECT_ENVELOPE_MESSAGE(0, protocol::StreamSetInfoV1);
        protocol::StreamSetInfoV1 *m = (protocol::StreamSetInfoV1 *)response.GetMessage(0);
        ASSERT_TRUE(m != NULL);
        ASSERT_TRUE(m->has_path());
        EXPECT_EQ(0, m->stream_size());
        EXPECT_STREQ("set0", m->path().c_str());
    }

    // path to set that does not exist
    {
        protocol::request::GetStreamSetInfoV1 req;
        req.set_path("/bucketA/DOESNOTEXIST");
        Envelope e;
        req.add_to_envelope(e);
        vector<Envelope> output;
        this->ExpectErrorResponse(this->p->ProcessRequest(e, output), protocol::response::PATH_NOT_FOUND, output);
    }
}

TEST_F(RequestProcessorTest, GetStreamInfo)
{
    this->ResetProcessor("simpledirectory://test/stores/01-singlestream");

    // path does not contain bucket nor stream set nor stream
    {
        protocol::request::GetStreamInfoV1 req;
        req.set_path("/");
        Envelope e;
        req.add_to_envelope(e);
        vector<Envelope> output;
        this->ExpectErrorResponse(this->p->ProcessRequest(e, output), protocol::response::INVALID_PATH, output);
    }

    // path contains bucket but not stream set nor stream
    {
        protocol::request::GetStreamInfoV1 req;
        req.set_path("/A");
        Envelope e;
        req.add_to_envelope(e);
        vector<Envelope> output;
        this->ExpectErrorResponse(this->p->ProcessRequest(e, output), protocol::response::INVALID_PATH, output);
    }

    // path contains bucket and stream set but not stream
    {
        protocol::request::GetStreamInfoV1 req;
        req.set_path("/A/B");
        Envelope e;
        req.add_to_envelope(e);
        vector<Envelope> output;
        this->ExpectErrorResponse(this->p->ProcessRequest(e, output), protocol::response::INVALID_PATH, output);
    }

    // this should work
    {
        protocol::request::GetStreamInfoV1 req;
        req.set_path("/A/B/2010-11-26-07");
        Envelope e;
        req.add_to_envelope(e);
        vector<Envelope> output;
        ASSERT_TRUE(::zippylog::RequestProcessor::AUTHORITATIVE == this->p->ProcessRequest(e, output));
        ASSERT_EQ(1, output.size());
        Envelope response = output[0];
        EXPECT_ENVELOPE_MESSAGE(0, protocol::StreamInfoV1);
        protocol::StreamInfoV1 *m = (protocol::StreamInfoV1 *)response.GetMessage(0);
        ASSERT_TRUE(m != NULL);
        ASSERT_TRUE(m->has_path());
        EXPECT_STREQ("2010-11-26-07", m->path().c_str());
        EXPECT_TRUE(m->has_length());

        protocol::StreamInfoV1 expected;
        EXPECT_TRUE(this->store->StreamInfo("/A/B/2010-11-26-07", expected));
        EXPECT_EQ(expected.length(), m->length());
    }

    // path to stream that does not exist
    {
        protocol::request::GetStreamInfoV1 req;
        req.set_path("/A/B/2011-01-01-00");
        Envelope e;
        req.add_to_envelope(e);
        vector<Envelope> output;
        this->ExpectErrorResponse(this->p->ProcessRequest(e, output), protocol::response::PATH_NOT_FOUND, output);
    }
}

TEST_F(RequestProcessorTest, GetStreamSegment)
{
    this->ResetProcessor("simpledirectory://test/stores/01-singlestream");

    string path = "/A/B/2010-11-26-07";

    ASSERT_TRUE(this->store->PathExists(path));

    // simple fetch of 1 envelope
    {
        protocol::request::GetStreamSegmentV1 m = protocol::request::GetStreamSegmentV1();
        m.set_path(path);
        m.set_start_offset(0);
        m.set_max_response_envelopes(1);
        Envelope e;
        m.add_to_envelope(e);
        vector<Envelope> output;

        ASSERT_TRUE(RequestProcessor::AUTHORITATIVE == this->p->ProcessRequest(e, output));

        // response should have stream segment start + content envelope + stream segment end
        ASSERT_EQ(3, output.size());
        EXPECT_ENVELOPE_MESSAGE(0, protocol::response::StreamSegmentStartV1);
        EXPECT_ENVELOPE_MESSAGE(2, protocol::response::StreamSegmentEndV1);

        ASSERT_EQ(1, output[0].MessageCount());
        ASSERT_EQ(1, output[0].MessageCount());
        protocol::response::StreamSegmentStartV1 *s1 = (protocol::response::StreamSegmentStartV1 *)output[0].GetMessage(0);
        ASSERT_TRUE(s1 != NULL);
        ASSERT_TRUE(s1->has_path());
        ASSERT_TRUE(s1->has_offset());
        ASSERT_TRUE(s1->path() == path);
        ASSERT_EQ(0, s1->offset());

        protocol::response::StreamSegmentEndV1 *end1 = (protocol::response::StreamSegmentEndV1 *)output[2].GetMessage(0);
        ASSERT_TRUE(end1 != NULL);
        ASSERT_TRUE(end1->has_offset());
        ASSERT_TRUE(end1->has_bytes_sent());
        ASSERT_TRUE(end1->has_envelopes_sent());
        ASSERT_EQ(1, end1->envelopes_sent());
        ASSERT_EQ(44, end1->offset());
        ASSERT_EQ(end1->offset() - 1, end1->bytes_sent());
    }

    // fetch of 10 envelopes
    {
        protocol::request::GetStreamSegmentV1 m;
        m.set_path(path);
        m.set_start_offset(0);
        m.set_max_response_envelopes(10);
        Envelope e;
        m.add_to_envelope(e);
        vector<Envelope> output;

        ASSERT_TRUE(RequestProcessor::AUTHORITATIVE == this->p->ProcessRequest(e, output));
        ASSERT_EQ(12, output.size());
        EXPECT_ENVELOPE_MESSAGE(0, protocol::response::StreamSegmentStartV1);
        EXPECT_ENVELOPE_MESSAGE(11, protocol::response::StreamSegmentEndV1);

        // we aren't testing that the store reads properly - we have other
        // tests for that
        InputStream * stream = this->store->GetInputStream(path);
        for (int i = 0; i < 10; i++) {
            Envelope expected;
            uint32 bytes_read;
            ASSERT_TRUE(stream->ReadEnvelope(expected, bytes_read));

            EXPECT_EQ(expected.MessageCount(), output[1+i].MessageCount());
            EXPECT_STREQ(expected.ToString().c_str(), output[i+1].ToString().c_str());
        }

        protocol::response::StreamSegmentEndV1 *end = (protocol::response::StreamSegmentEndV1 *)output[11].GetMessage(0);
        ASSERT_TRUE(end != NULL);
        ASSERT_TRUE(end->has_offset());
        ASSERT_TRUE(end->has_bytes_sent());
        ASSERT_TRUE(end->has_envelopes_sent());
        EXPECT_EQ(10, end->envelopes_sent());
        EXPECT_EQ(stream->CurrentEnvelopeOffset(), end->offset());
        EXPECT_EQ(end->offset() - 1, end->bytes_sent());
    }

    // fetch with valid offset
    {
        protocol::request::GetStreamSegmentV1 m;
        m.set_path(path);

        InputStream * stream = this->store->GetInputStream(path);
        Envelope expected;
        uint32 bytes_read;
        stream->ReadEnvelope(expected, bytes_read);
        stream->ReadEnvelope(expected, bytes_read);

        m.set_start_offset(stream->CurrentEnvelopeOffset());
        m.set_max_response_envelopes(2);
        Envelope req;
        m.add_to_envelope(req);
        vector<Envelope> output;

        ASSERT_TRUE(RequestProcessor::AUTHORITATIVE == this->p->ProcessRequest(req, output));
        ASSERT_EQ(4, output.size());
        EXPECT_ENVELOPE_MESSAGE(0, protocol::response::StreamSegmentStartV1);
        EXPECT_ENVELOPE_MESSAGE(3, protocol::response::StreamSegmentEndV1);
    }

    // fetch with invalid offset
    {
        protocol::request::GetStreamSegmentV1 m;
        m.set_path(path);
        m.set_start_offset(2);
        m.set_max_response_envelopes(2);
        Envelope req;
        m.add_to_envelope(req);
        vector<Envelope> output;

        this->ExpectErrorResponse(this->p->ProcessRequest(req, output), protocol::response::INVALID_STREAM_OFFSET, output);
    }
}

TEST_F(RequestProcessorTest, WriteEnvelopeErrorChecking)
{
    vector<Envelope> output;
    string path = "/bucketA/set0";

    // invalid path (to a bucket)
    {
        protocol::request::WriteEnvelopeV1 r;
        r.set_path("/bucketA");
        Envelope e;
        r.add_to_envelope(e);
        this->ExpectErrorResponse(this->p->ProcessRequest(e, output), protocol::response::INVALID_PATH, output);
        output.clear();
    }

    // path to non-existing stream set
    {
        protocol::request::WriteEnvelopeV1 r;
        r.set_path("/bucketA/DOESNOTEXIST");
        Envelope e;
        r.add_to_envelope(e);
        this->ExpectErrorResponse(this->p->ProcessRequest(e, output), protocol::response::PATH_NOT_FOUND, output);
        output.clear();
    }

    // valid path field, but no envelopes
    {
        protocol::request::WriteEnvelopeV1 r;
        r.set_path(path);
        Envelope e;
        r.add_to_envelope(e);
        this->ExpectErrorResponse(this->p->ProcessRequest(e, output), protocol::response::EMPTY_FIELD, output);
        output.clear();
    }
}

TEST_F(RequestProcessorTest, WriteEnvelopeSingleEnvelopeAck)
{
    vector<Envelope> output;

    protocol::request::WriteEnvelopeV1 r;
    r.set_path("/bucketA/set0");
    Envelope e, w;
    r.add_to_envelope(w);
    string serialized;
    w.Serialize(serialized);
    r.add_envelope(serialized);
    r.add_to_envelope(e);
    ASSERT_TRUE(RequestProcessor::AUTHORITATIVE == this->p->ProcessRequest(e, output));
    EXPECT_EQ(1, this->p->write_envelopes_count);
    ASSERT_EQ(1, output.size());
    EXPECT_ENVELOPE_MESSAGE(0, protocol::response::WriteAckV1);
    protocol::response::WriteAckV1 *m = (protocol::response::WriteAckV1 *)output[0].GetMessage(0);
    EXPECT_TRUE(m->has_envelopes_written());
    EXPECT_EQ(1, m->envelopes_written());
}

TEST_F(RequestProcessorTest, WriteEnvelopeSingleEnvelopeNoAck)
{
    vector<Envelope> output;

    protocol::request::WriteEnvelopeV1 r;
    r.set_path("/bucketA/set0");
    r.set_acknowledge(false);
    Envelope e, w;
    r.add_to_envelope(w);
    string serialized;
    w.Serialize(serialized);
    r.add_envelope(serialized);
    r.add_to_envelope(e);
    ASSERT_TRUE(RequestProcessor::DEFERRED == this->p->ProcessRequest(e, output));
    EXPECT_EQ(1, this->p->write_envelopes_count);
    ASSERT_EQ(0, output.size());
}

TEST_F(RequestProcessorTest, SubscribeStoreChangesBasic)
{
    vector<Envelope> output;

    protocol::request::SubscribeStoreChangesV1 r;
    r.add_path("/");
    Envelope e;
    r.add_to_envelope(e);

    EXPECT_TRUE(RequestProcessor::AUTHORITATIVE == this->p->ProcessRequest(e, output));
    EXPECT_EQ(1, output.size());
    EXPECT_ENVELOPE_MESSAGE(0, protocol::response::SubscriptionAcceptAckV1);
    protocol::response::SubscriptionAcceptAckV1 *m = (protocol::response::SubscriptionAcceptAckV1 *)output[0].GetMessage(0);
    EXPECT_TRUE(m->has_id());
    EXPECT_TRUE(m->has_ttl());
    EXPECT_EQ(1, this->p->subscribe_store_changes_count);
}

TEST_F(RequestProcessorTest, SubscribeStoreChangesInvalidPath)
{
    vector<Envelope> output;

    protocol::request::SubscribeStoreChangesV1 r;
    r.add_path("foo");
    Envelope e;
    r.add_to_envelope(e);

    RequestProcessor::ResponseStatus result = this->p->ProcessRequest(e, output);
    this->ExpectErrorResponse(result, protocol::response::INVALID_PATH, output);
}

TEST_F(RequestProcessorTest, SubscribeEnvelopesBasic)
{
    vector<Envelope> output;

    protocol::request::SubscribeEnvelopesV1 r;
    r.add_path("/");
    Envelope e;
    r.add_to_envelope(e);

    EXPECT_TRUE(RequestProcessor::AUTHORITATIVE == this->p->ProcessRequest(e, output));
    EXPECT_EQ(1, output.size());
    EXPECT_ENVELOPE_MESSAGE(0, protocol::response::SubscriptionAcceptAckV1);
    protocol::response::SubscriptionAcceptAckV1 *m = (protocol::response::SubscriptionAcceptAckV1 *)output[0].GetMessage(0);
    EXPECT_TRUE(m->has_id());
    EXPECT_TRUE(m->has_ttl());
    EXPECT_EQ(1, this->p->handle_subscribe_envelopes_count);

}