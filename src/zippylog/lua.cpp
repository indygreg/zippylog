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

#include <stdlib.h>
#include <string.h>

extern "C" {
#include <lualib.h>
}

using ::std::string;

#define LOAD_STRING_FUNCTION "zippylog_load_string"

namespace zippylog {
namespace lua {

LuaState::LuaState() :
    L(NULL),
    memory_exceeded(false),

    // the default max is barely enough to do anything except create the state
    memory_ceiling(32768),
    memory_current(0),
    memory_max_tried(0),
    memory_max_allowed(0),
    have_envelope_filter(false),
    have_line_processor(false)
{
    this->L = lua_newstate(LuaState::LuaAlloc, (void *)this);
}

LuaState::~LuaState()
{
    if (this->L) lua_close(this->L);
}

bool LuaState::SetMemoryCeiling(uint32 size)
{
    this->memory_ceiling = size;
    return true;
}

bool LuaState::HasEnvelopeFilter()
{
    return this->have_envelope_filter;
}

bool LuaState::HasLineProcessor()
{
    return this->have_line_processor;
}

bool LuaState::HasLoadString()
{
    return this->have_load_string;
}

// TODO verify we can't load binary Lua code (textual only) b/c bytecode
// verification is gone in Lua 5.2 and 5.1 was apparently buggy anyway

bool LuaState::LoadLuaCode(const string &code)
{
    if (luaL_dostring(this->L, code.c_str())) {
        return false;
    }

    return this->DetermineCapabilities();

    return true;
}

bool LuaState::LoadFile(const string &filename, string &error)
{
    if (luaL_dofile(this->L, filename.c_str())) {
        error = lua_tostring(L, -1);
        return false;
    }

    return this->DetermineCapabilities();
}

bool LuaState::LoadStringLibrary()
{
    luaopen_string(this->L);

    return true;
}

bool LuaState::DetermineCapabilities()
{
    // TODO need more robust logic here
    lua_getglobal(this->L, "zippylog_envelope_filter");
    this->have_envelope_filter = lua_isfunction(this->L, -1);
    lua_pop(this->L, 1);

    lua_getglobal(this->L, "zippylog_process_line");
    this->have_line_processor = lua_isfunction(this->L, -1);
    lua_pop(this->L, 1);

    lua_getglobal(this->L, LOAD_STRING_FUNCTION);
    this->have_load_string = lua_isfunction(this->L, -1);
    lua_pop(this->L, 1);

    return true;
}

bool LuaState::ExecuteLoadString(const string &s, LoadStringResult &result)
{
    if (!this->have_load_string) return false;

    int stack_size = lua_gettop(this->L);

    lua_getglobal(this->L, LOAD_STRING_FUNCTION);
    lua_pushlstring(this->L, s.c_str(), s.length());
    if (lua_pcall(this->L, 1, LUA_MULTRET, 0) != 0) {
        result.execution_success = false;
        size_t len = 0;
        const char *error = lua_tolstring(this->L, -1, &len);
        result.lua_error = string(error, len);
        lua_pop(this->L, 1);
        return true;
    }

    result.execution_success = true;

    int nresults = lua_gettop(L) - stack_size;
    if (nresults < 1) {
        result.return_type = result.NONE;
        return true;
    }

    // as a refresher, the following type conventions are handled:
    //
    //  nil
    //  bool
    //  1 or more strings
    //  1 or more messages
    //  1 or more envelopes
    //  table, <any of above except nil>

    // handle the one off case for a single nil
    if (nresults == 1 && lua_isnil(this->L, -1)) {
        result.return_type = result.NIL;
        goto LOAD_STRING_POP_AND_RETURN;
    }

    int stpos = -1 * nresults;
    int value_results = nresults;

    // so we have a type that could be repeated
    // the first and only first one could be a table, so handle that
    if (lua_istable(this->L, stpos)) {
        value_results--;

        if (value_results < 1) {
            result.return_type = result.INVALID;
            goto LOAD_STRING_POP_AND_RETURN;
        }

        // TODO should we error if type is not a string?

        lua_getfield(this->L, stpos, "bucket");
        if (lua_isstring(this->L, -1)) {
            size_t len;
            const char *s = lua_tolstring(this->L, -1, &len);
            result.has_bucket = true;
            result.bucket = string(s, len);
        }
        lua_pop(this->L, 1);

        lua_getfield(this->L, stpos, "set");
        if (lua_isstring(this->L, -1)) {
            size_t len;
            const char *s = lua_tolstring(this->L, -1, &len);
            result.has_set = true;
            result.set = string(s, len);
        }
        lua_pop(this->L, 1);

        stpos++;
    }

    bool got_type = false;
    // ok, now iterate through the remaining values
    // make sure the types are consistent along the way
    for (int i = stpos; i < 0; i++) {
        if (lua_isboolean(this->L, i)) {
            // multiple booleans doesn't make any sense. catch it
            if (value_results > 1) {
                result.return_type = result.INVALID;
                goto LOAD_STRING_POP_AND_RETURN;
            }

            result.return_type = lua_toboolean(this->L, i) ? result.BOOLTRUE : result.BOOLFALSE;
            got_type = true;
            continue;
        }

        if (lua_isstring(this->L, i)) {
            // can't mix types
            if (got_type && result.return_type != result.STRING) {
                result.return_type = result.INVALID;
                goto LOAD_STRING_POP_AND_RETURN;
            }

            size_t len = 0;
            const char *s = lua_tolstring(this->L, i, &len);
            result.return_type = result.STRING;
            result.strings.push_back(string(s, len));
            got_type = true;
            continue;
        }

        // TODO handle envelope and message cases
        result.return_type = result.INVALID;
        break;
    }

LOAD_STRING_POP_AND_RETURN:
    lua_pop(this->L, nresults);

    return true;
}

bool LuaState::ProcessLine(LineProcessorState &st)
{
    if (!this->have_line_processor) return false;

    int stack_size = lua_gettop(this->L);

    lua_getglobal(this->L, "zippylog_process_line");
    lua_pushstring(this->L, st.string_in.c_str());
    if (lua_pcall(this->L, 1, LUA_MULTRET, 0) != 0) {
        // TODO need better error handling
        return false;
    }

    int nresults = lua_gettop(L) - stack_size;
    if (nresults < 1) {
        // TODO better error handling
        return false;
    }

    int stpos = -1 * nresults;

    // base of stack is the first result, which can be a table or a value
    if (lua_istable(this->L, -1 * nresults)) {
        if (nresults < 2) {
            // TODO need better error handling
            lua_pop(this->L, nresults);
            return false;
        }
        // we need to look for the special keys

        // bucket selects where output goes
        lua_pushlstring(this->L, "bucket", strlen("bucket"));
        lua_gettable(this->L, -1 * nresults);

        if (lua_isstring(this->L, -1)) {
            st.bucket = lua_tostring(this->L, -1);
        }
        lua_pop(this->L, 1);

        // stream_set selects where output goes
        lua_pushlstring(this->L, "stream_set", strlen("stream_set"));
        lua_gettable(this->L, -1 * nresults);
        if (lua_isstring(this->L, -1)) {
            st.stream_set = lua_tostring(this->L, -1);
        }
        lua_pop(this->L, 1);

        stpos++;
    }

    // now we have the normal values
    if (lua_isnil(this->L, stpos)) {
        st.result = LineProcessorState::NOTHING;
        st.string_out = st.string_in;
    }
    else if (lua_isboolean(this->L, stpos)) {
        st.result = lua_toboolean(this->L, stpos) ? LineProcessorState::YES : LineProcessorState::NO;

        if (st.result == LineProcessorState::YES) {
            st.string_out = st.string_in;
        }
        else {
            st.string_out.clear();
        }
    }
    else if (lua_isstring(this->L, stpos)) {
        // TODO support multiple output strings

        // this constructor copies data, so it is safe to use
        // (Lua strings allocated out of Lua)
        st.string_out = string(lua_tostring(this->L, stpos));

        st.result = LineProcessorState::STRING_MODIFIED;
    }
    // TODO handle protocol buffer userdata
    else {
        // TODO better error handling
        lua_pop(this->L, nresults);
        return false;
    }

    // clear all results
    lua_pop(this->L, nresults);

    return true;
}

void * LuaState::LuaAlloc(void *ud, void *ptr, size_t osize, size_t nsize)
{
    LuaState *s = (LuaState *)ud;

    // letting go of memory
    if (nsize == 0) {
        s->memory_current -= osize;

        free(ptr);
        return NULL;
    }
    // shrinking existing memory
    else if (nsize < osize) {
        s->memory_current -= osize - nsize;
        return realloc(ptr, nsize);
    }
    // else growing memory
    s->memory_max_tried += nsize - osize;

    // would exceed our limit. disallow
    if (s->memory_current + nsize - osize > s->memory_ceiling) {
        s->memory_exceeded = true;
        return NULL;
    }

    // would not exceed, so we allocate
    s->memory_current += nsize - osize;
    if (s->memory_current > s->memory_max_allowed) s->memory_max_allowed = s->memory_current;
    return realloc(ptr, nsize);
}

int LuaState::LuaPanic(lua_State *L)
{
    return 0;
}

}} // namespaces