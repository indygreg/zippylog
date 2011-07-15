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

#include <zippylog/device/server.hpp>
#include <zippylog/client.hpp>

#include <gtest/gtest.h>

using ::std::invalid_argument;
using ::std::string;
using ::std::vector;
using ::zippylog::client::Client;
using ::zippylog::client::SubscriptionCallbackInfo;
using ::zmq::context_t;
using ::zmq::message_t;
using ::zmq::socket_t;

namespace zippylog {
namespace device {
namespace server {

string SANDBOX_PATH = "test/stores/sandbox";

class ServerTest : public ::testing::Test
{
    protected:
        Server * test00_server;
        Server * sandbox_server;
        Store * sandbox_store;
        vector<socket_t *> sockets;
        vector<Client *> clients;
        context_t ctx;

        ServerTest() :
            test00_server(NULL),
            sandbox_server(NULL),
            sandbox_store(NULL),
            ctx(3)
        { }

        ~ServerTest()
        {
            if (this->test00_server) delete test00_server;

            if (this->sandbox_server) {
                delete this->sandbox_server;

                // and clean up all touched files
                platform::RemoveDirectory(SANDBOX_PATH);
            }

            for (size_t i = 0; i < this->sockets.size(); i++) {
                if (this->sockets[i]) delete this->sockets[i];
            }

            for (size_t i = 0; i < this->clients.size(); i++) {
                if (this->clients[i]) delete this->clients[i];
            }

            if (this->sandbox_store) delete this->sandbox_store;
        }

        Server * GetTest00Server()
        {
            if (this->test00_server) {
                delete this->test00_server;
                this->test00_server = NULL;
            }

            ServerStartParams p1;
            p1.listen_endpoints.push_back("inproc://test00");
            p1.store_path = "simpledirectory://test/stores/00-simple";
            p1.ctx = &this->ctx;

            // disable log writing
            p1.log_bucket.clear();
            p1.log_stream_set.clear();

            this->test00_server = new Server(p1);
            return test00_server;
        }

        Server * GetSandboxServer()
        {
            if (this->sandbox_server) {
                delete this->sandbox_server;
                this->sandbox_server = NULL;
            }

            if (!platform::PathIsDirectory(SANDBOX_PATH)) {
                EXPECT_TRUE(platform::MakeDirectory(SANDBOX_PATH));
            }

            ServerStartParams p;
            p.listen_endpoints.push_back("inproc://sandbox");
            p.store_path = "simpledirectory://" + SANDBOX_PATH;
            p.ctx = &this->ctx;
            p.persisted_state_reactor_threads = 1;

            p.log_bucket.clear();
            p.log_stream_set.clear();

            this->sandbox_server = new Server(p);
            return this->sandbox_server;
        }

        socket_t * GetClientSocket(Server *s, int type = ZMQ_REQ)
        {
            EXPECT_TRUE(s->ClientEndpoints().size() > 0);
            string endpoint = s->ClientEndpoints().at(0);

            socket_t * result = new socket_t(this->ctx, type);
            this->sockets.push_back(result);
            result->connect(endpoint.c_str());

            return result;
        }

        Client * GetClient(Server *s)
        {
            EXPECT_TRUE(s->ClientEndpoints().size() > 0);
            string endpoint = s->ClientEndpoints().at(0);

            Client *c = new Client(&this->ctx, endpoint);

            this->clients.push_back(c);

            return c;
        }

