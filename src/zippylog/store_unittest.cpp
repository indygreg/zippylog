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

#include "zippylog/store.hpp"

#include <gtest/gtest.h>

#include <algorithm>
#include <vector>

using ::std::sort;
using ::std::string;
using ::std::vector;
using ::zippylog::Store;

TEST(StoreTest, PathValidation) {
    EXPECT_TRUE(Store::ValidatePath("/"));
    EXPECT_FALSE(Store::ValidatePath(""));
    EXPECT_FALSE(Store::ValidatePath("/a//"));
    EXPECT_FALSE(Store::ValidatePath("/a/../b"));
    EXPECT_FALSE(Store::ValidatePath("/a/b//"));
    EXPECT_FALSE(Store::ValidatePath("a/b/c"));

    EXPECT_TRUE(Store::ValidatePath("/a"));
    EXPECT_TRUE(Store::ValidatePath("/a/"));
    EXPECT_TRUE(Store::ValidatePath("/a/b"));
    EXPECT_TRUE(Store::ValidatePath("/a/b/"));
    EXPECT_TRUE(Store::ValidatePath("/a/b/c"));
    EXPECT_TRUE(Store::ValidatePath("/1/2/3"));
    EXPECT_TRUE(Store::ValidatePath("/A/B/C"));
    EXPECT_TRUE(Store::ValidatePath("/a/b/c.zippylog"));
}

TEST(StoreTest, PathParsing) {
    string path;
    string b, ss, s;

    path = "/";
    EXPECT_TRUE(Store::ParsePath(path, b, ss, s));
    EXPECT_TRUE(b.empty());
    EXPECT_TRUE(ss.empty());
    EXPECT_TRUE(s.empty());

    path = "/A";
    EXPECT_TRUE(Store::ParsePath(path, b, ss, s));
    EXPECT_STREQ("A", b.c_str());
    EXPECT_TRUE(ss.empty());
    EXPECT_TRUE(s.empty());

    path = "/bucketA";
    EXPECT_TRUE(Store::ParsePath(path, b, ss, s));
    EXPECT_STREQ("bucketA", b.c_str());
    EXPECT_TRUE(ss.empty());
    EXPECT_TRUE(s.empty());

    path = "/bucketA/";
    EXPECT_TRUE(Store::ParsePath(path, b, ss, s));
    EXPECT_STREQ("bucketA", b.c_str());
    EXPECT_TRUE(ss.empty());
    EXPECT_TRUE(s.empty());

    path = "/bucketA/setA";
    EXPECT_TRUE(Store::ParsePath(path, b, ss, s));
    EXPECT_STREQ("bucketA", b.c_str());
    EXPECT_STREQ("setA", ss.c_str());
    EXPECT_TRUE(s.empty());

    path = "/bucketA/setA/";
    EXPECT_TRUE(Store::ParsePath(path, b, ss, s));
    EXPECT_STREQ("bucketA", b.c_str());
    EXPECT_STREQ("setA", ss.c_str());
    EXPECT_TRUE(s.empty());

    path = "/bucketA/setA/streamA";
    EXPECT_TRUE(Store::ParsePath(path, b, ss, s));
    EXPECT_STREQ("bucketA", b.c_str());
    EXPECT_STREQ("setA", ss.c_str());
    EXPECT_STREQ("streamA", s.c_str());

    path = "/bucketA/setA/streamA/";
    EXPECT_FALSE(Store::ParsePath(path, b, ss, s));

    path = "/bucketA/setA/streamA.zippylog";
    EXPECT_TRUE(Store::ParsePath(path, b, ss, s));
    EXPECT_STREQ("bucketA", b.c_str());
    EXPECT_STREQ("setA", ss.c_str());
    EXPECT_STREQ("streamA", s.c_str());
}

TEST(StoreTest, StreamNaming)
{
    EXPECT_STREQ("2010-07-27", Store::StreamNameForTime(1280210699000000, 86400).c_str());
    EXPECT_STREQ("2010-07-27-06", Store::StreamNameForTime(1280212529000000, 3600).c_str());
    EXPECT_STREQ("2010-07-28-00-001-060", Store::StreamNameForTime(1280275200000000, 60).c_str());
    EXPECT_STREQ("2010-07-28-00-002-060", Store::StreamNameForTime(1280275260000000, 60).c_str());
}

class StoreContentsTest : public ::testing::Test {
protected:
    Store store;

    StoreContentsTest() : store("test/stores/00-simple") { }

};

// verifies that we find files in stores properly
TEST_F(StoreContentsTest, ContentDiscovery)
{
    ASSERT_STREQ("test/stores/00-simple", store.StorePath().c_str());

    vector<string> buckets;
    EXPECT_TRUE(store.BucketNames(buckets));
    EXPECT_EQ(2, buckets.size());

    sort(buckets.begin(), buckets.end());
    EXPECT_STREQ("bucketA", buckets[0].c_str());
    EXPECT_STREQ("bucketB", buckets[1].c_str());

    EXPECT_TRUE(store.BucketNames(buckets));
    EXPECT_EQ(2, buckets.size());

    vector<string> paths;
    EXPECT_TRUE(store.BucketPaths(paths));
    EXPECT_EQ(2, paths.size());

    sort(paths.begin(), paths.end());
    EXPECT_STREQ("/bucketA", paths[0].c_str());
    EXPECT_STREQ("/bucketB", paths[1].c_str());

    EXPECT_TRUE(store.BucketExists("bucketA"));
    EXPECT_TRUE(store.BucketExists("bucketB"));

    vector<string> sets;
    EXPECT_TRUE(store.StreamSetNames("bucketA", sets));
    EXPECT_EQ(3, sets.size());
    sort(sets.begin(), sets.end());
    EXPECT_STREQ("set0", sets[0].c_str());
    EXPECT_STREQ("set1", sets[1].c_str());
    EXPECT_STREQ("set2", sets[2].c_str());
    EXPECT_TRUE(store.StreamsetExists("bucketA", "set0"));

    EXPECT_TRUE(store.StreamSetNames("bucketB", sets));
    EXPECT_EQ(4, sets.size());
    sort(sets.begin(), sets.end());
    EXPECT_STREQ("b", sets[0].c_str());
    EXPECT_TRUE(store.StreamsetExists("bucketB", "b"));

    EXPECT_TRUE(store.StreamsetPaths(paths));
    EXPECT_EQ(7, paths.size());
    sort(paths.begin(), paths.end());
    EXPECT_STREQ("/bucketA/set0", paths[0].c_str());
    EXPECT_STREQ("/bucketA/set1", paths[1].c_str());
    EXPECT_STREQ("/bucketA/set2", paths[2].c_str());
    EXPECT_STREQ("/bucketB/b", paths[3].c_str());
    EXPECT_STREQ("/bucketB/set4", paths[4].c_str());
    EXPECT_STREQ("/bucketB/set5", paths[5].c_str());
    EXPECT_STREQ("/bucketB/set6", paths[6].c_str());
}