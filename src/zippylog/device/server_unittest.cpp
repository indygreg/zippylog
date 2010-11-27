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

#include <zippylog/device/server.hpp>

#include <gtest/gtest.h>

using ::std::invalid_argument;
using ::std::string;

namespace zippylog {
namespace device {
namespace server {

class ServerTest : public ::testing::Test
{
    protected:
        ServerTest() :
            test00_server(NULL)
        { }

        ~ServerTest()
        {
            if (this->test00_server) delete test00_server;
        }

        Server * test00_server;

        Server * GetTest00Server()
        {
            if (this->test00_server) {
                delete this->test00_server;
                this->test00_server = NULL;
            }

            ServerStartParams p1;
            p1.listen_endpoints.push_back("inproc://test00");
            p1.store_path = "simpledirectory://test/stores/00-simple";

            this->test00_server = new Server(p1);
            return test00_server;
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

}}} // namespaces