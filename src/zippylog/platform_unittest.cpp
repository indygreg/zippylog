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

#include <gtest/gtest.h>

#include <string>
#include <vector>

using namespace ::zippylog::platform;
using ::std::string;
using ::std::vector;
using ::zippylog::platform::ConditionalWait;
using ::zippylog::platform::Time;
using ::zippylog::platform::UUID;

class PlatformTest : public ::zippylog::testing::TestBase
{ };

TEST_F(PlatformTest, CreateTimers)
{
    ASSERT_NO_THROW(Timer t(1000000));
    ASSERT_NO_THROW(Timer t());

    Timer t(100000);
    EXPECT_FALSE(t.Signaled());
    EXPECT_TRUE(t.Start());
    EXPECT_FALSE(t.Signaled());
    ::zippylog::platform::sleep(105);
    EXPECT_TRUE(t.Signaled());

    EXPECT_TRUE(t.Reset());
    EXPECT_FALSE(t.Signaled());

    EXPECT_TRUE(t.Start(50000));
    EXPECT_FALSE(t.Signaled());
    ::zippylog::platform::sleep(51);
    EXPECT_TRUE(t.Signaled());

    EXPECT_TRUE(t.Start(100000));
    EXPECT_TRUE(t.Reset());
    EXPECT_FALSE(t.Signaled());
    ::zippylog::platform::sleep(105);
    EXPECT_FALSE(t.Signaled());
}

TEST_F(PlatformTest, TimeConversion)
{
    Time t;
    ASSERT_TRUE(UnixMicroTimeToZippyTime(0, t));
    EXPECT_EQ(1970, t.year);
    EXPECT_EQ(1, t.mon);
    EXPECT_EQ(1, t.mday);
    EXPECT_EQ(0, t.epoch_sec);
    EXPECT_EQ(0, t.epoch_micro);
    EXPECT_EQ(0, t.usec);
    EXPECT_EQ(0, t.hour);
    EXPECT_EQ(0, t.min);
    EXPECT_EQ(0, t.sec);
    EXPECT_EQ(0, t.yday);
    EXPECT_EQ(4, t.wday);

    ASSERT_TRUE(UnixMicroTimeToZippyTime(1234567890000000, t));
    EXPECT_EQ(2009, t.year);
    EXPECT_EQ(2, t.mon);
    EXPECT_EQ(13, t.mday);
    EXPECT_EQ(1234567890, t.epoch_sec);
    EXPECT_EQ(1234567890000000, t.epoch_micro);
    EXPECT_EQ(0, t.usec);
    EXPECT_EQ(23, t.hour);
    EXPECT_EQ(31, t.min);
    EXPECT_EQ(30, t.sec);
    EXPECT_EQ(43, t.yday);
    EXPECT_EQ(5, t.wday);

    ASSERT_TRUE(UnixMicroTimeToZippyTime(1234567890001024, t));
    EXPECT_EQ(2009, t.year);
    EXPECT_EQ(2, t.mon);
    EXPECT_EQ(13, t.mday);
    EXPECT_EQ(1234567890, t.epoch_sec);
    EXPECT_EQ(1234567890001024, t.epoch_micro);
    EXPECT_EQ(1024, t.usec);
    EXPECT_EQ(23, t.hour);
    EXPECT_EQ(31, t.min);
    EXPECT_EQ(30, t.sec);
    EXPECT_EQ(43, t.yday);
    EXPECT_EQ(5, t.wday);
}

TEST_F(PlatformTest, CreateUUID)
{
    UUID u1;
    memset(&u1, 0, sizeof(u1));
    ASSERT_NO_THROW(ASSERT_TRUE(CreateUUID(u1)));
    bool have_data = false;
    for (size_t i = 0; i < sizeof(u1); i++) {
        if (u1.data[i] > 0) {
            have_data = true;
            break;
        }
    }
    ASSERT_TRUE(have_data);

    string s = CreateUUID();
    EXPECT_EQ(16, s.length());

    s = CreateUUID(true);
    EXPECT_EQ(36, s.length());
}

