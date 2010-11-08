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

#include "zippylog/platform.hpp"

#include <gtest/gtest.h>

using namespace ::zippylog::platform;

TEST(TimerTest, CreateTimers)
{
    ASSERT_NO_THROW(Timer t(1000000));

    Timer t(100000);
    ASSERT_NO_THROW(t.Start());
    EXPECT_FALSE(t.Signaled());
    sleep(105);
    EXPECT_TRUE(t.Signaled());

    EXPECT_TRUE(t.Reset());
    EXPECT_FALSE(t.Signaled());

    EXPECT_TRUE(t.Start(50000));
    EXPECT_FALSE(t.Signaled());
    sleep(51);
    EXPECT_TRUE(t.Signaled());

    EXPECT_TRUE(t.Start(100000));
    EXPECT_TRUE(t.Reset());
    EXPECT_FALSE(t.Signaled());
    sleep(105);
    EXPECT_FALSE(t.Signaled());
}

TEST(TimeTest, TimeConversion)
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