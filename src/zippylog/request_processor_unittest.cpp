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
#include <zippylog/protocol/request.pb.h>

#include <gtest/gtest.h>
#include <zmq.hpp>

using namespace ::zippylog;

using ::std::string;
using ::std::vector;
using ::zmq::message_t;

/// request processor implementation that we use to test things
class TestRequestProcessor : public RequestProcessor
{
public:
    TestRequestProcessor(RequestProcessorStartParams &params) :
      RequestProcessor(params),
      subscribe_store_changes_count(0),
      handle_subscribe_envelopes_count(0),
      handle_subscribe_keepalive_count(0)
    {
    }

protected:
    int subscribe_store_changes_count;
    int handle_subscribe_envelopes_count;
    int handle_subscribe_keepalive_count;

    void Reset()
    {
        this->subscribe_store_changes_count = 0;
        this->handle_subscribe_envelopes_count = 0;
        this->handle_subscribe_keepalive_count = 0;
    }

    ResponseStatus HandleSubscribeStoreChanges(Envelope &request, vector<Envelope> &output)
    {
        this->subscribe_store_changes_count++;
        return AUTHORITATIVE;
    }

    ResponseStatus HandleSubscribeEnvelopes(Envelope &request, vector<Envelope> &output)
    {
        this->handle_subscribe_envelopes_count++;
        return AUTHORITATIVE;
    }

    ResponseStatus HandleSubscribeKeepalive(Envelope &request, vector<Envelope> &output)
    {
        this->handle_subscribe_keepalive_count++;
        return AUTHORITATIVE;
    }

};

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

    void ResetProcessor()
    {
        if (this->p) delete this->p;

        RequestProcessorStartParams params;
        params.active = &this->active;
        params.store_path = "simpledirectory://test/stores/00-simple";
        params.ctx = &this->ctx;
        params.logger_endpoint = "inproc://logger";
        params.client_endpoint = "inproc://client";

        this->p = new TestRequestProcessor(params);
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

        ASSERT_EQ(protocol::response::Error::zippylog_namespace, response.MessageNamespace(0));
        ASSERT_EQ(protocol::response::Error::zippylog_enumeration, response.MessageType(0));

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

// this test verifies our core message processing routine is robust
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
    input.push_back(&m);
    code = protocol::response::UNKNOWN_MESSAGE_FORMAT_VERSION;
    this->p->ProcessMessages(identities, input, output);
    this->ExpectErrorResponse(code, output);
    output.clear();

    m.rebuild(1);
    *(char *)(m.data()) = 0x02;
    input.push_back(&m);
    code = protocol::response::UNKNOWN_MESSAGE_FORMAT_VERSION;
    this->p->ProcessMessages(identities, input, output);
    this->ExpectErrorResponse(code, output);
    output.clear();

    // no data after version
    m.rebuild(1);
    *(char *)(m.data()) = 0x01;
    input.push_back(&m);
    code = protocol::response::PROTOCOL_NO_ENVELOPE;
    this->p->ProcessMessages(identities, input, output);
    this->ExpectErrorResponse(code, output);
    output.clear();

    // bad envelope after version
    m.rebuild(10);
    *(char *)(m.data()) = 0x01;
    input.push_back(&m);
    code = protocol::response::ENVELOPE_PARSE_FAILURE;
    this->p->ProcessMessages(identities, input, output);
    this->ExpectErrorResponse(code, output);
    output.clear();
}

TEST_F(RequestProcessorTest, SupportedVersions)
{
    vector<Envelope> msgs;
    protocol::response::ErrorCode code = protocol::response::UNSUPPORTED_OPERATION_MESSAGE_VERSION;

    {
        protocol::request::GetStoreInfo m;
        m.set_version(2);
        Envelope e;
        m.add_to_envelope(&e);

        this->ExpectErrorResponse(this->p->ProcessRequest(e, msgs), code, msgs);
        msgs.clear();
    }

    {
        protocol::request::GetBucketInfo m;
        m.set_version(2);
        Envelope e;
        m.add_to_envelope(&e);

        this->ExpectErrorResponse(this->p->ProcessRequest(e, msgs), code, msgs);
        msgs.clear();
    }

    {
        protocol::request::GetStreamSetInfo m;
        m.set_version(2);
        Envelope e;
        m.add_to_envelope(&e);

        this->ExpectErrorResponse(this->p->ProcessRequest(e, msgs), code, msgs);
        msgs.clear();
    }

    {
        protocol::request::GetStreamInfo m;
        m.set_version(2);
        Envelope e;
        m.add_to_envelope(&e);

        this->ExpectErrorResponse(this->p->ProcessRequest(e, msgs), code, msgs);
        msgs.clear();
    }

}

