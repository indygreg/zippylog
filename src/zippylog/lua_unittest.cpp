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

#include <zippylog/lua.hpp>

#include <gtest/gtest.h>

#include <string>

using ::std::string;
using ::zippylog::lua::LuaState;
using ::zippylog::lua::LoadStringResult;

TEST(LuaTest, SimpleExecution)
{
    LuaState l;
    EXPECT_TRUE(l.LoadLuaCode("function test()\nreturn true\nend"));
    EXPECT_FALSE(l.HasEnvelopeFilter());
}

TEST(LuaTest, DetectBadCode)
{
    LuaState l;
    EXPECT_FALSE(l.LoadLuaCode("function foo(32r"));
}

TEST(LuaTest, DetectLoadString)
{
    LuaState l;
    string code = "function zippylog_load_string(s)\n"
                  "  return true\n"
                  "end";

    EXPECT_TRUE(l.LoadLuaCode(code));
    EXPECT_TRUE(l.HasLoadString());
}

TEST(LuaTest, LoadStringLuaError)
{
    LuaState l;
    string code = "function zippylog_load_string(s)\n"
                  "  return foo.bar()\n"
                  "end";

    ASSERT_TRUE(l.LoadLuaCode(code));

    LoadStringResult result;
    EXPECT_TRUE(l.ExecuteLoadString("foo", result));

    EXPECT_FALSE(result.execution_success);
    EXPECT_GT(result.lua_error.size(), 0);
}

TEST(LuaTest, LoadStringSingleNil)
{
    LuaState l;
    string code = "function zippylog_load_string(s)\n"
                  "  return nil\n"
                  "end";
    EXPECT_TRUE(l.LoadLuaCode(code));

    LoadStringResult result;
    ASSERT_TRUE(l.ExecuteLoadString("foo", result));
    EXPECT_TRUE(result.execution_success);

    EXPECT_FALSE(result.has_bucket);
    EXPECT_FALSE(result.has_set);
    EXPECT_EQ(LoadStringResult::NIL, result.return_type);
}

TEST(LuaTest, LoadStringMultipleNil)
{
    LuaState l;
    string code = "function zippylog_load_string(s)\n"
                  "  return nil, nil\n"
                  "end";
    EXPECT_TRUE(l.LoadLuaCode(code));

    LoadStringResult result;
    ASSERT_TRUE(l.ExecuteLoadString("foo", result));
    EXPECT_TRUE(result.execution_success);

    EXPECT_FALSE(result.has_bucket);
    EXPECT_FALSE(result.has_set);
    EXPECT_EQ(LoadStringResult::INVALID, result.return_type);
}

TEST(LuaTest, LoadStringTrue)
{
    LuaState l;
    string code = "function zippylog_load_string(s)\n"
                  "  return true\n"
                  "end";
    EXPECT_TRUE(l.LoadLuaCode(code));

    LoadStringResult result;
    ASSERT_TRUE(l.ExecuteLoadString("foo", result));
    EXPECT_TRUE(result.execution_success);

    EXPECT_FALSE(result.has_bucket);
    EXPECT_FALSE(result.has_set);
    EXPECT_EQ(LoadStringResult::BOOLTRUE, result.return_type);
}

TEST(LuaTest, LoadStringMultipleTrue)
{
    LuaState l;
    string code = "function zippylog_load_string(s)\n"
                  "  return true, true\n"
                  "end";
    EXPECT_TRUE(l.LoadLuaCode(code));

    LoadStringResult result;
    ASSERT_TRUE(l.ExecuteLoadString("foo", result));
    EXPECT_TRUE(result.execution_success);

    EXPECT_FALSE(result.has_bucket);
    EXPECT_FALSE(result.has_set);
    EXPECT_EQ(LoadStringResult::INVALID, result.return_type);
}

TEST(LuaTest, LoadStringFalse)
{
    LuaState l;
    string code = "function zippylog_load_string(s)\n"
                  "  return false\n"
                  "end";
    EXPECT_TRUE(l.LoadLuaCode(code));

    LoadStringResult result;
    ASSERT_TRUE(l.ExecuteLoadString("foo", result));
    EXPECT_TRUE(result.execution_success);

    EXPECT_FALSE(result.has_bucket);
    EXPECT_FALSE(result.has_set);
    EXPECT_EQ(LoadStringResult::BOOLFALSE, result.return_type);
}

