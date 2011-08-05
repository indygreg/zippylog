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

#include <zippylog/lua.hpp>

#include <string>

using ::std::string;
using ::zippylog::lua::LuaState;
using ::zippylog::lua::LoadStringResult;
using ::zippylog::lua::EnvelopeFilterResult;

namespace zippylog {
namespace lua {

class LuaTest : public ::zippylog::testing::TestBase
{ };

TEST_F(LuaTest, SimpleExecution)
{
    LuaState l;
    string error;
    EXPECT_EQ(0, l.GetStackSize());
    EXPECT_TRUE(l.LoadLuaCode("function test()\nreturn true\nend", error));
    EXPECT_EQ(0, l.GetStackSize());
    EXPECT_EQ(0, error.length());
    EXPECT_FALSE(l.HasLoadString());
    EXPECT_FALSE(l.HasSubscriptionEnvelopeFilter());
    EXPECT_FALSE(l.HasSubscriptionTimer());
}

TEST_F(LuaTest, LoadStringLibrary)
{
    LuaState l;
    ASSERT_TRUE(l.LoadStringLibrary());
    EXPECT_EQ(0, l.GetStackSize());
}

TEST_F(LuaTest, DetectBadCode)
{
    LuaState l;
    string error;
    EXPECT_FALSE(l.LoadLuaCode("function foo(32r", error));
    EXPECT_EQ(0, l.GetStackSize());
    EXPECT_GT(error.length(), 0);
}

TEST_F(LuaTest, GetGlobalInteger)
{
    LuaState l;
    string error;
    EXPECT_TRUE(l.LoadLuaCode("foo = 12", error));
    int64 value;
    EXPECT_TRUE(l.GetGlobal("foo", value));
    EXPECT_EQ(12, value);

    EXPECT_TRUE(l.LoadLuaCode("bar = true", error));
    EXPECT_FALSE(l.GetGlobal("bar", value));
}

TEST_F(LuaTest, GetGlobalString)
{
    LuaState l;
    string error;
    EXPECT_TRUE(l.LoadLuaCode("foo = \"bar\"", error));
    string value;
    EXPECT_TRUE(l.GetGlobal("foo", value));
    EXPECT_EQ("bar", value);

    EXPECT_TRUE(l.LoadLuaCode("bar = false", error));
    EXPECT_FALSE(l.GetGlobal("bar", value));
}

TEST_F(LuaTest, MemoryExhaustion)
{
    LuaState l;
    string error;
    l.SetMemoryCeiling(16384);
    EXPECT_EQ(16384, l.GetMemoryCeiling());
    EXPECT_TRUE(l.LoadStringLibrary());

    EXPECT_TRUE(l.LoadLuaCode("s1 = \"1234567890\"\n"
                              "s2 = s1\n"
                              "function test()\n"
                              "  while true do\n"
                              "    s2 = s2 .. s1\n"
                              "  end\n"
                              "end", error));

    EXPECT_FALSE(l.LoadLuaCode("test()", error));

}

TEST_F(LuaTest, EnvelopeConstruction)
{
    LuaState l;
    string error;
    EXPECT_TRUE(l.LoadLuaCode("e = zippylog.envelope.new()", error));
    EXPECT_EQ(0, l.GetStackSize());
}

TEST_F(LuaTest, EnvelopeApi)
{
    LuaState l;
    string error;

    EXPECT_TRUE(l.LoadLuaCode("e = zippylog.envelope.new()", error));
    EXPECT_TRUE(l.LoadLuaCode("count = e:message_count()", error));
    int64 ivalue = -100;
    EXPECT_TRUE(l.GetGlobal("count", ivalue));
    EXPECT_EQ(0, l.GetStackSize());
    EXPECT_EQ(0, ivalue);

    EXPECT_TRUE(l.LoadLuaCode("count = e:tag_count()", error));
    EXPECT_TRUE(l.GetGlobal("count", ivalue));
    EXPECT_EQ(0, ivalue);

    EXPECT_FALSE(l.LoadLuaCode("e:get_tag()", error));
    EXPECT_EQ(0, l.GetStackSize());
    EXPECT_FALSE(l.LoadLuaCode("e:get_tag(0)", error));
    EXPECT_EQ(0, l.GetStackSize());
    EXPECT_FALSE(l.LoadLuaCode("e:get_tag(1)", error));
    EXPECT_EQ(0, l.GetStackSize());
    EXPECT_TRUE(l.LoadLuaCode("e:add_tag(\"foo\")", error));
    EXPECT_EQ(0, l.GetStackSize());
    EXPECT_TRUE(l.LoadLuaCode("count = e:tag_count()", error));
    EXPECT_EQ(0, l.GetStackSize());
    EXPECT_TRUE(l.GetGlobal("count", ivalue));
    EXPECT_EQ(1, ivalue);

    string svalue;
    EXPECT_TRUE(l.LoadLuaCode("tag = e:get_tag(1)", error));
    EXPECT_EQ(0, l.GetStackSize());
    EXPECT_TRUE(l.GetGlobal("tag", svalue));
    EXPECT_EQ(0, l.GetStackSize());
    EXPECT_EQ("foo", svalue);

    EXPECT_FALSE(l.LoadLuaCode("tag = e:get_tag(2)", error));
    EXPECT_EQ(0, l.GetStackSize());
    EXPECT_FALSE(l.LoadLuaCode("e:add_tag()", error));
    EXPECT_EQ(0, l.GetStackSize());

    EXPECT_TRUE(l.LoadLuaCode("size = e:serialized_byte_size()", error));
    EXPECT_EQ(0, l.GetStackSize());
    EXPECT_TRUE(l.GetGlobal("size", ivalue));
    EXPECT_EQ(14, ivalue);

    EXPECT_FALSE(l.LoadLuaCode("e:message_enumeration()", error));
    EXPECT_EQ(0, l.GetStackSize());
    EXPECT_FALSE(l.LoadLuaCode("e:message_enumeration(0)", error));
    EXPECT_EQ(0, l.GetStackSize());
    EXPECT_FALSE(l.LoadLuaCode("e:message_enumeration(1)", error));
    EXPECT_EQ(0, l.GetStackSize());

    EXPECT_TRUE(l.LoadLuaCode("s = e:get_string_value()", error));
    EXPECT_EQ(0, l.GetStackSize());
    EXPECT_TRUE(l.LoadLuaCode("e:set_string_value(\"bar\")", error));
    EXPECT_EQ(0, l.GetStackSize());
    EXPECT_TRUE(l.LoadLuaCode("s = e:get_string_value()", error));
    EXPECT_EQ(0, l.GetStackSize());
    EXPECT_TRUE(l.GetGlobal("s", svalue));
    EXPECT_EQ("bar", svalue);
}

TEST_F(LuaTest, DetectLoadString)
{
    LuaState l;
    string error;
    string code = "function zippylog_load_string(s)\n"
                  "  return true\n"
                  "end";

    EXPECT_TRUE(l.LoadLuaCode(code, error));
    EXPECT_EQ(0, l.GetStackSize());
    EXPECT_TRUE(l.HasLoadString());
}

TEST_F(LuaTest, LoadStringLuaError)
{
    LuaState l;
    string error;
    string code = "function zippylog_load_string(s)\n"
                  "  return foo.bar()\n"
                  "end";

    ASSERT_TRUE(l.LoadLuaCode(code, error));

    LoadStringResult result;
    EXPECT_TRUE(l.ExecuteLoadString("foo", result));
    EXPECT_EQ(0, l.GetStackSize());

    EXPECT_FALSE(result.execution_success);
    EXPECT_GT(result.lua_error.size(), 0);
}

TEST_F(LuaTest, LoadStringSingleNil)
{
    LuaState l;
    string error;
    string code = "function zippylog_load_string(s)\n"
                  "  return nil\n"
                  "end";
    EXPECT_TRUE(l.LoadLuaCode(code, error));
    EXPECT_EQ(0, l.GetStackSize());

    LoadStringResult result;
    ASSERT_TRUE(l.ExecuteLoadString("foo", result));
    EXPECT_EQ(0, l.GetStackSize());
    EXPECT_TRUE(result.execution_success);

    EXPECT_FALSE(result.has_bucket);
    EXPECT_FALSE(result.has_set);
    EXPECT_EQ(LoadStringResult::NIL, result.return_type);
}

TEST_F(LuaTest, LoadStringMultipleNil)
{
    LuaState l;
    string error;
    string code = "function zippylog_load_string(s)\n"
                  "  return nil, nil\n"
                  "end";
    EXPECT_TRUE(l.LoadLuaCode(code, error));

    LoadStringResult result;
    ASSERT_TRUE(l.ExecuteLoadString("foo", result));
    EXPECT_EQ(0, l.GetStackSize());
    EXPECT_TRUE(result.execution_success);

    EXPECT_FALSE(result.has_bucket);
    EXPECT_FALSE(result.has_set);
    EXPECT_EQ(LoadStringResult::INVALID, result.return_type);
}

TEST_F(LuaTest, LoadStringTrue)
{
    LuaState l;
    string error;
    string code = "function zippylog_load_string(s)\n"
                  "  return true\n"
                  "end";
    EXPECT_TRUE(l.LoadLuaCode(code, error));

    LoadStringResult result;
    ASSERT_TRUE(l.ExecuteLoadString("foo", result));
    EXPECT_EQ(0, l.GetStackSize());
    EXPECT_TRUE(result.execution_success);

    EXPECT_FALSE(result.has_bucket);
    EXPECT_FALSE(result.has_set);
    EXPECT_EQ(LoadStringResult::BOOLTRUE, result.return_type);
}

TEST_F(LuaTest, LoadStringMultipleTrue)
{
    LuaState l;
    string error;
    string code = "function zippylog_load_string(s)\n"
                  "  return true, true\n"
                  "end";
    EXPECT_TRUE(l.LoadLuaCode(code, error));

    LoadStringResult result;
    ASSERT_TRUE(l.ExecuteLoadString("foo", result));
    EXPECT_EQ(0, l.GetStackSize());
    EXPECT_TRUE(result.execution_success);

    EXPECT_FALSE(result.has_bucket);
    EXPECT_FALSE(result.has_set);
    EXPECT_EQ(LoadStringResult::INVALID, result.return_type);
}

TEST_F(LuaTest, LoadStringFalse)
{
    LuaState l;
    string error;
    string code = "function zippylog_load_string(s)\n"
                  "  return false\n"
                  "end";
    EXPECT_TRUE(l.LoadLuaCode(code, error));

    LoadStringResult result;
    ASSERT_TRUE(l.ExecuteLoadString("foo", result));
    EXPECT_EQ(0, l.GetStackSize());
    EXPECT_TRUE(result.execution_success);

    EXPECT_FALSE(result.has_bucket);
    EXPECT_FALSE(result.has_set);
    EXPECT_EQ(LoadStringResult::BOOLFALSE, result.return_type);
}

TEST_F(LuaTest, LoadStringMultipleFalse)
{
    LuaState l;
    string error;
    string code = "function zippylog_load_string(s)\n"
                  "  return false,false\n"
                  "end";
    EXPECT_TRUE(l.LoadLuaCode(code, error));

    LoadStringResult result;
    ASSERT_TRUE(l.ExecuteLoadString("foo", result));
    EXPECT_EQ(0, l.GetStackSize());
    EXPECT_TRUE(result.execution_success);

    EXPECT_FALSE(result.has_bucket);
    EXPECT_FALSE(result.has_set);
    EXPECT_EQ(LoadStringResult::INVALID, result.return_type);
}

TEST_F(LuaTest, LoadStringSingleString)
{
    LuaState l;
    string error;
    string code = "function zippylog_load_string(s)\n"
                  "  return \"bar\"\n"
                  "end";
    EXPECT_TRUE(l.LoadLuaCode(code, error));

    LoadStringResult result;
    ASSERT_TRUE(l.ExecuteLoadString("foo", result));
    EXPECT_EQ(0, l.GetStackSize());
    EXPECT_EQ(0, l.GetStackSize());
    EXPECT_TRUE(result.execution_success);

    EXPECT_FALSE(result.has_bucket);
    EXPECT_FALSE(result.has_set);
    EXPECT_EQ(LoadStringResult::STRING, result.return_type);
    EXPECT_EQ(1, result.strings.size());
    EXPECT_STREQ("bar", result.strings[0].c_str());
}

TEST_F(LuaTest, LoadStringMultipleStrings)
{
    LuaState l;
    string error;
    string code = "function zippylog_load_string(s)\n"
                  "  return \"bar\", \"baz\"\n"
                  "end";
    EXPECT_TRUE(l.LoadLuaCode(code, error));

    LoadStringResult result;
    ASSERT_TRUE(l.ExecuteLoadString("foo", result));
    EXPECT_EQ(0, l.GetStackSize());
    EXPECT_TRUE(result.execution_success);

    EXPECT_FALSE(result.has_bucket);
    EXPECT_FALSE(result.has_set);
    EXPECT_EQ(LoadStringResult::STRING, result.return_type);
    EXPECT_EQ(2, result.strings.size());
    EXPECT_STREQ("bar", result.strings[0].c_str());
    EXPECT_STREQ("baz", result.strings[1].c_str());
}

TEST_F(LuaTest, LoadStringTableBucketThenString)
{
    LuaState l;
    string error;
    string code = "function zippylog_load_string(s)\n"
                  "  return { [\"bucket\"] = \"b\" }, \"bar\"\n"
                  "end";
    EXPECT_TRUE(l.LoadLuaCode(code, error));

    LoadStringResult result;
    ASSERT_TRUE(l.ExecuteLoadString("foo", result));
    EXPECT_EQ(0, l.GetStackSize());
    EXPECT_TRUE(result.execution_success);

    EXPECT_TRUE(result.has_bucket);
    EXPECT_STREQ("b", result.bucket.c_str());
    EXPECT_FALSE(result.has_set);
    EXPECT_EQ(LoadStringResult::STRING, result.return_type);
    EXPECT_EQ(1, result.strings.size());
    EXPECT_STREQ("bar", result.strings[0].c_str());
}

TEST_F(LuaTest, LoadStringTableBucketAndSetThenString)
{
    LuaState l;
    string error;
    string code = "function zippylog_load_string(s)\n"
                  "  return { [\"bucket\"] = \"b\", [\"set\"] = \"s\" }, \"bar\"\n"
                  "end";
    EXPECT_TRUE(l.LoadLuaCode(code, error));

    LoadStringResult result;
    ASSERT_TRUE(l.ExecuteLoadString("foo", result));
    EXPECT_EQ(0, l.GetStackSize());
    EXPECT_TRUE(result.execution_success);

    EXPECT_TRUE(result.has_bucket);
    EXPECT_STREQ("b", result.bucket.c_str());
    EXPECT_TRUE(result.has_set);
    EXPECT_STREQ("s", result.set.c_str());
    EXPECT_EQ(LoadStringResult::STRING, result.return_type);
    EXPECT_EQ(1, result.strings.size());
    EXPECT_STREQ("bar", result.strings[0].c_str());
}

TEST_F(LuaTest, LoadStringTableThenStrings)
{
    LuaState l;
    string error;
    string code = "function zippylog_load_string(s)\n"
                  "  return { [\"bucket\"] = \"buck\", [\"set\"] = \"se\" }, \"foo\", \"bar\"\n"
                  "end";
    EXPECT_TRUE(l.LoadLuaCode(code, error));

    LoadStringResult result;
    ASSERT_TRUE(l.ExecuteLoadString("foo", result));
    EXPECT_EQ(0, l.GetStackSize());
    ASSERT_TRUE(result.execution_success);

    EXPECT_TRUE(result.has_bucket);
    EXPECT_STREQ("buck", result.bucket.c_str());
    EXPECT_TRUE(result.has_set);
    EXPECT_STREQ("se", result.set.c_str());
    EXPECT_EQ(LoadStringResult::STRING, result.return_type);
    EXPECT_EQ(2, result.strings.size());
    EXPECT_STREQ("foo", result.strings[0].c_str());
    EXPECT_STREQ("bar", result.strings[1].c_str());
}

TEST_F(LuaTest, LoadStringSingleEnvelope)
{
    LuaState l;
    string error;
    ASSERT_TRUE(l.LoadStringLibrary());
    string code = "function zippylog_load_string(s)\n"
                  "  e = zippylog.envelope.new()\n"
                  "  e:set_string_value(string.reverse(s))\n"
                  "  return e\n"
                  "end";

    EXPECT_TRUE(l.LoadLuaCode(code, error));

    LoadStringResult result;
    ASSERT_TRUE(l.ExecuteLoadString("foo", result));
    EXPECT_EQ(0, l.GetStackSize());
    EXPECT_TRUE(result.execution_success);

    EXPECT_FALSE(result.has_bucket);
    EXPECT_FALSE(result.has_set);

    EXPECT_EQ(LoadStringResult::ENVELOPE, result.return_type);
    ASSERT_EQ(1, result.envelopes.size());
    EXPECT_EQ("oof", result.envelopes[0].GetStringValueField());
}

TEST_F(LuaTest, LoadStringMultipleEnvelopes)
{
    LuaState l;
    string error;
    string code = "function zippylog_load_string(s)\n"
                  "  e1 = zippylog.envelope.new()\n"
                  "  e2 = zippylog.envelope.new()\n"
                  "  e3 = zippylog.envelope.new()\n"
                  "  e1:set_string_value(\"foo\")\n"
                  "  e2:set_string_value(\"bar\")\n"
                  "  e3:set_string_value(\"baz\")\n"
                  "  return e1, e2, e3\n"
                  "end";
    EXPECT_TRUE(l.LoadLuaCode(code, error));

    LoadStringResult result;
    ASSERT_TRUE(l.ExecuteLoadString("foo", result));
    EXPECT_EQ(0, l.GetStackSize());
    ASSERT_TRUE(result.execution_success);

    EXPECT_EQ(LoadStringResult::ENVELOPE, result.return_type);
    ASSERT_EQ(3, result.envelopes.size());
    EXPECT_EQ(0, result.strings.size());
    EXPECT_EQ("foo", result.envelopes[0].GetStringValueField());
    EXPECT_EQ("bar", result.envelopes[1].GetStringValueField());
    EXPECT_EQ("baz", result.envelopes[2].GetStringValueField());
}

TEST_F(LuaTest, LoadStringTableThenEnvelopes)
{
    LuaState l;
    string error;
    string code = "function zippylog_load_string(s)\n"
                  "  e1 = zippylog.envelope.new()\n"
                  "  e2 = zippylog.envelope.new()\n"
                  "  t = {}\n"
                  "  t[\"bucket\"] = \"buck\"\n"
                  "  t[\"set\"] = \"stream_set\"\n"
                  "  return t, e1, e2\n"
                  "end";

    ASSERT_TRUE(l.LoadLuaCode(code, error));

    LoadStringResult result;
    ASSERT_TRUE(l.ExecuteLoadString("foo", result));
    EXPECT_EQ(0, l.GetStackSize());
    ASSERT_TRUE(result.execution_success);

    EXPECT_EQ(LoadStringResult::ENVELOPE, result.return_type);
    EXPECT_TRUE(result.has_bucket);
    EXPECT_TRUE(result.has_set);

    EXPECT_EQ("buck", result.bucket);
    EXPECT_EQ("stream_set", result.set);
    ASSERT_EQ(2, result.envelopes.size());
}

// TODO need tests for protocol buffer message returns
// but, we need code for that first

TEST_F(LuaTest, DetectSubscriptionEnvelopeFilter)
{
    LuaState l;
    string error;
    string code = "function zippylog_subscription_filter_envelope(e, bucket, stream_set, stream)\n"
                  "  return true\n"
                  "end";

    EXPECT_TRUE(l.LoadLuaCode(code, error));
    EXPECT_TRUE(l.HasSubscriptionEnvelopeFilter());
    EXPECT_EQ(0, l.GetStackSize());
}

TEST_F(LuaTest, SubscriptionEnvelopeFilterLuaError)
{
    LuaState l;
    string error;
    string code = "function zippylog_subscription_filter_envelope(e, bucket, stream_set, stream)\n"
                  "  return foo.bar()\n"
                  "end";

    EXPECT_TRUE(l.LoadLuaCode(code, error));
    EXPECT_TRUE(l.HasSubscriptionEnvelopeFilter());

    Envelope e;
    EnvelopeFilterResult result;

    EXPECT_TRUE(l.ExecuteSubscriptionEnvelopeFilter(e, "/buck/set/stream", result));
    EXPECT_EQ(0, l.GetStackSize());
    EXPECT_FALSE(result.execution_success);
    EXPECT_GT(result.lua_error.size(), 0);
}

TEST_F(LuaTest, SubscriptionEnvelopeFilterFilterTrue)
{
    LuaState l;
    string error;
    string code = "function zippylog_subscription_filter_envelope(e, bucket, stream_set, stream)\n"
                  "  return true\n"
                  "end";

    EXPECT_TRUE(l.LoadLuaCode(code, error));
    EXPECT_TRUE(l.HasSubscriptionEnvelopeFilter());

    Envelope e;
    EnvelopeFilterResult result;

    EXPECT_TRUE(l.ExecuteSubscriptionEnvelopeFilter(e, "/buck/set/stream", result));
    EXPECT_EQ(0, l.GetStackSize());
    EXPECT_TRUE(result.execution_success);
    EXPECT_EQ(EnvelopeFilterResult::BOOLTRUE, result.return_type);
}

TEST_F(LuaTest, SubscriptionEnvelopeFilterFilterFalse)
{
    LuaState l;
    string error;
    string code = "function zippylog_subscription_filter_envelope(e, bucket, stream_set, stream)\n"
                  "  return false\n"
                  "end";

    EXPECT_TRUE(l.LoadLuaCode(code, error));
    EXPECT_TRUE(l.HasSubscriptionEnvelopeFilter());

    Envelope e;
    EnvelopeFilterResult result;

    EXPECT_TRUE(l.ExecuteSubscriptionEnvelopeFilter(e, "/buck/set/stream", result));
    EXPECT_EQ(0, l.GetStackSize());
    EXPECT_TRUE(result.execution_success);
    EXPECT_EQ(EnvelopeFilterResult::BOOLFALSE, result.return_type);
}

TEST_F(LuaTest, SubscriptionEnvelopeFilterFilterOther)
{
    LuaState l;
    string error;
    string code = "function zippylog_subscription_filter_envelope(e, bucket, stream_set, stream)\n"
                  "  return 5\n"
                  "end";

    EXPECT_TRUE(l.LoadLuaCode(code, error));
    EXPECT_TRUE(l.HasSubscriptionEnvelopeFilter());

    Envelope e;
    EnvelopeFilterResult result;

    EXPECT_TRUE(l.ExecuteSubscriptionEnvelopeFilter(e, "/buck/set/stream", result));
    EXPECT_EQ(0, l.GetStackSize());
    EXPECT_TRUE(result.execution_success);
    EXPECT_EQ(EnvelopeFilterResult::OTHER, result.return_type);
}

}} // namespaces
