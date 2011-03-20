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
#include <zippylog/protocol/request.pb.h>

#include <gtest/gtest.h>
#include <string.h>

using ::std::invalid_argument;
using ::std::string;
using ::zippylog::Envelope;
using ::zippylog::protocol::request::GetStream;
using ::zmq::message_t;

namespace zippylog {

TEST(EnvelopeTest, ConstructorEmptyEnvelope)
{
    ASSERT_NO_THROW(Envelope e());

    Envelope e;
    EXPECT_EQ(0, e.MessageCount());
    string s;
    EXPECT_TRUE(e.Serialize(s));
}

TEST(EnvelopeTest, ConstructorInvalidData)
{
    ASSERT_THROW(Envelope e(NULL, 0), invalid_argument);
    ASSERT_THROW(Envelope e(NULL, 10), invalid_argument);
    ASSERT_THROW(Envelope e((void *)324234, 0), invalid_argument);
    ASSERT_THROW(Envelope e((void *)352537, -10), invalid_argument);
}

TEST(EnvelopeTest, ConstructorZmqMessage)
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

TEST(EnvelopeTest, ConstructorString)
{
    EXPECT_NO_THROW(Envelope e("hello, world"));

    Envelope e("hello, world");
    EXPECT_STREQ("hello, world", e.envelope.string_value().c_str());
}

TEST(EnvelopeTest, EquivalenceAndCopying)
{
    Envelope e1("hello, world");

    Envelope e2(e1);
    Envelope e3 = e1;

    EXPECT_TRUE(e1 == e2);
    EXPECT_TRUE(e1 == e3);
    EXPECT_TRUE(e2 == e3);

    Envelope e4;
    EXPECT_TRUE(e1 != e4);
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

TEST(EnvelopeTest, GetMessageErrors)
{
    Envelope e;

    EXPECT_THROW(e.GetMessage(-1), invalid_argument);
    EXPECT_TRUE(NULL == e.GetMessage(0));
    EXPECT_TRUE(NULL == e.GetMessage(10));
}

TEST(EnvelopeTest, UnknownMessageTypes)
{
    Envelope e;
    e.envelope.add_message("foo bar");
    e.envelope.add_message_namespace(32423432);
    e.envelope.add_message_type(89672356);

    string serialized;
    EXPECT_TRUE(e.Serialize(serialized));

    Envelope e2(serialized.data(), serialized.length());
    EXPECT_EQ(1, e2.MessageCount());
    EXPECT_TRUE(NULL == e2.GetMessage(0));
}

TEST(EnvelopeTest, MessageSemantics)
{
    Envelope e;
    GetStream get_stream;
    get_stream.set_version(1);
    get_stream.add_to_envelope(e);

    uint32 expected_namespace = GetStream::zippylog_namespace;
    uint32 expected_type = GetStream::zippylog_enumeration;

    EXPECT_EQ(1, e.MessageCount());
    EXPECT_EQ(expected_namespace, e.MessageNamespace(0));
    EXPECT_EQ(expected_type, e.MessageType(0));

    GetStream *message = (GetStream *)e.GetMessage(0);
    ASSERT_TRUE(NULL != message);

    EXPECT_EQ(get_stream.version(), message->version());

    // verify the pointer is the same
    GetStream *message2 = (GetStream *)e.GetMessage(0);
    EXPECT_EQ(message, message2);

    // verify change in original doesn't touch what's in envelope
    get_stream.set_version(2);
    EXPECT_EQ(1, message->version());
}

TEST(EnvelopeTest, MessagesAndCopying)
{
    Envelope e1;
    GetStream gs1;
    gs1.set_version(1);
    gs1.add_to_envelope(e1);

    EXPECT_TRUE(NULL != e1.GetMessage(0));

    Envelope e2 = e1;
    EXPECT_EQ(1, e2.MessageCount());
    GetStream *gs2 = (GetStream *)e2.GetMessage(0);
    ASSERT_TRUE(NULL != gs2);
    EXPECT_TRUE(&gs1 != gs2);
}

} // namespace
