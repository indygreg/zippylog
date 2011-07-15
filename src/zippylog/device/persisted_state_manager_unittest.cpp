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
#include <zippylog/testing.hpp>

#include <string>

using ::std::invalid_argument;
using ::std::string;

namespace zippylog {

class PersistedStateManagerTest : public ::zippylog::testing::TestBase
{ };

TEST_F(PersistedStateManagerTest, Constructor)
{
    PersistedStateManagerStartParams p;

    EXPECT_THROW(PersistedStateManager m(p), invalid_argument);

    p.store_uri = this->GetTemporaryStoreUri();
    EXPECT_NO_THROW(PersistedStateManager m(p));
}

TEST_F(PersistedStateManagerTest, IsPathSubscribed)
{
    SubscriptionInfo s(1000);
    s.paths.push_back("/");
    s.type = SubscriptionInfo::STORE_CHANGE;

    EXPECT_TRUE(PersistedStateManager::IsPathSubscribed("/", s));
    EXPECT_TRUE(PersistedStateManager::IsPathSubscribed("/foo", s));
    EXPECT_TRUE(PersistedStateManager::IsPathSubscribed("/foo/bar", s));
    EXPECT_TRUE(PersistedStateManager::IsPathSubscribed("/foo/bar/stream", s));

    s.paths.clear();
    s.paths.push_back("/foo");
    EXPECT_FALSE(PersistedStateManager::IsPathSubscribed("/", s));
    EXPECT_TRUE(PersistedStateManager::IsPathSubscribed("/foo", s));
    EXPECT_TRUE(PersistedStateManager::IsPathSubscribed("/foo/bar", s));
    EXPECT_TRUE(PersistedStateManager::IsPathSubscribed("/foo/bar/stream", s));

    s.paths.push_back("/bar");
    EXPECT_FALSE(PersistedStateManager::IsPathSubscribed("/", s));
    EXPECT_TRUE(PersistedStateManager::IsPathSubscribed("/foo", s));
    EXPECT_TRUE(PersistedStateManager::IsPathSubscribed("/foo/bar", s));
    EXPECT_TRUE(PersistedStateManager::IsPathSubscribed("/bar", s));
    EXPECT_TRUE(PersistedStateManager::IsPathSubscribed("/bar/foo", s));
}

} // namespaces
