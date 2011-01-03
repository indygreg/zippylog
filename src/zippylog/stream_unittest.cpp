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

#include <zippylog/stream.hpp>
#include <zippylog/envelope.hpp>

#include <gtest/gtest.h>

#include <string>

using ::std::string;
using ::zippylog::Envelope;
using ::zippylog::FileInputStream;

string path = "test/stores/01-singlestream/A/B/2010-11-26-07.zippylog";

TEST(StreamTest, FileInputStreamConstructor)
{
    ASSERT_NO_THROW(FileInputStream fis(path));

    FileInputStream fis(path);
    EXPECT_EQ(1, fis.CurrentEnvelopeOffset());
}

TEST(StreamTest, FileInputStreamReading)
{
    FileInputStream f1(path);
    ASSERT_TRUE(f1.CanSetAbsoluteOffset());

    Envelope e;
    uint32 read = 0;
    EXPECT_TRUE(f1.ReadEnvelope(e, read));
    EXPECT_EQ(43, read);
    EXPECT_EQ(44, f1.CurrentEnvelopeOffset());

    EXPECT_TRUE(f1.SetAbsoluteOffset(1));
    EXPECT_EQ(42, f1.NextEnvelopeSize());
    EXPECT_TRUE(f1.ReadEnvelope(e, read));
    EXPECT_EQ(43, read);
    EXPECT_EQ(44, f1.CurrentEnvelopeOffset());
    EXPECT_EQ(12, f1.NextEnvelopeSize());
    EXPECT_TRUE(f1.ReadEnvelope(e, read));
    EXPECT_EQ(13, read);

    // initial envelope offset
    {
        FileInputStream f(path, 1);
        EXPECT_EQ(42, f.NextEnvelopeSize());
        EXPECT_TRUE(f.ReadEnvelope(e, read));
        EXPECT_EQ(43, read);
    }

    // offset is within an envelope
    {
        FileInputStream f(path, 12);
        EXPECT_FALSE(f.ReadEnvelope(e, read));
    }

    // correct offset
    {
        FileInputStream f(path, 44);
        EXPECT_TRUE(f.ReadEnvelope(e, read));
        EXPECT_EQ(13, read);
        EXPECT_EQ(44 + 12 + 1, f.CurrentEnvelopeOffset());
    }
}
