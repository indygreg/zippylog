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

#ifndef ZIPPYLOG_DEVICE_STRING_RECEIVER_HPP_
#define ZIPPYLOG_DEVICE_STRING_RECEIVER_HPP_

#include <zippylog/zippylog.hpp>
#include <zippylog/lua.hpp>

#include <gtest/gtest_prod.h>

#include <iostream>
#include <string>
#include <vector>

namespace zippylog {
namespace device {

/// Used to instantiate instances of the StringReceiver
class ZIPPYLOG_EXPORT StringReceiverStartParams {
public:
    StringReceiverStartParams() :
      stream(NULL),
      lua_max_size(1024)
    { }

    /// Stream to read from
    ///
    ///Must be defined
    ::std::istream *stream;

    /// Filename containing Lua code that we should load
    ::std::string lua_file;

    /// Max size (in kilobytes) that Lua interpreter is allowed to grow to
    uint32 lua_max_size;
};

/// Represents a result from a StringReceiver operation
class ZIPPYLOG_EXPORT StringReceiverResult {
public:
    StringReceiverResult() :
      result(NO_RESULT)
    {}

    enum Result {
        /// Default placeholder value
        NO_RESULT = 0,

        /// Could not read from input stream
        ///
        /// There is no output
        NO_INPUT = 1,

        /// Error running Lua code
        LUA_ERROR = 2,

        /// Lua string processor returned false
        ///
        /// This typically means to drop the message on the floor, but it
        /// all depends on how the string receiver is being used.
        LUA_FALSE = 3,

        /// Processed string OK and we have string(s) as output
        OK_STRING = 4,
    };

    Result result;

    /// String that was received
    ::std::string input_string;

    /// Holds results of string processing
    ///
    /// It is possible for string processors to split the string into multiple
    /// output strings. In this case, this vector will have length() greater
    /// than 1.
    ::std::vector< ::std::string > output_strings;

    /// The bucket this string should be routed to
    ///
    /// Lua processors can return a desired bucket for the data. If they do,
    /// this holds the value returned by Lua.
    ::std::string bucket;

    /// The bucket the output should be routed to
    ///
    /// This is like bucket, but for the stream set.
    ::std::string stream_set;
};

/// Device that receives and processes strings and converts them to envelopes
///
/// The string receiver is likely how string data (like a newline-delimited
/// log file) gets converted to zippylog envelopes.
class ZIPPYLOG_EXPORT StringReceiver {
public:
    /// Construct a string receiver from parameters
    ///
    /// The string receiver device reads strings from an istream object.
    /// For each string received, it can optionally perform processing,
    /// courtesy of Lua.
    ///
    /// See the documentation for StringReceiverStartParams for details of what
    /// is configurable.
    StringReceiver(StringReceiverStartParams &params);

    /// Read a line from the configured stream and process it
    ///
    /// Throws an exception if no stream is configured
    void ReceiveLine(StringReceiverResult &result);

    /// Process a line from a string variable
    ///
    /// This is for manually loading lines into the string receiver
    void ProcessLine(const ::std::string &line, StringReceiverResult &result);

    bool HaveLuaLineProcessor() const { return this->have_lua_line_processor; }

protected:
    ::std::istream *inpipe;

    // Lua interpreter for processing
    ::zippylog::lua::LuaState L;
    ::std::string lua_file;
    uint32 lua_max_size;
    bool have_lua_line_processor;

private:
    StringReceiver(const StringReceiver &orig);
    StringReceiver & operator=(const StringReceiver &orig);

    FRIEND_TEST(StringReceiverTest, ConstructorTest);
};

}} // namespaces

#endif