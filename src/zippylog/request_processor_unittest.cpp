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

using namespace ::zippylog;

using ::std::vector;

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

    void SetUp()
    {
        this->ResetProcessor();
    }
};

TEST_F(RequestProcessorTest, StoreInfo)
{
    protocol::request::StoreInfo m;
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

