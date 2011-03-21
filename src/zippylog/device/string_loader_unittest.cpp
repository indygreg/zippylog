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

#include <zippylog/device/string_loader.hpp>

#include <gtest/gtest.h>

#include <iostream>

namespace zippylog {
namespace device {

using ::std::endl;
using ::std::string;
using ::std::stringstream;
using ::zippylog::device::StringLoader;
using ::zippylog::device::StringLoaderStartParams;
using ::zippylog::device::StringLoaderProcessingResult;

TEST(StringLoaderTest, ConstructorTest)
{
    StringLoaderStartParams p;

    EXPECT_NO_THROW(StringLoader l(p));

    StringLoader l(p);
    EXPECT_EQ(1024 * 1024, l.L.GetMemoryCeiling());
    EXPECT_EQ(NULL, l.instream);
    EXPECT_EQ(NULL, l.outstream);
    EXPECT_FALSE(l.have_lua_string_loader);

    stringstream ss;
    p.input_stream = &ss;

    StringLoader l2(p);
    EXPECT_EQ(&ss, l2.instream);
}

TEST(StringLoaderTest, ProcessStringSimple)
{
    StringLoaderStartParams p;
    StringLoader l(p);

    string input = "test line";
    StringLoaderProcessingResult result;

    EXPECT_NO_THROW(l.ProcessString(input, result));
    EXPECT_TRUE(result.success);
    EXPECT_FALSE(result.has_bucket);
    EXPECT_FALSE(result.has_set);
    EXPECT_EQ(0, result.bucket.size());
    EXPECT_EQ(0, result.set.size());
    EXPECT_EQ(0, result.error.size());
    ASSERT_EQ(1, result.envelopes.size());
    EXPECT_TRUE(result.envelopes[0] != NULL);
    EXPECT_STREQ(input.c_str(), result.envelopes[0]->envelope.string_value().c_str());
}

TEST(StringLoaderTest, DetectNullInputStream)
{
    StringLoaderStartParams p;
    StringLoader l(p);
    StringLoaderProcessingResult result;

    EXPECT_ANY_THROW(l.ReadLineAndProcess(result));
}

TEST(StringLoaderTest, ClosedInputStream)
{
    StringLoaderStartParams p;
    stringstream ss;
    p.input_stream = &ss;
    StringLoader l(p);

    string expected = "test 1 2 3";
    ss << expected << endl;

    StringLoaderProcessingResult result;
    EXPECT_NO_THROW(l.ReadLineAndProcess(result));
    EXPECT_TRUE(result.success);
    EXPECT_FALSE(result.has_bucket);
    EXPECT_FALSE(result.has_set);
    EXPECT_EQ(1, result.envelopes.size());
    EXPECT_TRUE(NULL != result.envelopes[0]);
    EXPECT_STREQ(expected.c_str(), result.envelopes[0]->envelope.string_value().c_str());

    ss.setstate(::std::ios_base::eofbit | ::std::ios_base::failbit);
    StringLoaderProcessingResult result2;
    EXPECT_NO_THROW(l.ReadLineAndProcess(result2));
    EXPECT_FALSE(result2.success);
}

TEST(StringLoaderTest, LuaReturnNil)
{
    StringLoaderStartParams p;
    p.lua_code = "function zippylog_load_string(s)\n"
                 "  return nil\n"
                 "end";

    StringLoader l(p);
    EXPECT_TRUE(l.HaveLuaStringLoader());

    StringLoaderProcessingResult result;
    EXPECT_NO_THROW(l.ProcessString("foo", result));

    EXPECT_TRUE(result.success);
    EXPECT_FALSE(result.has_bucket);
    EXPECT_FALSE(result.has_set);
    EXPECT_EQ(1, result.envelopes.size());
    EXPECT_TRUE(NULL != result.envelopes[0]);
    EXPECT_STREQ("foo", result.envelopes[0]->envelope.string_value().c_str());
}

// same as above
TEST(StringLoaderTest, LuaReturnTrue)
{
    StringLoaderStartParams p;
    p.lua_code = "function zippylog_load_string(s)\n"
                 "  return true\n"
                 "end";

    StringLoader l(p);
    EXPECT_TRUE(l.HaveLuaStringLoader());

    StringLoaderProcessingResult result;
    EXPECT_NO_THROW(l.ProcessString("foo", result));

    EXPECT_TRUE(result.success);
    EXPECT_FALSE(result.has_bucket);
    EXPECT_FALSE(result.has_set);
    EXPECT_EQ(1, result.envelopes.size());
    EXPECT_TRUE(NULL != result.envelopes[0]);
    EXPECT_STREQ("foo", result.envelopes[0]->envelope.string_value().c_str());
}

TEST(StringLoaderTest, LuaReturnFalse)
{
    StringLoaderStartParams p;
    p.lua_code = "function zippylog_load_string(s)\n"
                 "  return false\n"
                 "end";

    StringLoader l(p);
    EXPECT_TRUE(l.HaveLuaStringLoader());

    StringLoaderProcessingResult result;
    EXPECT_NO_THROW(l.ProcessString("foo", result));

    EXPECT_TRUE(result.success);
    EXPECT_FALSE(result.has_bucket);
    EXPECT_FALSE(result.has_set);
    EXPECT_EQ(0, result.envelopes.size());
}

TEST(StringLoaderTest, LuaReturnSingleString)
{
    StringLoaderStartParams p;
    p.lua_code = "function zippylog_load_string(s)\n"
                 "  return \"bar\"\n"
                 "end";

    StringLoader l(p);
    EXPECT_TRUE(l.HaveLuaStringLoader());

    StringLoaderProcessingResult result;
    EXPECT_NO_THROW(l.ProcessString("foo", result));

    EXPECT_TRUE(result.success);
    EXPECT_FALSE(result.has_bucket);
    EXPECT_FALSE(result.has_set);
    EXPECT_EQ(1, result.envelopes.size());
    EXPECT_TRUE(NULL != result.envelopes[0]);
    EXPECT_STREQ("bar", result.envelopes[0]->envelope.string_value().c_str());
}

TEST(StringLoaderTest, LuaReturnMultipleString)
{
    StringLoaderStartParams p;
    p.lua_code = "function zippylog_load_string(s)\n"
                 "  return \"bar\", \"baz\"\n"
                 "end";

    StringLoader l(p);
    EXPECT_TRUE(l.HaveLuaStringLoader());

    StringLoaderProcessingResult result;
    EXPECT_NO_THROW(l.ProcessString("foo", result));

    EXPECT_TRUE(result.success);
    EXPECT_FALSE(result.has_bucket);
    EXPECT_FALSE(result.has_set);
    EXPECT_EQ(2, result.envelopes.size());
    EXPECT_TRUE(NULL != result.envelopes[0]);
    EXPECT_TRUE(NULL != result.envelopes[1]);
    EXPECT_STREQ("bar", result.envelopes[0]->envelope.string_value().c_str());
    EXPECT_STREQ("baz", result.envelopes[1]->envelope.string_value().c_str());
}

TEST(StringLoaderTest, LuaBucketAndTrue)
{
    StringLoaderStartParams p;
    p.lua_code = "function zippylog_load_string(s)\n"
                 "  return { [\"bucket\"] = \"buck\" }, true\n"
                 "end";

    StringLoader l(p);
    EXPECT_TRUE(l.HaveLuaStringLoader());

    StringLoaderProcessingResult result;
    EXPECT_NO_THROW(l.ProcessString("foo", result));

    EXPECT_TRUE(result.success);
    EXPECT_TRUE(result.has_bucket);
    EXPECT_FALSE(result.has_set);
    EXPECT_STREQ("buck", result.bucket.c_str());
    EXPECT_EQ(1, result.envelopes.size());
    EXPECT_TRUE(NULL != result.envelopes[0]);
    EXPECT_STREQ("foo", result.envelopes[0]->envelope.string_value().c_str());
}

TEST(StringLoaderTest, LuaSetAndTrue)
{
    StringLoaderStartParams p;
    p.lua_code = "function zippylog_load_string(s)\n"
                 "  return { [\"set\"] = \"ss\" }, true\n"
                 "end";

    StringLoader l(p);
    EXPECT_TRUE(l.HaveLuaStringLoader());

    StringLoaderProcessingResult result;
    EXPECT_NO_THROW(l.ProcessString("foo", result));

    EXPECT_TRUE(result.success);
    EXPECT_FALSE(result.has_bucket);
    EXPECT_TRUE(result.has_set);
    EXPECT_STREQ("ss", result.set.c_str());
    EXPECT_EQ(1, result.envelopes.size());
    EXPECT_TRUE(NULL != result.envelopes[0]);
    EXPECT_STREQ("foo", result.envelopes[0]->envelope.string_value().c_str());
}

TEST(StringLoaderTest, LuaBucketAndSetAndString)
{
    StringLoaderStartParams p;
    p.lua_code = "function zippylog_load_string(s)\n"
                 "  return { [\"bucket\"] = \"buck\", [ \"set\" ] = \"ss\" }, \"bar\"\n"
                 "end";

    StringLoader l(p);
    EXPECT_TRUE(l.HaveLuaStringLoader());

    StringLoaderProcessingResult result;
    EXPECT_NO_THROW(l.ProcessString("foo", result));

    EXPECT_TRUE(result.success);
    EXPECT_TRUE(result.has_bucket);
    EXPECT_TRUE(result.has_set);
    EXPECT_STREQ("buck", result.bucket.c_str());
    EXPECT_STREQ("ss", result.set.c_str());
    EXPECT_EQ(1, result.envelopes.size());
    EXPECT_TRUE(NULL != result.envelopes[0]);
    EXPECT_STREQ("bar", result.envelopes[0]->envelope.string_value().c_str());
}

}} // namespaces