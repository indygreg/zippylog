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

#include <zippylog/persisted_state_manager.hpp>
#include <zippylog/platform.hpp>

#include <gtest/gtest.h>

#include <string>

using ::std::invalid_argument;
using ::std::string;

namespace zippylog {

class PersistedStateManagerTest : public ::testing::Test
{
protected:

    string id;
    string path;
    string store_uri;

    void SetUp()
    {
        this->id = platform::CreateUUID(true);
        this->path = "test/stores/" + id;
        this->store_uri = "simpledirectory://" + this->path;

        ASSERT_FALSE(platform::PathIsDirectory(this->path));
        ASSERT_TRUE(platform::MakeDirectory(this->path));
    }

    void TearDown()
    {
        if (platform::PathIsDirectory(this->path)) {
            EXPECT_TRUE(platform::RemoveDirectory(this->path));
        }
    }
};

TEST_F(PersistedStateManagerTest, Constructor)
{
    PersistedStateManagerStartParams p;

    EXPECT_THROW(PersistedStateManager m(p), invalid_argument);

    p.store_uri = this->store_uri;
    EXPECT_NO_THROW(PersistedStateManager m(p));
}

} // namespaces
