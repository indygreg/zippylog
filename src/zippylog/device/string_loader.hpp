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

#ifndef ZIPPYLOG_DEVICE_STRING_LOADER_HPP_
#define ZIPPYLOG_DEVICE_STRING_LOADER_HPP_

#include <zippylog/zippylog.hpp>
#include <zippylog/lua.hpp>
#include <zippylog/store.hpp>

#include <gtest/gtest_prod.h>

#include <iostream>
#include <string>
#include <vector>

namespace zippylog {
namespace device {

/// Used to instantiate instances of the StringLoader
class ZIPPYLOG_EXPORT StringLoaderStartParams {
public:
    StringLoaderStartParams() :
      input_stream(NULL),
      lua_max_size(1024),
      output_stream(NULL),
      active(NULL)
    { }

    /// Input parameters

    /// Stream to read string data from
    ::std::istream * input_stream;

    /// Processing parameters

    /// Filename containing Lua code that we should load
    ::std::string lua_file;

    /// Lua code to load
    ::std::string lua_code;

    /// Max size (in kilobytes) that Lua interpreter is allowed to grow to
    uint32 lua_max_size;

    /// Output parameters

    /// If defined, a stream to write processed string data to
    ///
    /// This is typically only used for debugging purposes
    ::std::ostream * output_stream;

    /// If defined, will write to a store constructed from this store path
    ::std::string store_path;

    /// Default bucket to route envelopes to
    ///
    /// Must be defined if writing to a server or store
    ::std::string default_bucket;

    /// Default stream set to route envelopes to
    ///
    /// Must be defined if writing to a server or store
    ::std::string default_set;

    /// Misc parameters

    /// Semaphore for the device to remain active
    bool * active;
};

/// Represents the result of string processing in the string loader
class ZIPPYLOG_EXPORT StringLoaderProcessingResult {
public:
    /// Construct an empty result
    StringLoaderProcessingResult() :
      success(false),
      has_bucket(false),
      has_set(false)
    { }

    ~StringLoaderProcessingResult() {
        for (size_t i = this->envelopes.size(); i; i--) {
            delete this->envelopes[i-1];
        }
    }

    /// Whether the result was successful
    bool success;

    /// Error describing what went wrong
    ///
    /// Only defined if success is false
    ::std::string error;

    /// Envelopes produced as part of processing
    ///
    /// The envelopes are owned by this object. They will be destroyed when
    /// the instance holding them is destroyed.
    ::std::vector<Envelope *> envelopes;

    /// Whether a bucket was set
    ///
    /// If true, bucket should have a value
    bool has_bucket;

    /// Whether a stream set was set
    ///
    /// If true, set should have a value
    bool has_set;

    /// The bucket to load the envelope into
    ::std::string bucket;

    /// The stream set to load the envelope into
    ::std::string set;
};

/// Device that loads strings into envelopes and optionally sends them
/// somewhere
///
/// The device can be configured with an input stream and an output mode or
/// modes. When configured this way, you simply configure the device, start
/// it, and it is self-sustaining.
///
/// Alternatively, you can create the device without any input or output and
/// call the functions that do processing. In those mode, you are free to
/// generate input or route output to your desire.
class ZIPPYLOG_EXPORT StringLoader {
public:
    /// Construct a string loader from parameters
    ///
    /// @param params Parameters to control behavior
    StringLoader(StringLoaderStartParams &params);

    ~StringLoader();

    /// Read a line from the configured input stream and process it
    ///
    /// Throws an exception if no input stream is configured
    void ReadLineAndProcess(StringLoaderProcessingResult &result);

    /// Process a string for loading
    ///
    /// This is what's internally called by ReceiveLine(). It handles all the
    /// logic for invoking Lua (if configured) and marshaling the string to an
    /// envelope.
    void ProcessString(::std::string const& s, StringLoaderProcessingResult &result);

    /// Sends a result to the configured outputs
    ///
    /// This is called internally when the device is executing on its own.
    /// The API is public just in case.
    void SendResult(StringLoaderProcessingResult const& result);

    /// Whether a Lua string loader is configured
    bool HaveLuaStringLoader() const { return this->have_lua_string_loader; }

    /// Runs the device
    ///
    /// Will read data from the input stream and route to the configured
    /// outputs until the semaphore passed in the constructor says to stop.
    ///
    /// In other words, this doesn't return until the semaphore goes to false.
    void Run();

protected:
    /// Stream we are reading from
    ::std::istream * const instream;

    /// Stream we are writing to
    ::std::ostream * const outstream;

    /// Lua interpreter for processing
    ::zippylog::lua::LuaState L;

    /// Whether the Lua state has string loader processing
    bool have_lua_string_loader;

    /// The default bucket for envelopes
    ::std::string default_bucket;

    /// The default stream set for envelopes
    ::std::string default_set;

    /// The store we are writing to
    ::zippylog::Store * store;

    /// Whether the device should remain active
    bool * const active;

private:
    StringLoader(StringLoader const &orig);
    StringLoader & operator=(StringLoader const &orig);

    FRIEND_TEST(StringLoaderTest, ConstructorTest);
};

}} // namespaces

#endif