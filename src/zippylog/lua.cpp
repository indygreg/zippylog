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

#include <zippylog/lua.hpp>

#include <stdlib.h>
#include <string.h>

extern "C" {
#include <lualib.h>
}

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
    }

    // now we have the normal values
    int stpos = -1 * nresults + 1;
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