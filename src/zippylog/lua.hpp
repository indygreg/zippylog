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

extern "C" {
#include <lua.h>
#include <lauxlib.h>
}

#include <string>

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

private:
    LuaState(const LuaState &);
    LuaState & operator=(const LuaState &);
};

}} // end namespaces

#endif