        Store * GetSandboxStore()
        {
            if (!this->sandbox_store) {
                this->sandbox_store = Store::CreateStore("simpledirectory://" + SANDBOX_PATH);
            }

            return this->sandbox_store;
        }
};

TEST_F(ServerTest, StartParamValidation)
{
    ServerStartParams p1;
    EXPECT_THROW(Server s(p1), invalid_argument);

    p1.listen_endpoints.push_back("inproc://test00");
    EXPECT_THROW(Server s(p1), invalid_argument);
}

TEST_F(ServerTest, ConstructBadStore)
{
    ServerStartParams p1;
    p1.listen_endpoints.push_back("inproc://test00");

    p1.store_path = "foo";
    EXPECT_THROW(Server s(p1), InvalidStoreUriException);

    p1.store_path = "foo://bad";
    EXPECT_THROW(Server s(p1), UnsupportedStoreUriException);

    p1.store_path = "simpledirectory://DOESNOTEXIST";
    EXPECT_THROW(Server s(p1), StorePathNotDirectoryException);
}

TEST_F(ServerTest, ConstructSuccess)
{
    ServerStartParams p1;
    p1.listen_endpoints.push_back("inproc://test00");
    p1.store_path = "simpledirectory://test/stores/00-simple";
    EXPECT_NO_THROW(Server s(p1));

    Server s(p1);
    vector<string> endpoints = s.ClientEndpoints();
    EXPECT_EQ(1, endpoints.size());
    EXPECT_TRUE("inproc://test00" == endpoints[0]);
}

TEST_F(ServerTest, Startup)
{
    Server *s = this->GetTest00Server();
    ASSERT_NO_THROW(s->Start());
}

// Pump() should work out the gate w/o any errors
TEST_F(ServerTest, SimplePump)
{
    Server *s = this->GetTest00Server();
    s->Start();
    for (size_t i = 100; i; --i) {
        ASSERT_NE(-1, s->Pump(0));
    }
}

// verifies that we can send 0MQ request messages to the server
TEST_F(ServerTest, SimpleMessageProcessing)
{
    Server *s = this->GetTest00Server();
    s->RunAsync();

    socket_t *sock = this->GetClientSocket(s);
    ASSERT_TRUE(sock != NULL);

    protocol::request::GetStoreInfoV1 m;
    Envelope e;
    m.add_to_envelope(&e);

    message_t msg;
    ASSERT_TRUE(e.ToProtocolZmqMessage(msg));

    ASSERT_TRUE(sock->send(msg, 0));

    // and we wait for a response
    ASSERT_NO_THROW(sock->recv(&msg, 0));

    ASSERT_TRUE(msg.size() > 1);
    int64 more;
    size_t moresz = sizeof(more);
    sock->getsockopt(ZMQ_RCVMORE, &more, &moresz);
    EXPECT_EQ(0, more);

    // first byte is format version and should be 1
    ASSERT_EQ(0x01, *((char *)msg.data()));

    Envelope response;
    ASSERT_NO_THROW(response = Envelope(msg, 1));
    EXPECT_EQ(1, response.MessageCount());

    s->Shutdown();
}

TEST_F(ServerTest, ClientPing)
{
    Server *s = this->GetTest00Server();
    s->RunAsync();

    Client *c = this->GetClient(s);

    EXPECT_TRUE(c->Ping(100000));

    s->Shutdown();
}

void stream_added(string id, protocol::StoreChangeStreamAddedV1 &r, void *d)
{

}

void bucket_added(Client *, string id, protocol::StoreChangeBucketAddedV1 &r, void *d)
{
    uint32 *b = (uint32 *)d;
    (*b)++;
}

TEST_F(ServerTest, ClientSubscribeStoreChanges)
{
    Server *s = this->GetSandboxServer();
    s->RunAsync();

    Client *c = this->GetClient(s);

    int32 buckets_added = 0;

    SubscriptionCallbackInfo cb;
    cb.BucketAdded = bucket_added;

    /*
    c->SubscribeStoreChanges("/", cb, (void *)&buckets_added);

    /// @todo fix the sleep hackiness

    platform::sleep(1000);

    Store *st = this->GetSandboxStore();
    st->CreateBucket("foo");

    for (size_t i = 0; i < 3; i++) {
        c->Pump(100000);
    }

    EXPECT_EQ(1, buckets_added);
    */

    s->Shutdown();
}

}}} // namespaces