TEST(LuaTest, LoadStringMultipleFalse)
{
    LuaState l;
    string code = "function zippylog_load_string(s)\n"
                  "  return false,false\n"
                  "end";
    EXPECT_TRUE(l.LoadLuaCode(code));

    LoadStringResult result;
    ASSERT_TRUE(l.ExecuteLoadString("foo", result));
    EXPECT_TRUE(result.execution_success);

    EXPECT_FALSE(result.has_bucket);
    EXPECT_FALSE(result.has_set);
    EXPECT_EQ(LoadStringResult::INVALID, result.return_type);
}

TEST(LuaTest, LoadStringSingleString)
{
    LuaState l;
    string code = "function zippylog_load_string(s)\n"
                  "  return \"bar\"\n"
                  "end";
    EXPECT_TRUE(l.LoadLuaCode(code));

    LoadStringResult result;
    ASSERT_TRUE(l.ExecuteLoadString("foo", result));
    EXPECT_TRUE(result.execution_success);

    EXPECT_FALSE(result.has_bucket);
    EXPECT_FALSE(result.has_set);
    EXPECT_EQ(LoadStringResult::STRING, result.return_type);
    EXPECT_EQ(1, result.strings.size());
    EXPECT_STREQ("bar", result.strings[0].c_str());
}

TEST(LuaTest, LoadStringMultipleStrings)
{
    LuaState l;
    string code = "function zippylog_load_string(s)\n"
                  "  return \"bar\", \"baz\"\n"
                  "end";
    EXPECT_TRUE(l.LoadLuaCode(code));

    LoadStringResult result;
    ASSERT_TRUE(l.ExecuteLoadString("foo", result));
    EXPECT_TRUE(result.execution_success);

    EXPECT_FALSE(result.has_bucket);
    EXPECT_FALSE(result.has_set);
    EXPECT_EQ(LoadStringResult::STRING, result.return_type);
    EXPECT_EQ(2, result.strings.size());
    EXPECT_STREQ("bar", result.strings[0].c_str());
    EXPECT_STREQ("baz", result.strings[1].c_str());
}

TEST(LuaTest, LoadStringTableBucketThenString)
{
    LuaState l;
    string code = "function zippylog_load_string(s)\n"
                  "  return { [\"bucket\"] = \"b\" }, \"bar\"\n"
                  "end";
    EXPECT_TRUE(l.LoadLuaCode(code));

    LoadStringResult result;
    ASSERT_TRUE(l.ExecuteLoadString("foo", result));
    EXPECT_TRUE(result.execution_success);

    EXPECT_TRUE(result.has_bucket);
    EXPECT_STREQ("b", result.bucket.c_str());
    EXPECT_FALSE(result.has_set);
    EXPECT_EQ(LoadStringResult::STRING, result.return_type);
    EXPECT_EQ(1, result.strings.size());
    EXPECT_STREQ("bar", result.strings[0].c_str());
}

TEST(LuaTest, LoadStringTableBucketAndSetThenString)
{
    LuaState l;
    string code = "function zippylog_load_string(s)\n"
                  "  return { [\"bucket\"] = \"b\", [\"set\"] = \"s\" }, \"bar\"\n"
                  "end";
    EXPECT_TRUE(l.LoadLuaCode(code));

    LoadStringResult result;
    ASSERT_TRUE(l.ExecuteLoadString("foo", result));
    EXPECT_TRUE(result.execution_success);

    EXPECT_TRUE(result.has_bucket);
    EXPECT_STREQ("b", result.bucket.c_str());
    EXPECT_TRUE(result.has_set);
    EXPECT_STREQ("s", result.set.c_str());
    EXPECT_EQ(LoadStringResult::STRING, result.return_type);
    EXPECT_EQ(1, result.strings.size());
    EXPECT_STREQ("bar", result.strings[0].c_str());
}

TEST(LuaTest, LoadStringTableThenStrings)
{
    LuaState l;
    string code = "function zippylog_load_string(s)\n"
                  "  return { [\"bucket\"] = \"buck\", [\"set\"] = \"se\" }, \"foo\", \"bar\"\n"
                  "end";
    EXPECT_TRUE(l.LoadLuaCode(code));

    LoadStringResult result;
    ASSERT_TRUE(l.ExecuteLoadString("foo", result));
    EXPECT_TRUE(result.execution_success);

    EXPECT_TRUE(result.has_bucket);
    EXPECT_STREQ("buck", result.bucket.c_str());
    EXPECT_TRUE(result.has_set);
    EXPECT_STREQ("se", result.set.c_str());
    EXPECT_EQ(LoadStringResult::STRING, result.return_type);
    EXPECT_EQ(2, result.strings.size());
    EXPECT_STREQ("foo", result.strings[0].c_str());
    EXPECT_STREQ("bar", result.strings[1].c_str());
}

// TODO need tests for protocol buffer message and envelope returns
// but, we need code for that first
