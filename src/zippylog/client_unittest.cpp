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

#include <zippylog/client.hpp>
#include <zippylog/device/server.hpp>

#include <gtest/gtest.h>

using ::std::invalid_argument;
using ::std::string;
using ::std::vector;
using ::zippylog::device::Server;
using ::zippylog::device::ServerStartParams;
using ::zippylog::Store;
using ::zmq::context_t;

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

        ~ClientTest() {
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
};

TEST_F(ClientTest, StartParamValidation)
{
    ASSERT_TRUE(true);
}

TEST_F(ClientTest, SynchronousTimeout)
{
    string endpoint;
    Server *s = this->GetServer("simpledirectory://test/stores/02-multiple", endpoint);
    Client c(this->GetContext(), endpoint);

    protocol::StoreInfo si;
    // it is not reasonable to expect the server to respond in 1 microsecond
    // although, it could be possible
    EXPECT_FALSE(c.StoreInfo(si, 1));
}

TEST_F(ClientTest, StoreInfoSynchronous)
{
    string endpoint;
    string store_path = "simpledirectory://test/stores/00-simple";
    Server *s = this->GetServer(store_path, endpoint);
    Store *store = this->GetStore(store_path);

    Client c(this->GetContext(), endpoint);

    protocol::StoreInfo si;
    ASSERT_TRUE(c.StoreInfo(si));
    protocol::StoreInfo expected;
    ASSERT_TRUE(store->StoreInfo(expected));

    ASSERT_TRUE(si.SerializeAsString() == expected.SerializeAsString());
}

TEST_F(ClientTest, GetSynchronous)
{
    string endpoint;
    string store_path = "simpledirectory://test/stores/01-singlestream";
    Server *s = this->GetServer(store_path, endpoint);
    Store *store = this->GetStore(store_path);

    Client c(this->GetContext(), endpoint);

    StreamSegment segment;
    ASSERT_TRUE(c.Get("/A/B/2010-11-26-07", 0, segment, 5000000));

    EXPECT_EQ(segment.EnvelopesSent, segment.Envelopes.size());
    EXPECT_EQ(160, segment.EnvelopesSent);
}

TEST_F(ClientTest, PingSynchronous)
{
    string endpoint;
    string store_path = "simpledirectory://test/stores/01-singlestream";
    Server *s = this->GetServer(store_path, endpoint);

    Client c(this->GetContext(), endpoint);

    EXPECT_TRUE(c.Ping());
}

}} // namespaces
