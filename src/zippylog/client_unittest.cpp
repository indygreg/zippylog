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

/// This file contains unit tests for the client classes.
///
/// It is preferred to test the client using mock servers whenever possible.
/// This way, we validate the client-specific code and not the server code by
/// extension. This does mean we do a little extra work writing tests when
/// there are changes to the protocol. But, we have extra checks to verify
/// both the client and server each work as expected and we have the ability
/// to pin blame on the client or server.

#include <zippylog/client.hpp>
#include <zippylog/device/server.hpp>
#include <zippylog/platform.hpp>
#include <zippylog/zeromq.hpp>

#include <gtest/gtest.h>

using ::std::invalid_argument;
using ::std::string;
using ::std::vector;
using ::zippylog::device::Server;
using ::zippylog::device::ServerStartParams;
using ::zippylog::Store;
using ::zippylog::platform::ConditionalWait;
using ::zippylog::platform::UUID;
using ::zippylog::zeromq::MessageContainer;
using ::zmq::context_t;
using ::zmq::message_t;
using ::zmq::socket_t;

namespace zippylog {
namespace client {

class ClientTest : public ::testing::Test
{
    protected:
        Server * server;
        Store * store;
        context_t ctx;
        ConditionalWait cw;

        ClientTest() :
            server(NULL),
            store(NULL),
            ctx(2)
        { }

        virtual ~ClientTest() {
            if (this->server) {
                this->server->StopAsync();
                delete this->server;
            }

            if (this->store) delete this->store;
        }

        Server * GetServer(string store_path, string &endpoint)
        {
            if (this->server) {
                delete this->server;
                this->server = NULL;
            }

            ServerStartParams p1;
            p1.active = &this->cw;
            p1.listen_endpoints.push_back("inproc://test00");
            endpoint = "inproc://test00";
            p1.store_path = store_path;
            p1.ctx = &this->ctx;

            // disable log writing
            p1.log_bucket.clear();
            p1.log_stream_set.clear();

            this->server = new Server(p1);
            this->server->RunAsync();

            return this->server;
        }

        Store * GetStore(string path)
        {
            if (this->store) {
                delete this->store;
                this->store = NULL;
            }

            this->store = Store::CreateStore(path);

            return this->store;
        }

        context_t * GetContext() {
            return &this->ctx;
        }

        static void PingCallback(Client *c, void *)
        {
            EXPECT_TRUE(c != NULL) << "client instance is not NULL";
        }

        static void FeaturesCallback(Client *c, protocol::response::FeatureSpecificationV1 &, void *)
        {
            EXPECT_TRUE(c != NULL) << "client instance is not NULL";
        }

        static void StoreInfoCallback(Client *c, protocol::StoreInfoV1 &, void *)
        {
            EXPECT_TRUE(c != NULL) << "client instance is not NULL";
        }

        static void BucketInfoCallback(Client *c, protocol::BucketInfoV1 &, void *)
        {
            EXPECT_TRUE(c != NULL) << "client instance is not NULL";
        }

        static void StreamSetInfoCallback(Client *c, protocol::StreamSetInfoV1 &, void *)
        {
            EXPECT_TRUE(c != NULL) << "client instance is not NULL";
        }

        static void StreamInfoCallback(Client *c, protocol::StreamInfoV1 &, void *)
        {
            EXPECT_TRUE(c != NULL) << "client instance is not NULL";
        }

        static void StreamSegmentCallback(Client *c, string const &, uint64, StreamSegment &, void *)
        {
            EXPECT_TRUE(c != NULL) << "client instance is not NULL";
        }
};

class ClientSendingTest : public ClientTest
{
public:
    ClientSendingTest() :
        client(NULL),
        socket(NULL)
    {
        this->GetClient();
    }

    ~ClientSendingTest()
    {
        if (this->client != NULL) {
            delete this->client;
            this->client = NULL;
        }

        if (this->socket != NULL) {
            delete this->socket;
            this->socket = NULL;
        }
    }

    Client * GetClient() {
        if (!this->client) {
            UUID id;
            EXPECT_TRUE(::zippylog::platform::CreateUUID(id));

            string s;
            EXPECT_TRUE(::zippylog::platform::FormatUUID(id, s));

            string address = "inproc://" + s;

            this->socket = new socket_t(this->ctx, ZMQ_XREP);
            this->socket->bind(address.c_str());

            this->client = new Client(&this->ctx, address);
        }

        return this->client;
    }