TEST_F(RequestProcessorTest, StoreInfo)
{
    protocol::request::GetStoreInfo m;
    Envelope e;
    m.add_to_envelope(&e);

    vector<Envelope> msgs;
    RequestProcessor::ResponseStatus result = this->p->ProcessRequest(e, msgs);
    EXPECT_TRUE(::zippylog::RequestProcessor::AUTHORITATIVE == result);

    EXPECT_EQ(1, msgs.size());
    Envelope response = msgs[0];
    EXPECT_EQ(1, response.MessageCount());

    // work around taking references of undefined static const enumeration class members
    // TODO remove if we have a better solution in the future
    uint32 expected = protocol::StoreInfo::zippylog_namespace;
    ASSERT_EQ(expected, response.MessageNamespace(0));
    expected = protocol::StoreInfo::zippylog_enumeration;
    ASSERT_EQ(expected, response.MessageType(0));

    protocol::StoreInfo *r = (protocol::StoreInfo *)response.GetMessage(0);
    ASSERT_TRUE(r != NULL);
    EXPECT_EQ(2, r->bucket_size());

    protocol::StoreInfo esi;
    EXPECT_TRUE(this->store->StoreInfo(esi));
    EXPECT_STREQ(esi.SerializeAsString().c_str(), r->SerializeAsString().c_str());
}

TEST_F(RequestProcessorTest, BucketInfo)
{
    // an empty request should result in missing field

    protocol::request::GetBucketInfo m1;
    Envelope e1;
    m1.add_to_envelope(&e1);

    vector<Envelope> output;
    this->ExpectErrorResponse(this->p->ProcessRequest(e1, output), protocol::response::EMPTY_FIELD, output);
    output.clear();

    // path does not contain bucket
    protocol::request::GetBucketInfo m2;
    m2.set_path("/");
    Envelope e2;
    m2.add_to_envelope(&e2);
    this->ExpectErrorResponse(this->p->ProcessRequest(e2, output), protocol::response::INVALID_PATH, output);
    output.clear();

    // this should work
    protocol::request::GetBucketInfo m3;
    m3.set_path("/bucketA");
    Envelope e3;
    m3.add_to_envelope(&e3);
    ASSERT_TRUE(::zippylog::RequestProcessor::AUTHORITATIVE == this->p->ProcessRequest(e3, output));
    ASSERT_EQ(1, output.size());
    Envelope response = output[0];
    ASSERT_EQ(1, response.MessageCount());
    uint32 expected = protocol::BucketInfo::zippylog_namespace;
    ASSERT_EQ(expected, response.MessageNamespace(0));
    expected = protocol::BucketInfo::zippylog_enumeration;
    ASSERT_EQ(expected, response.MessageType(0));
    protocol::BucketInfo *m = (protocol::BucketInfo *)response.GetMessage(0);
    ASSERT_TRUE(m != NULL);
    protocol::BucketInfo ebi;
    ASSERT_TRUE(this->store->BucketInfo("bucketA", ebi));
    ASSERT_STREQ(ebi.SerializeAsString().c_str(), m->SerializeAsString().c_str());
    output.clear();

    // bucket doesn't exist
    protocol::request::GetBucketInfo m4;
    m4.set_path("/DOESNOTEXIST");
    Envelope e4;
    m4.add_to_envelope(&e4);
    this->ExpectErrorResponse(this->p->ProcessRequest(e4, output), protocol::response::PATH_NOT_FOUND, output);
    output.clear();
}