TEST_F(PlatformTest, FormatUUID)
{
    UUID u1;
    memset(&u1, 64, sizeof(u1));
    string f1;
    ASSERT_NO_THROW(ASSERT_TRUE(FormatUUID(u1, f1)));
    ASSERT_EQ(36, f1.length());
    ASSERT_TRUE("40404040-4040-4040-4040-404040404040" == f1);
}

TEST_F(PlatformTest, ConditionalWaitConstructor)
{
    ASSERT_NO_THROW(ConditionalWait w);
}

TEST_F(PlatformTest, ConditionalWaitNoWait)
{
    ConditionalWait w;
    Time start;
    ASSERT_TRUE(TimeNow(start));
    ASSERT_NO_THROW(EXPECT_FALSE(w.Wait(0)));
    Time end;
    ASSERT_TRUE(TimeNow(end));

    // 10ms seems reasonable
    EXPECT_LE(end.epoch_micro - start.epoch_micro, 10000);
}

TEST_F(PlatformTest, ConditionalWaitWaitTimeout)
{
    ConditionalWait w;
    Time start;
    ASSERT_TRUE(TimeNow(start));

    // wait 0.25s
    ASSERT_NO_THROW(EXPECT_FALSE(w.Wait(250000)));

    Time end;
    ASSERT_TRUE(TimeNow(end));
    EXPECT_GE(end.epoch_micro, start.epoch_micro + 200000);
}

typedef struct conditional_wait_start {
    ConditionalWait *w;
    uint32 sleep;
} conditional_wait_start;

void * sleep_n_and_signal(void *d)
{
    conditional_wait_start *s = (conditional_wait_start *)d;

    if (s->sleep) ::zippylog::platform::sleep(s->sleep / 1000);

    s->w->Signal();

    return NULL;
}

TEST_F(PlatformTest, ConditionalWaitSimpleSignal)
{
    ConditionalWait w;

    conditional_wait_start start;
    start.w = &w;
    start.sleep = 250000;

    Time begin;
    ASSERT_TRUE(TimeNow(begin));
    Thread t1(sleep_n_and_signal, &start);

    ASSERT_NO_THROW(EXPECT_FALSE(w.Wait(100000)));
    ASSERT_NO_THROW(EXPECT_TRUE(w.Wait(200000)));

    Time end;
    ASSERT_TRUE(TimeNow(end));
    EXPECT_GE(end.epoch_micro, begin.epoch_micro + 250000);

    t1.Join();
}

void * cond_wait_waiter(void *d)
{
    ConditionalWait *w = (ConditionalWait *)d;

    EXPECT_NO_THROW(EXPECT_TRUE(w->Wait(-1)));

    return NULL;
}


TEST_F(PlatformTest, ConditionalWaitMultipleWaiters)
{
    ConditionalWait w;

    Thread t1(cond_wait_waiter, &w);
    Thread t2(cond_wait_waiter, &w);

    ::zippylog::platform::sleep(100);
    EXPECT_TRUE(w.Signal());

    t1.Join();
    t2.Join();
}

TEST_F(PlatformTest, DirectoryWatcherConstructor)
{
    string path = this->GetTemporaryDirectory();

    EXPECT_NO_THROW(DirectoryWatcher w(path, false)) << "DirectoryWatcher regular ctor doesn't throw";
    EXPECT_NO_THROW(DirectoryWatcher w(path, true)) << "DirectoryWatcher recursive ctor doesn't throw";

}

TEST_F(PlatformTest, DirectoryWatcherWaitForChanges)
{
    string path = this->GetTemporaryDirectory();

    DirectoryWatcher w(path, false);
    vector<DirectoryChange> changes;

    EXPECT_TRUE(w.GetChanges(changes));
    EXPECT_EQ(0, changes.size());

    string path1 = PathJoin(path, "foo");
    ASSERT_TRUE(MakeDirectory(path1));

    EXPECT_TRUE(w.WaitForChanges(10000));

    EXPECT_TRUE(w.GetChanges(changes));
    EXPECT_EQ(1, changes.size()) << "one directory change recorded";

    EXPECT_FALSE(w.WaitForChanges(1000));
}