    /// Validates that a protocol request message has been sent
    ///
    /// Expects the request message to have a single protocol envelope with a
    /// single embedded message of the enumeration specified and serialized to
    /// exactly what we specify
    void ExpectRequestMessage(uint32 type, ::google::protobuf::Message &expected)
    {
        ASSERT_TRUE(this->socket != NULL);

        ::zmq::pollitem_t pollitem;
        pollitem.events = ZMQ_POLLIN;
        pollitem.socket = *this->socket;
        pollitem.fd = 0;
        pollitem.revents = 0;

        ASSERT_EQ(1, ::zmq::poll(&pollitem, 1, 1000))
            << "Request message sent in orderly manner";

        MessageContainer messages;
        ASSERT_TRUE(zeromq::ReceiveMessage(*this->socket, messages));

        ASSERT_EQ(1, messages.IdentitiesSize()) << "client sent identities as part of request";
        ASSERT_EQ(1, messages.MessagesSize()) << "client sent 1 message with content";

        message_t msg;
        msg.copy(messages[0]);

        ASSERT_GT(msg.size(), 1) << "sent message has content";

        char msg_version = 0;
        memcpy(&msg_version, msg.data(), 1);
        EXPECT_EQ(0x01, msg_version) << "sent message has protocol header";

        Envelope e;

        EXPECT_NO_THROW(e = Envelope(msg, 1)) << "envelope could be deserialized";

        EXPECT_EQ(1, e.MessageCount()) << "envelope contains one message";
        EXPECT_EQ(::zippylog::message_namespace, e.MessageNamespace(0)) << "message of proper namespace";
        EXPECT_EQ(type, e.MessageType(0)) << "message of proper type";

        EXPECT_EQ(1, e.TagSize()) << "envelope has tag";

        ::google::protobuf::Message *sent = (::google::protobuf::Message *)e.GetMessage(0);
        ASSERT_TRUE(sent != NULL) << "able to deserialize embedded message";

        string serialized_expected, serialized_actual;
        EXPECT_TRUE(expected.SerializeToString(&serialized_expected));
        EXPECT_TRUE(sent->SerializeToString(&serialized_actual));

        EXPECT_EQ(serialized_expected, serialized_actual) << "sent request message matches expected";
    }

