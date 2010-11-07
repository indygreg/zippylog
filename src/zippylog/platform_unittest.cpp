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
    ASSERT_FALSE(t.Signaled());
    sleep(105);
    ASSERT_TRUE(t.Signaled());

    ASSERT_TRUE(t.Reset());
    ASSERT_FALSE(t.Signaled());

    ASSERT_TRUE(t.Start(50000));
    ASSERT_FALSE(t.Signaled());
    sleep(51);
    ASSERT_TRUE(t.Signaled());

    ASSERT_TRUE(t.Start(100000));
    ASSERT_TRUE(t.Reset());
    ASSERT_FALSE(t.Signaled());
    sleep(105);
    ASSERT_FALSE(t.Signaled());
}


