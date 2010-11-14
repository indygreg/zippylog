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

#include <zippylog/device/string_receiver.hpp>

#include <gtest/gtest.h>

#include <iostream>

namespace zippylog {
namespace device {

using ::std::endl;
using ::std::string;
using ::std::stringstream;
using ::zippylog::device::StringReceiver;
using ::zippylog::device::StringReceiverStartParams;
using ::zippylog::device::StringReceiverResult;

TEST(StringReceiverTest, ConstructorTest) {
    StringReceiverStartParams p1;

    EXPECT_NO_THROW(StringReceiver r(p1));

    StringReceiver r1(p1);

    EXPECT_EQ(1024, r1.lua_max_size);
    EXPECT_EQ(NULL, r1.inpipe);
    EXPECT_FALSE(r1.have_lua_line_processor);

    stringstream ss;
    p1.stream = &ss;

    StringReceiver r2(p1);
    EXPECT_EQ(&ss, r2.inpipe);
}

TEST(StringReceiverTest, SimpleProcessLineTest) {
    StringReceiverStartParams p1;
    StringReceiver r1(p1);

    string line = "test line";
    StringReceiverResult res1;

    EXPECT_NO_THROW(r1.ProcessLine(line, res1));
    EXPECT_EQ(StringReceiverResult::OK_STRING, res1.result);
    EXPECT_EQ(1, res1.output_strings.size());
    EXPECT_STREQ(line.c_str(), res1.output_strings[0].c_str());
    EXPECT_STREQ(line.c_str(), res1.input_string.c_str());
    EXPECT_EQ(0, res1.bucket.length());
    EXPECT_EQ(0, res1.stream_set.length());
}

TEST(StringReceiverTest, SimpleReceiveLineTest) {
    StringReceiverStartParams p1;
    stringstream ss;
    p1.stream = &ss;
    StringReceiver r1(p1);

    string expected = "test 1 2 3";
    ss << expected << endl;

    StringReceiverResult res1;
    EXPECT_NO_THROW(r1.ReceiveLine(res1));
    EXPECT_EQ(StringReceiverResult::OK_STRING, res1.result);
    EXPECT_EQ(1, res1.output_strings.size());
    EXPECT_STREQ(expected.c_str(), res1.output_strings[0].c_str());
    EXPECT_STREQ(expected.c_str(), res1.input_string.c_str());
    EXPECT_EQ(0, res1.bucket.length());
    EXPECT_EQ(0, res1.stream_set.length());
}

TEST(StringReceiverTest, FailWhenReceiveNoStream) {
    StringReceiverStartParams p1;
    StringReceiver r1(p1);

    StringReceiverResult res1;
    EXPECT_ANY_THROW(r1.ReceiveLine(res1));
}

TEST(StringReceiverTest, ClosedStreamTest) {
    // copied from above to establish a baseline
    StringReceiverStartParams p1;
    stringstream ss;
    p1.stream = &ss;
    StringReceiver r1(p1);

    string expected = "test 1 2 3";
    ss << expected << endl;

    StringReceiverResult res1;
    EXPECT_NO_THROW(r1.ReceiveLine(res1));
    EXPECT_EQ(StringReceiverResult::OK_STRING, res1.result);
    EXPECT_EQ(1, res1.output_strings.size());
    EXPECT_STREQ(expected.c_str(), res1.output_strings[0].c_str());
    EXPECT_STREQ(expected.c_str(), res1.input_string.c_str());
    EXPECT_EQ(0, res1.bucket.length());
    EXPECT_EQ(0, res1.stream_set.length());

    // end of copy

    ss.setstate(::std::ios_base::eofbit | ::std::ios_base::failbit);
    StringReceiverResult res2;
    EXPECT_NO_THROW(r1.ReceiveLine(res2));
    EXPECT_EQ(StringReceiverResult::NO_INPUT, res2.result);
}

TEST(StringReceiverTest, LuaProcessing) {
    // Lua function here just returns booleans true
    StringReceiverStartParams p1;
    p1.lua_file = "test/lua/process_line-00.lua";
    ASSERT_NO_THROW(StringReceiver r1(p1));

    StringReceiver r1(p1);
    EXPECT_TRUE(r1.HaveLuaLineProcessor());

    StringReceiverResult res1;
    string input = "foo testing 1 2 3";
    EXPECT_NO_THROW(r1.ProcessLine(input, res1));

    EXPECT_EQ(StringReceiverResult::OK_STRING, res1.result);
    EXPECT_EQ(1, res1.output_strings.size());
    EXPECT_STREQ(input.c_str(), res1.output_strings[0].c_str());
    EXPECT_EQ(0, res1.bucket.length());
    EXPECT_EQ(0, res1.stream_set.length());

    // returns false
    p1.lua_file = "test/lua/process_line-01.lua";
    StringReceiver r2(p1);
    EXPECT_TRUE(r2.HaveLuaLineProcessor());
    StringReceiverResult res2;
    EXPECT_NO_THROW(r2.ProcessLine(input, res2));
    EXPECT_EQ(StringReceiverResult::LUA_FALSE, res2.result);
    EXPECT_EQ(0, res2.output_strings.size());
    EXPECT_STREQ(input.c_str(), res2.input_string.c_str());

    // returns "foo" always
    p1.lua_file = "test/lua/process_line-02.lua";
    StringReceiver r3(p1);
    EXPECT_TRUE(r3.HaveLuaLineProcessor());
    StringReceiverResult res3;
    EXPECT_NO_THROW(r3.ProcessLine(input, res3));
    EXPECT_EQ(StringReceiverResult::OK_STRING, res3.result);
    string expected = "foo";
    EXPECT_STREQ(input.c_str(), res3.input_string.c_str());
    EXPECT_EQ(1, res3.output_strings.size());
    EXPECT_STREQ(expected.c_str(), res3.output_strings[0].c_str());
}

}} // namespaces