    /// Helper to respond to a subscription request message
    void RespondToSubscriptionRequest(string const &)
    {
        MessageContainer messages;
        ASSERT_TRUE(zeromq::ReceiveMessage(*this->socket, messages));

        message_t msg;
        msg.copy(messages[0]);

        Envelope request(msg, 1);

        Envelope response;
        for (int i = 0; i < request.TagSize(); i++) {
            response.AddTag(request.GetTag(i));
        }

        protocol::response::SubscriptionAcceptAckV1 ack;
        ack.set_id("foobar");
        ack.set_ttl(10000);

        ack.add_to_envelope(response);

        zeromq::SendEnvelope(*this->socket, messages.GetIdentities(), response, true, 0);
    }

protected:
    Client *client;
    socket_t *socket;
};

TEST_F(ClientTest, StartParamValidation)
{
    ASSERT_TRUE(true);
}

TEST_F(ClientSendingTest, GetBucketInfoParameterValidation)
{
    EXPECT_THROW(this->client->GetBucketInfo("", NULL), invalid_argument);
    EXPECT_THROW(this->client->GetBucketInfo("/foo", NULL), invalid_argument);
    EXPECT_THROW(this->client->GetBucketInfo("/foo/bar", ClientSendingTest::BucketInfoCallback), invalid_argument);

    protocol::BucketInfoV1 m;

    EXPECT_THROW(this->client->GetBucketInfo("", m), invalid_argument);
    EXPECT_THROW(this->client->GetBucketInfo("/foo/bar", m), invalid_argument);
}

// the next tests verify that all the functions send the appropriate request messages
TEST_F(ClientSendingTest, Ping)
{
    EXPECT_TRUE(this->client->Ping(ClientSendingTest::PingCallback, NULL));

    protocol::request::PingV1 m;

    this->ExpectRequestMessage(m.zippylog_enumeration, m);

    EXPECT_FALSE(this->client->Ping(10));
    this->ExpectRequestMessage(m.zippylog_enumeration, m);
}

TEST_F(ClientSendingTest, GetFeatures)
{
    EXPECT_TRUE(this->client->GetFeatures(ClientSendingTest::FeaturesCallback, NULL));

    protocol::request::GetFeaturesV1 m;

    this->ExpectRequestMessage(m.zippylog_enumeration, m);

    protocol::response::FeatureSpecificationV1 f;
    EXPECT_FALSE(this->client->GetFeatures(f, 10));
    this->ExpectRequestMessage(m.zippylog_enumeration, m);
}

TEST_F(ClientSendingTest, GetStoreInfo)
{
    EXPECT_TRUE(this->client->GetStoreInfo(ClientSendingTest::StoreInfoCallback, NULL));

    protocol::request::GetStoreInfoV1 m;

    this->ExpectRequestMessage(m.zippylog_enumeration, m);

    protocol::StoreInfoV1 si;
    EXPECT_FALSE(this->client->GetStoreInfo(si, 10));
    this->ExpectRequestMessage(m.zippylog_enumeration, m);
}

TEST_F(ClientSendingTest, GetBucketInfo)
{
    const string path = "/foo";

    EXPECT_TRUE(this->client->GetBucketInfo(path, ClientSendingTest::BucketInfoCallback, NULL));

    protocol::request::GetBucketInfoV1 m;
    m.set_path(path);

    this->ExpectRequestMessage(m.zippylog_enumeration, m);

    protocol::BucketInfoV1 bi;
    EXPECT_FALSE(this->client->GetBucketInfo(path, bi, 10));
    this->ExpectRequestMessage(m.zippylog_enumeration, m);
}

TEST_F(ClientSendingTest, GetStreamSetInfo)
{
    const string path = "/foo/bar";

    EXPECT_TRUE(this->client->GetStreamSetInfo(path, ClientSendingTest::StreamSetInfoCallback, NULL));

    protocol::request::GetStreamSetInfoV1 m;
    m.set_path(path);

    this->ExpectRequestMessage(m.zippylog_enumeration, m);

    protocol::StreamSetInfoV1 si;
    EXPECT_FALSE(this->client->GetStreamSetInfo(path, si, 10));
    this->ExpectRequestMessage(m.zippylog_enumeration, m);
}

TEST_F(ClientSendingTest, GetStreamInfo)
{
    const string path = "/foo/bar/stream";

    EXPECT_TRUE(this->client->GetStreamInfo(path, ClientSendingTest::StreamInfoCallback, NULL));

    protocol::request::GetStreamInfoV1 m;
    m.set_path(path);

    this->ExpectRequestMessage(protocol::request::GetStreamInfoV1::zippylog_enumeration, m);

    protocol::StreamInfoV1 si;
    EXPECT_FALSE(this->client->GetStreamInfo(path, si, 10));
    this->ExpectRequestMessage(m.zippylog_enumeration, m);
}

TEST_F(ClientSendingTest, GetStreamSegmentSimple)
{
    const string path = "/foo/bar/stream";

    EXPECT_TRUE(this->client->GetStreamSegment(path, 0, ClientSendingTest::StreamSegmentCallback));

    protocol::request::GetStreamSegmentV1 m;
    m.set_path(path);
    m.set_start_offset(0);

    this->ExpectRequestMessage(m.zippylog_enumeration, m);
}

TEST_F(ClientSendingTest, GetStreamSegmentEndOffset)
{
    const string path = "/foo/bar/stream";

    EXPECT_TRUE(this->client->GetStreamSegment(path, 0, (uint64)500, ClientSendingTest::StreamSegmentCallback));

    protocol::request::GetStreamSegmentV1 m;
    m.set_path(path);
    m.set_start_offset(0);
    m.set_max_response_bytes(500);

    this->ExpectRequestMessage(m.zippylog_enumeration, m);

    this->client->GetStreamSegment(path, 100, (uint64)500, ClientSendingTest::StreamSegmentCallback);
    m.set_start_offset(100);
    m.set_max_response_bytes(400);
    this->ExpectRequestMessage(m.zippylog_enumeration, m);
}

TEST_F(ClientSendingTest, SubscribeStoreChanges)
{
    const string path = "/foo";

    protocol::request::SubscribeStoreChangesV1 m;

    SubscriptionCallbackInfo cbi;
    SubscriptionRequestResult result;

    EXPECT_FALSE(this->client->SubscribeStoreChanges(cbi, result, 10));

    this->ExpectRequestMessage(m.zippylog_enumeration, m);

    m.add_path(path);

    EXPECT_FALSE(this->client->SubscribeStoreChanges(path, cbi, result, 10));
    this->ExpectRequestMessage(m.zippylog_enumeration, m);
}

TEST_F(ClientSendingTest, SynchronousTimeout)
{
    platform::Time start, end;
    EXPECT_TRUE(platform::TimeNow(start));

    int32 timeout = 50;

    // it doesn't matter what the timeout is since we have nothing responding
    EXPECT_FALSE(this->client->Ping(timeout)) << "synchronous requests time out";

    EXPECT_TRUE(platform::TimeNow(end));

    EXPECT_NEAR(end.epoch_micro, start.epoch_micro, timeout * 2000);
}

TEST_F(ClientTest, StoreInfoSynchronous)
{
    string endpoint;
    string store_path = "simpledirectory://test/stores/00-simple";
    this->GetServer(store_path, endpoint);
    Store *store = this->GetStore(store_path);

    Client c(this->GetContext(), endpoint);

    protocol::StoreInfoV1 si;
    ASSERT_TRUE(c.GetStoreInfo(si));
    protocol::StoreInfoV1 expected;
    ASSERT_TRUE(store->StoreInfo(expected));

    ASSERT_TRUE(si.SerializeAsString() == expected.SerializeAsString());
}

TEST_F(ClientTest, GetSynchronous)
{
    string endpoint;
    string store_path = "simpledirectory://test/stores/01-singlestream";
    this->GetServer(store_path, endpoint);
    this->GetStore(store_path);

    Client c(this->GetContext(), endpoint);

    StreamSegment segment;
    ASSERT_TRUE(c.GetStreamSegment("/A/B/2010-11-26-07", 0, segment, 5000));

    EXPECT_EQ(segment.EnvelopesSent, segment.Envelopes.size());
    EXPECT_EQ(160, segment.EnvelopesSent);
}

TEST_F(ClientTest, PingSynchronous)
{
    string endpoint;
    string store_path = "simpledirectory://test/stores/01-singlestream";
    this->GetServer(store_path, endpoint);

    Client c(this->GetContext(), endpoint);

    EXPECT_TRUE(c.Ping());
}

}} // namespaces
