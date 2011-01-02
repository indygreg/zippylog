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

#include <zippylog/envelope.hpp>

#include <gtest/gtest.h>
#include <string.h>

using ::std::invalid_argument;
using ::std::string;
using ::zippylog::Envelope;
using ::zmq::message_t;

namespace zippylog {

TEST(EnvelopeTest, EmptyConstructor)
{
    ASSERT_NO_THROW(Envelope e());

    Envelope e;
    EXPECT_EQ(0, e.MessageCount());
    string s;
    EXPECT_TRUE(e.Serialize(s));
}

TEST(EnvelopeTest, InvalidDataConstruct)
{
    ASSERT_THROW(Envelope e(NULL, 0), invalid_argument);
    ASSERT_THROW(Envelope e(NULL, 10), invalid_argument);
    ASSERT_THROW(Envelope e((void *)324234, 0), invalid_argument);
    ASSERT_THROW(Envelope e((void *)352537, -10), invalid_argument);
}

TEST(EnvelopeTest, ZmqMessageConstructor)
{
    message_t m;

    // empty message
    EXPECT_THROW(Envelope e(m), invalid_argument);

    // offset >= size
    m.rebuild(10);
    EXPECT_THROW(Envelope e(m, 9), invalid_argument);

    Envelope e;
    ASSERT_TRUE(e.ToZmqMessage(m));
    EXPECT_NO_THROW(Envelope e2(m));

    message_t m2(m.size() + 1);
    memset(m.data(), 1, 1);
    memcpy((void *)((char *)m2.data() + 1), m.data(), m.size());
    EXPECT_NO_THROW(Envelope e2(m2, 1));
}

TEST(EnvelopeTest, Serialize)
{
    Envelope e;
    string s;
    ASSERT_TRUE(e.Serialize(s));

    string s2 = "foo";
    ASSERT_TRUE(e.Serialize(s2));
    ASSERT_EQ(3 + s.length(), s2.length());
}

TEST(EnvelopeTest, ZMQSerialization)
{
    Envelope e;

    string expected;
    ASSERT_TRUE(e.Serialize(expected));
    message_t m;

    ASSERT_TRUE(e.ToZmqMessage(m));
    ASSERT_EQ(expected.length(), m.size());
    EXPECT_TRUE(0 == memcmp(m.data(), expected.data(), m.size()));
}

TEST(EnvelopeTest, ZMQProtocolSerialization)
{
    Envelope e;

    string expected(1, 0x01);
    ASSERT_TRUE(e.Serialize(expected));
    message_t m;
    ASSERT_TRUE(e.ToProtocolZmqMessage(m));
    ASSERT_EQ(expected.length(), m.size());
    EXPECT_EQ(0x01, *((char *)m.data()));
    EXPECT_TRUE(0 == memcmp(m.data(), expected.data(), m.size()));
}

} // namespace
