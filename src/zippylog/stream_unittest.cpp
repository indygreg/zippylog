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

#include <zippylog/platform.hpp>
#include <zippylog/stream.hpp>
#include <zippylog/envelope.hpp>

#include <string>

using ::std::string;
using ::zippylog::Envelope;
using ::zippylog::FileInputStream;
using ::zippylog::FileOutputStream;

namespace zippylog {

class StreamTest : public ::zippylog::testing::TestBase
{ };

string path = "test/stores/01-singlestream/A/B/2010-11-26-07.zippylog";

TEST_F(StreamTest, FileOutputStreamConstructor)
{
    string dir = this->GetTemporaryDirectory();
    string path = platform::PathJoin(dir, "s1.zippylog");

    ASSERT_NO_THROW(FileOutputStream fos(path, false)) << "create FileOutputStream without lock";
    ASSERT_NO_THROW(FileOutputStream fos(path, true)) << "create FileOutputStream with lock";
}

TEST_F(StreamTest, FileOutputStreamStreamFormat)
{
    string path = platform::PathJoin(this->GetTemporaryDirectory(), "s1.zippylog");

    Envelope e1, e2;
    e1.AddTag("foo");
    e1.AddTag("bar");
    e1.SetStringValueField("string value");
    e2.SetStringValueField("another envelope");

    ASSERT_EQ(33, e1.SerializedByteSize());
    ASSERT_EQ(27, e2.SerializedByteSize());

    // in a closure so the output stream gets destroyed
    {
        FileOutputStream fos(path, true);
        ASSERT_TRUE(fos.WriteEnvelope(e1));
        ASSERT_TRUE(fos.WriteEnvelope(e2));
    }

    platform::FileStat fs;
    ASSERT_TRUE(platform::stat(path, fs)) << "able to stat written stream";

    // stream header + envelope length header + envelope data
    EXPECT_EQ(1 + 2 + e1.SerializedByteSize() + e2.SerializedByteSize(), fs.size)
        << "stream is correct size";

    platform::File f;
    ASSERT_TRUE(f.Open(path, platform::File::READ | platform::File::BINARY));

    int fd = f.FileDescriptor();
    ASSERT_GT(fd, 0) << "file descriptor for reading is valid";

    char *data = new char[fs.size];
    EXPECT_EQ(fs.size, read(fd, data, fs.size));

    EXPECT_EQ(0x01, data[0]);
    EXPECT_EQ(33, data[1]);
    EXPECT_EQ(27, data[1 + 1 + 33]);

    EXPECT_NO_THROW(Envelope e1p((void *)(data + 2), 33));
    EXPECT_NO_THROW(Envelope e2p((void *)(data + 1 + 1 + 33 + 1), 27));

    Envelope e1p((void *)(data + 2), 33);
    Envelope e2p((void *)(data + 2+33+1), 27);

    EXPECT_EQ(e1, e1p);
    EXPECT_EQ(e2, e2p);

    delete [] data;
}

TEST_F(StreamTest, FileInputStreamConstructor)
{
    ASSERT_NO_THROW(FileInputStream fis(path));

    FileInputStream fis(path);
    EXPECT_EQ(1, fis.CurrentEnvelopeOffset());
}

TEST_F(StreamTest, FileInputStreamReading)
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

} // namespace