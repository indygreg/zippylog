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

#ifndef ZIPPYLOG_LUA_HPP_
#define ZIPPYLOG_LUA_HPP_

#include <zippylog/zippylog.hpp>
#include <zippylog/envelope.hpp>

extern "C" {
#include <lua.h>
#include <lauxlib.h>
}

#include <string>
#include <vector>

namespace zippylog {
namespace lua {

/// represents the input and output to a line processing function call
class ZIPPYLOG_EXPORT LineProcessorState {
public:
    // DEFINITIONS
    enum CallbackResult {
        // take/took no action
        NOTHING = 1,

        YES = 2,

        NO = 3,

        STRING_MODIFIED = 4,
    };

    // INPUT parameters
    ::std::string string_in;

    // OUTPUT parameters
    CallbackResult result;
    ::std::string string_out;
    ::std::string bucket;
    ::std::string stream_set;
};

/// Represents the result of a load string function execution
/// via LuaState::ExecuteLoadString()
class ZIPPYLOG_EXPORT LoadStringResult {
public:
    /// Describes how the function returned
    enum ReturnType {
        /// No return from the function. You executed a bad function.
        NONE = 1,

        /// Function returned nil
        NIL = 2,

        /// Function returned true
        ///
        /// Sadly, we can't use the literal "TRUE" here b/c that is
        /// preprocessor definition in many headers
        BOOLTRUE = 3,

        /// Function returned false
        BOOLFALSE = 4,

        /// One or more strings were returned
        STRING = 5,

        /// One or more protocol buffer messages were returned
        MESSAGE = 6,

        /// One or more envelopes were returned
        ENVELOPE = 7,

        /// Invalid return type pattern seen
        INVALID = 8,
    };

    LoadStringResult() :
        execution_success(false),
        has_bucket(false),
        has_set(false)
    { }

    /// Whether the Lua function executed without error
    ///
    /// If this is false lua_error will contain an error message
    bool execution_success;

    /// Whether a bucket was defined in the response
    bool has_bucket;

    /// Whether a stream set was defined in the response
    bool has_set;

    /// The Lua error message on execution failure
    ///
    /// Will only be set if execution_success is false
    ::std::string lua_error;

    /// Return type from the function
    ///
    /// Will only be set if execution_success is true
    ReturnType return_type;

    /// Bucket for the returned object
    ///
    /// Only defined if has_bucket is true
    ::std::string bucket;

    /// Stream set for returned object
    ///
    /// Only defined if has_set is true
    ::std::string set;

    /// List of string(s) returned
    ///
    /// Only contains elements if return_type is STRING
    ::std::vector< ::std::string > strings;

    /// List of protocol buffer messages returned
    ///
    /// Only contains elements if return_type is MESSAGE
    /// @todo implement with proper type
    /// @todo define who owns memory
    ::std::vector<void *> messages;

    /// List of envelopes returned
    ///
    /// Only contains elements if return_type is ENVELOPE
    ::std::vector< ::zippylog::Envelope > envelopes;
};

// class that handles common Lua functionality
class ZIPPYLOG_EXPORT LuaState {
public:
    LuaState();
    ~LuaState();

    // sets the limit for memory consumption of the interpreter
    bool SetMemoryCeiling(uint32 size);

    // whether the state has an enveloper filter function
    bool HasEnvelopeFilter();

    // whether the interpreter can process text lines
    bool HasLineProcessor();

    /// Whether we have a zippylog_load_string() function
    bool HasLoadString();

    // loads user-supplied Lua code into the interpreter
    bool LoadLuaCode(const ::std::string &code);

    // loads Lua code from a file into the interpret
    bool LoadFile(const ::std::string &filename, ::std::string &error);

    // loads the string standard library into the Lua interpreter
    bool LoadStringLibrary();

    // process a line via the interpreter's line processor
    //
    // Returns true if the function executed without triggering an error.
    // Returns false if there is no line processor or if there was an error.
    // TODO we really need something better than a bool
    bool ProcessLine(LineProcessorState &state);

    /// Executes the load string callback
    ///
    /// If Lua is invoked (even if it fails), LoadStringResult contains a
    /// detailed record of what transpired.
    ///
    /// If there is no load string function registered, will return false.
    ///
    /// @return whether we executed any Lua code
    bool ExecuteLoadString(const ::std::string &s, LoadStringResult &result);

    static void * LuaAlloc(void *ud, void *ptr, size_t osize, size_t nsize);
    static int LuaPanic(lua_State *L);

protected:
    bool PushFilterFunction();
    bool DetermineCapabilities();

    lua_State *L;
    bool memory_exceeded;
    uint32 memory_ceiling;
    uint32 memory_current;
    uint32 memory_max_tried;
    uint32 memory_max_allowed;

    bool have_envelope_filter;
    bool have_line_processor;

    bool have_load_string;

private:
    LuaState(const LuaState &);
    LuaState & operator=(const LuaState &);
};

}} // end namespaces

#endif