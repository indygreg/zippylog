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

#include <gtest/gtest_prod.h>

#include <string>
#include <vector>

namespace zippylog {
namespace lua {

/// Represents the result of a load string function execution
/// via LuaState::ExecuteLoadString()
class ZIPPYLOG_EXPORT LoadStringResult {
public:
    /// Describes how the function returned
    enum ReturnType {
        /// Function returned nil
        NIL = 1,

        /// Function returned true
        ///
        /// Sadly, we can't use the literal "TRUE" here b/c that is
        /// preprocessor definition in many headers
        BOOLTRUE = 2,

        /// Function returned false
        BOOLFALSE = 3,

        /// One or more strings were returned
        STRING = 4,

        /// One or more protocol buffer messages were returned
        MESSAGE = 5,

        /// One or more envelopes were returned
        ENVELOPE = 6,

        /// Invalid return type pattern seen
        INVALID = 7,
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

/// Represents the result of an envelope filter invocation
class ZIPPYLOG_EXPORT EnvelopeFilterResult {
public:
    enum ReturnType {
        BOOLTRUE = 1,
        BOOLFALSE = 2,
        OTHER = 3,
    };

    EnvelopeFilterResult() :
        execution_success(false)
    { }

    /// Whether the filter function executed successfully, without error
    ///
    /// If this is false, lua_error will be populated
    bool execution_success;

    /// Describes the return type of the function
    ///
    /// Only defined if execution_success is true
    ReturnType return_type;

    /// Lua error message on Lua execution failure
    ///
    /// Only defined if execution_success is false
    ::std::string lua_error;
};

/// Represents an individual Lua interpreter
///
/// Provides zippylog-specific APIs for invoking functionality on interpreter
class ZIPPYLOG_EXPORT LuaState {
public:
    /// Initialize a new Lua state
    ///
    /// The state has the zippylog APIs defined in it. Also, a custom memory
    /// allocator is used to limit allocation size.
    LuaState();
    ~LuaState();

    /// Sets the limit for memory consumption of the interpreter, in bytes
    ///
    /// @param size Maximum state allocation usage, in bytes
    bool SetMemoryCeiling(uint32 size);

    /// Obtains the set memory ceiling for the interpreter
    inline uint32 GetMemoryCeiling() const { return this->memory_ceiling; }

    /// Whether we have a zippylog_load_string() function
    inline bool HasLoadString() const { return this->have_load_string; }

    /// Whether the state has a subscription envelope filter function
    inline bool HasSubscriptionEnvelopeFilter() const { return this->have_subscription_envelope_filter; }

    /// Whether the state has a subscription timer
    inline bool HasSubscriptionTimer() const { return this->have_subscription_timer; }

    /// Returns the current Lua stack size
    inline int GetStackSize() const { return lua_gettop(this->L); }

    /// Loads user-supplied Lua code into the interpreter
    ///
    /// @return Whether code loaded without error
    bool LoadLuaCode(::std::string const &code, ::std::string &error);

    /// Loads Lua code from a file into the interpret
    ///
    /// @return Whether code loaded without error
    bool LoadFile(::std::string const &filename, ::std::string &error);

    /// Loads the string standard library into the Lua interpreter
    ///
    /// @return Whether the library loaded without error
    bool LoadStringLibrary();

    /// Executes the load string callback
    ///
    /// If Lua is invoked (even if it fails), LoadStringResult contains a
    /// detailed record of what transpired.
    ///
    /// If there is no load string function registered, will return false.
    ///
    /// @return whether we executed any Lua code
    bool ExecuteLoadString(::std::string const &s, LoadStringResult &result);

    /// Executes the subscription envelope filter callback
    ///
    /// A side-effect is the EnvelopeFilterResult passed is populated with
    /// details of the execution.
    ///
    /// @return whether we executed any Lua code
    bool ExecuteSubscriptionEnvelopeFilter(Envelope const &e, ::std::string const &path, EnvelopeFilterResult &result);

    /// Custom memory allocatio function for Lua states
    static void * LuaAlloc(void *ud, void *ptr, size_t osize, size_t nsize);
    static int LuaPanic(lua_State *L);

protected:
    bool DetermineCapabilities();

    /// Initializes a state instance
    ///
    /// Registers zippylog types, etc
    void InitializeState();

    /// Registers the zippylog.envelope type with the interpreter
    int RegisterEnvelopeType();

    /// Obtain the integer value of a global variable
    ///
    /// This is a convenience method. It likely has no use outside of testing
    bool GetGlobal(::std::string const &s, int64 &value);

    /// Obtain the string value of a global variable
    ///
    /// This is a convenience method and likely has no use outside of testing
    bool GetGlobal(::std::string const &s, ::std::string &value);

    /// Pushes an envelope onto the Lua stack
    bool PushEnvelope(Envelope const &e);

    /// Member variables
    lua_State *L;
    bool memory_exceeded;
    uint32 memory_ceiling;
    uint32 memory_current;
    uint32 memory_max_tried;
    uint32 memory_max_allowed;

    bool have_load_string;
    bool have_subscription_envelope_filter;
    bool have_subscription_timer;

private:
    LuaState(LuaState const &);
    LuaState & operator=(LuaState const &);

    FRIEND_TEST(LuaTest, EnvelopeApi);
    FRIEND_TEST(LuaTest, GetGlobalInteger);
    FRIEND_TEST(LuaTest, GetGlobalString);
};

}} // end namespaces

#endif
