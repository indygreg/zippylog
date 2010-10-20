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

#ifndef ZIPPYLOG_LUA_HPP_
#define ZIPPYLOG_LUA_HPP_

#include <zippylog/zippylog.h>

extern "C" {
#include <lua.h>
#include <lauxlib.h>
}

#include <string>

namespace zippylog {
namespace lua {

using ::std::string;

// class that handles common Lua functionality
class LuaState {
public:
    LuaState();
    ~LuaState();

    // sets the limit for memory consumption of the interpreter
    bool SetMemoryCeiling(uint32 size);

    // whether the state has an enveloper filter function
    bool HasEnvelopeFilter();

    // loads user-supplied Lua code into the interpreter
    bool LoadLuaCode(const string &code);

    static void * LuaAlloc(void *ud, void *ptr, size_t osize, size_t nsize);
    static int LuaPanic(lua_State *L);

protected:
    bool PushFilterFunction();

    lua_State *L;
    bool memory_exceeded;
    uint32 memory_ceiling;
    uint32 memory_current;
    uint32 memory_max_tried;
    uint32 memory_max_allowed;

    bool have_envelope_filter;

private:
    LuaState(const LuaState &);
    LuaState & operator=(const LuaState &);
};

}} // end namespaces

#endif