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
#include <vector>

using ::std::invalid_argument;
using ::std::string;
using ::std::vector;

namespace zippylog {

class PersistedStateManagerTest : public ::zippylog::testing::TestBase
{
public:
    PersistedStateManager * GetManagerForTemporary()
    {
        PersistedStateManagerStartParams p;
        p.store_uri = this->GetTemporaryStoreUri();

        PersistedStateManager *m = new PersistedStateManager(p);

        this->managers.push_back(m);

        return m;
    }

protected:
    virtual void TearDown()
    {
        vector<PersistedStateManager *>::iterator i = this->managers.begin();
        for (; i != this->managers.end(); ++i) {
            delete *i;
        }

        this->managers.clear();

        TestBase::TearDown();
    }

    vector<PersistedStateManager *> managers;
};

TEST_F(PersistedStateManagerTest, Constructor)
{
    PersistedStateManagerStartParams p;

    EXPECT_THROW(PersistedStateManager m(p), invalid_argument);

    p.store_uri = this->GetTemporaryStoreUri();
    EXPECT_NO_THROW(PersistedStateManager m(p));
}

TEST_F(PersistedStateManagerTest, IsPathSubscribed)
{
    SubscriptionInfo si;
    si.paths.push_back("/");
    si.type = STORE_CHANGE;
    PersistedStateManagerSubscriptionRecord s(si, 1000);

    EXPECT_TRUE(PersistedStateManager::IsPathSubscribed("/", s));
    EXPECT_TRUE(PersistedStateManager::IsPathSubscribed("/foo", s));
    EXPECT_TRUE(PersistedStateManager::IsPathSubscribed("/foo/bar", s));
    EXPECT_TRUE(PersistedStateManager::IsPathSubscribed("/foo/bar/stream", s));

    si.paths.clear();
    si.paths.push_back("/foo");
    PersistedStateManagerSubscriptionRecord s2(si, 1000);
    EXPECT_FALSE(PersistedStateManager::IsPathSubscribed("/", s2));
    EXPECT_TRUE(PersistedStateManager::IsPathSubscribed("/foo", s2));
    EXPECT_TRUE(PersistedStateManager::IsPathSubscribed("/foo/bar", s2));
    EXPECT_TRUE(PersistedStateManager::IsPathSubscribed("/foo/bar/stream", s2));

    si.paths.push_back("/bar");
    PersistedStateManagerSubscriptionRecord s3(si, 1000);
    EXPECT_FALSE(PersistedStateManager::IsPathSubscribed("/", s3));
    EXPECT_TRUE(PersistedStateManager::IsPathSubscribed("/foo", s3));
    EXPECT_TRUE(PersistedStateManager::IsPathSubscribed("/foo/bar", s3));
    EXPECT_TRUE(PersistedStateManager::IsPathSubscribed("/bar", s3));
    EXPECT_TRUE(PersistedStateManager::IsPathSubscribed("/bar/foo", s3));
}

TEST_F(PersistedStateManagerTest, RegisterSubscription)
{
    PersistedStateManager *m = this->GetManagerForTemporary();

    SubscriptionInfo s;

    EXPECT_THROW(m->RegisterSubscription(s), invalid_argument) << "subscription with no data should be rejected";
}

TEST_F(PersistedStateManagerTest, HasSubscription)
{
    PersistedStateManager *m = this->GetManagerForTemporary();

    EXPECT_FALSE(m->HasSubscription("foo"));
    EXPECT_FALSE(m->HasSubscription(""));
    EXPECT_FALSE(m->HaveStoreChangeSubscriptions());
    EXPECT_FALSE(m->HaveStoreChangeSubscriptions("/"));

    SubscriptionInfo s;
    s.paths.push_back("/bucket");
    s.type = STORE_CHANGE;
    s.id = "foo";

    m->RegisterSubscription(s);

    EXPECT_TRUE(m->HasSubscription("foo"));
    EXPECT_TRUE(m->HaveStoreChangeSubscriptions());
    EXPECT_FALSE(m->HaveStoreChangeSubscriptions("/"));
    EXPECT_TRUE(m->HaveStoreChangeSubscriptions("/bucket"));
    EXPECT_FALSE(m->HaveStoreChangeSubscriptions("/foo"));
}

TEST_F(PersistedStateManagerTest, SubscriptionExpiration)
{
    PersistedStateManager *m = this->GetManagerForTemporary();
    m->subscription_ttl = 100; // milliseconds

    string id = "foo";

    SubscriptionInfo s;
    s.paths.push_back("/");
    s.type = STORE_CHANGE;
    s.id = id;

    m->RegisterSubscription(s);
    EXPECT_TRUE(m->HasSubscription(id));
    EXPECT_EQ(0, m->RemoveExpiredSubscriptions()) << "0 subscriptions removed";

    platform::sleep(110);
    EXPECT_TRUE(m->HasSubscription(id));
    EXPECT_EQ(1, m->RemoveExpiredSubscriptions()) << "subscription removed after expiration";
    EXPECT_FALSE(m->HasSubscription(id));


}

} // namespaces
