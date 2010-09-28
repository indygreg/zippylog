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

using ::std::string;
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