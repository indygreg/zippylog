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

#include <zippylog/testing.hpp>

#include <zippylog/envelope.hpp>
#include <zippylog/protocol/request.pb.h>
#include <zippylog/protocol/response.pb.h>

#include <gtest/gtest.h>
#include <string>
#include <utility>
#include <vector>

using ::google::protobuf::Message;
using ::std::invalid_argument;
using ::std::pair;
using ::std::string;
using ::std::vector;
using ::zippylog::Envelope;
using ::zippylog::protocol::request::GetStreamSegmentV1;
using ::zippylog::protocol::request::PingV1;
using ::zippylog::protocol::response::PongV1;
using ::zmq::message_t;

namespace zippylog {

class EnvelopeTest : public ::zippylog::testing::TestBase
{ };

TEST_F(EnvelopeTest, ConstructorEmptyEnvelope)
{
    ASSERT_NO_THROW(Envelope e());

    Envelope e;
    EXPECT_EQ(0, e.MessageCount());
    string s;
    EXPECT_TRUE(e.Serialize(s));
}

TEST_F(EnvelopeTest, ConstructorInvalidData)
{
    ASSERT_THROW(Envelope e(NULL, 0), invalid_argument);
    ASSERT_THROW(Envelope e(NULL, 10), invalid_argument);
    ASSERT_THROW(Envelope e((void *)324234, 0), invalid_argument);
    ASSERT_THROW(Envelope e((void *)352537, -10), invalid_argument);
}

TEST_F(EnvelopeTest, ConstructorZmqMessage)
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

TEST_F(EnvelopeTest, ConstructorString)
{
    EXPECT_NO_THROW(Envelope e("hello, world"));

    Envelope e("hello, world");
    EXPECT_EQ("hello, world", e.GetStringValueField());
}

TEST_F(EnvelopeTest, EquivalenceAndCopying)
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

TEST_F(EnvelopeTest, Serialize)
{
    Envelope e;
    string s;
    ASSERT_TRUE(e.Serialize(s));

    string s2 = "foo";
    ASSERT_TRUE(e.Serialize(s2));
    ASSERT_EQ(3 + s.length(), s2.length());
}

TEST_F(EnvelopeTest, ZMQSerialization)
{
    Envelope e;

    string expected;
    ASSERT_TRUE(e.Serialize(expected));
    message_t m;

    ASSERT_TRUE(e.ToZmqMessage(m));
    ASSERT_EQ(expected.length(), m.size());
    EXPECT_TRUE(0 == memcmp(m.data(), expected.data(), m.size()));
}

TEST_F(EnvelopeTest, ZMQProtocolSerialization)
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

TEST_F(EnvelopeTest, GetMessageErrors)
{
    Envelope e;

    EXPECT_THROW(e.GetMessage(-1), invalid_argument);
    EXPECT_TRUE(NULL == e.GetMessage(0));
    EXPECT_TRUE(NULL == e.GetMessage(10));
}

TEST_F(EnvelopeTest, UnknownMessageTypes)
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

TEST_F(EnvelopeTest, MessageSemantics)
{
    Envelope e;
    GetStreamSegmentV1 get_stream;
    get_stream.set_path("/");
    get_stream.set_start_offset(0);
    get_stream.add_to_envelope(e);

    uint32 expected_namespace = GetStreamSegmentV1::zippylog_namespace;
    uint32 expected_type = GetStreamSegmentV1::zippylog_enumeration;

    EXPECT_EQ(1, e.MessageCount());
    EXPECT_EQ(expected_namespace, e.MessageNamespace(0));
    EXPECT_EQ(expected_type, e.MessageType(0));

    GetStreamSegmentV1 *message = (GetStreamSegmentV1 *)e.GetMessage(0);
    ASSERT_TRUE(NULL != message);

    // verify the pointer is the same
    GetStreamSegmentV1 *message2 = (GetStreamSegmentV1 *)e.GetMessage(0);
    EXPECT_EQ(message, message2);

    // verify change in original doesn't touch what's in envelope
    get_stream.set_start_offset(100);
    EXPECT_EQ(0, message->start_offset());
}

TEST_F(EnvelopeTest, MessagesAndCopying)
{
    Envelope e1;
    PingV1 p1;
    p1.add_to_envelope(e1);

    EXPECT_TRUE(NULL != e1.GetMessage(0));

    Envelope e2 = e1;
    EXPECT_EQ(1, e2.MessageCount());
    PingV1 *p2 = (PingV1 *)e2.GetMessage(0);
    ASSERT_TRUE(NULL != p2);
    EXPECT_TRUE(&p1 != p2);
}

TEST_F(EnvelopeTest, MessageCacheResizing)
{
    Envelope e;
    PingV1 p1, p2, p3;
    p1.add_to_envelope(e);

    EXPECT_TRUE(NULL != e.GetMessage(0));
    Message *m0 = e.GetMessage(0);

    p2.add_to_envelope(e);
    p3.add_to_envelope(e);
    EXPECT_TRUE(NULL != e.GetMessage(0));
    EXPECT_TRUE(NULL != e.GetMessage(1));

    EXPECT_EQ(m0, e.GetMessage(0));

    ASSERT_EQ(3, e.messages_size);
    EXPECT_EQ(NULL, e.messages[2]);
}

TEST_F(EnvelopeTest, Clear)
{
    Envelope e1;
    string s1;
    ASSERT_TRUE(e1.Serialize(s1));

    PingV1 p1;
    p1.add_to_envelope(e1);

    e1.Clear();
    EXPECT_EQ(0, e1.MessageCount());
    EXPECT_TRUE(NULL == e1.GetMessage(0));

    string s2;
    ASSERT_TRUE(e1.Serialize(s2));
    EXPECT_EQ(s1, s2);

    // Verify we can add messages again
    PongV1 pong;
    pong.add_to_envelope(e1);
    pong.add_to_envelope(e1);
    EXPECT_EQ(2, e1.MessageCount());
    EXPECT_TRUE(NULL != e1.GetMessage(0));
    EXPECT_TRUE(NULL != e1.GetMessage(1));
    e1.Clear();
}

TEST_F(EnvelopeTest, CopyMessage)
{
    Envelope e1, e2;
    PingV1 p1;
    p1.add_to_envelope(e1);

    EXPECT_FALSE(e1.CopyMessage(1, e2));

    ASSERT_TRUE(e1.CopyMessage(0, e2));
    ASSERT_EQ(1, e2.MessageCount());

    EXPECT_TRUE(NULL != e1.GetMessage(0));
    EXPECT_TRUE(NULL != e2.GetMessage(0));

    EXPECT_NE(e1.GetMessage(0), e2.GetMessage(0));

    e1.Clear();
    EXPECT_TRUE(NULL == e1.GetMessage(0));
    EXPECT_TRUE(NULL != e2.GetMessage(0));
}

TEST_F(EnvelopeTest, RemoveMessage)
{
    Envelope e;
    PingV1 ping;
    PongV1 pong;

    ping.add_to_envelope(e);
    pong.add_to_envelope(e);

    // populate the cache
    EXPECT_TRUE(NULL != e.GetMessage(0));
    EXPECT_TRUE(NULL != e.GetMessage(1));

    EXPECT_FALSE(e.RemoveMessage(2));

    EXPECT_TRUE(e.RemoveMessage(0));
    EXPECT_EQ(1, e.MessageCount());
    EXPECT_EQ(1, e.MessageNamespaceSize());
    EXPECT_EQ(1, e.MessageTypeSize());
    EXPECT_EQ(PongV1::zippylog_enumeration, e.MessageType(0));
    EXPECT_TRUE(NULL != e.GetMessage(0));
    EXPECT_TRUE(NULL == e.GetMessage(1));

    EXPECT_TRUE(e.RemoveMessage(0));
    EXPECT_EQ(0, e.MessageCount());
    EXPECT_EQ(0, e.MessageNamespaceSize());
    EXPECT_EQ(0, e.MessageTypeSize());
    EXPECT_TRUE(NULL == e.GetMessage(0));

    EXPECT_FALSE(e.RemoveMessage(0));

    e.Clear();

    ping.add_to_envelope(e);
    pong.add_to_envelope(e);
    ping.add_to_envelope(e);

    Message * m0 = e.GetMessage(0);
    Message * m1 = e.GetMessage(2);

    EXPECT_TRUE(NULL != e.GetMessage(0));
    EXPECT_TRUE(NULL != e.GetMessage(1));
    EXPECT_TRUE(NULL != e.GetMessage(2));

    EXPECT_TRUE(e.RemoveMessage(1));
    EXPECT_EQ(2, e.MessageCount());
    EXPECT_EQ(2, e.MessageNamespaceSize());
    EXPECT_EQ(2, e.MessageTypeSize());

    EXPECT_EQ(PingV1::zippylog_enumeration, e.MessageType(0));
    EXPECT_EQ(PingV1::zippylog_enumeration, e.MessageType(1));

    EXPECT_EQ(m0, e.GetMessage(0));
    EXPECT_EQ(m1, e.GetMessage(1));
}

TEST_F(EnvelopeTest, RandomGeneration)
{
    vector< pair<uint32, uint32> > enumerations;
    MessageRegistrar::instance()->GetAllEnumerations(enumerations);

    vector < pair<uint32, uint32> >::iterator i = enumerations.begin();
    for (; i != enumerations.end(); i++) {
        for (int32 j = 10; j; j--) {
            ::google::protobuf::Message *m = this->GetRandomMessage(i->first, i->second);
            delete m;
        }
    }

    for (int32 j = 1000; j; j--) {
        this->GetRandomEnvelope();
    }
}

} // namespace
