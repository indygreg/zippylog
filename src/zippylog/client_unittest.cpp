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

/// This file contains unit tests for

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
using ::zippylog::platform::UUID;
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

        ClientTest() :
            server(NULL),
            store(NULL),
            ctx(2)
        { }

        virtual ~ClientTest() {
            if (this->server) {
                this->server->Shutdown();
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
            p1.listen_endpoints.push_back("inproc://test00");
            endpoint = "inproc://test00";
            p1.store_path = store_path;
            p1.ctx = &this->ctx;

            // disable log writing
            p1.log_bucket.clear();
            p1.log_stream_set.clear();

            this->server = new Server(p1);
            server->RunAsync();

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

        static void PingCallback(Client *c, void *d)
        {
            EXPECT_TRUE(c != NULL);
        }
};

class ClientSendingTest : public ClientTest
{
public:
    ClientSendingTest() :
        client(NULL),
        socket(NULL)
    { }

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

        for (int i = 0; i < messages.size(); i++) {
            delete this->messages[i];
            this->messages[i] = NULL;
        }
    }

    Client * GetClient() {
        if (!this->client) {
            UUID id;
            EXPECT_TRUE(::zippylog::platform::CreateUUID(id));

            string s;
            EXPECT_TRUE(true, ::zippylog::platform::FormatUUID(id, s));

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

        ASSERT_EQ(1, ::zmq::poll(&pollitem, 1, 1000000))
            << "Request message sent in orderly manner";

        vector<string> identities;

        ASSERT_TRUE(::zippylog::zeromq::receive_multipart_message(this->socket, identities, this->messages));

        ASSERT_EQ(1, identities.size()) << "client sent identities as part of request";
        ASSERT_EQ(1, this->messages.size()) << "client sent 1 message with content";

        message_t msg;
        msg.copy(this->messages[0]);

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

protected:
    Client *client;
    socket_t *socket;
    vector<message_t *> messages;
};

TEST_F(ClientTest, StartParamValidation)
{
    ASSERT_TRUE(true);
}

TEST_F(ClientTest, SynchronousTimeout)
{
    string endpoint;
    this->GetServer("simpledirectory://test/stores/02-multiple", endpoint);
    Client c(this->GetContext(), endpoint);

    protocol::StoreInfoV1 si;
    // it is not reasonable to expect the server to respond in 1 microsecond
    // although, it could be possible
    EXPECT_FALSE(c.GetStoreInfo(si, 1));
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
    ASSERT_TRUE(c.GetStreamSegment("/A/B/2010-11-26-07", 0, segment, 5000000));

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

TEST_F(ClientSendingTest, Ping)
{
    Client *c = this->GetClient();
    c->Ping(ClientSendingTest::PingCallback, NULL);

    protocol::request::PingV1 m;

    this->ExpectRequestMessage(protocol::request::PingV1::zippylog_enumeration, m);
}

}} // namespaces
