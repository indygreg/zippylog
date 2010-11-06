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

namespace zippylog {
namespace lua {

LuaState::LuaState() :
    have_envelope_filter(false),

    // the default max is barely enough to do anything except create the state
    memory_ceiling(32768),
    memory_current(0),
    memory_exceeded(false),
    memory_max_tried(0),
    memory_max_allowed(0),
    L(NULL)
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

bool LuaState::LoadLuaCode(const string &code)
{
    if (luaL_dostring(this->L, code.c_str())) {
        return false;
    }

    // TODO need more robust logic here
    lua_getglobal(this->L, "zippylog_envelope_filter");
    this->have_envelope_filter = lua_isfunction(this->L, -1);
    lua_pop(this->L, 1);

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