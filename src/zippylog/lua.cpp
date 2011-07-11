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
#include <zippylog/store.hpp>
#include <lua-protobuf.h>

#include <stdlib.h>
#include <string.h>

extern "C" {
#include <lualib.h>
}

using ::std::string;
using ::zippylog::Envelope;
using ::zippylog::Store;

#define SUBSCRIPTION_FILTER_ENVELOPE "zippylog_subscription_filter_envelope"
#define SUBSCRIPTION_TIMER_INTERVAL "zippylog_subscription_timer_interval"
#define SUBSCRIPTION_TIMER_SIGNALED "zippylog_subscription_timer_signaled"
#define LOAD_STRING_FUNCTION "zippylog_load_string"
#define LUA_ENVELOPE_METHOD_TABLENAME "zippylog._envelope"
#define LUA_ENVELOPE_FUNCTION_TABLENAME "zippylog.envelope"

// we define our Lua callbacks in the context of C to avoid name mangling
// and ugly compiler attribute usage
extern "C" {

/// Lua userdata type for envelope instances
typedef struct envelope_udata {
    Envelope *e;
} envelope_udata;

/// Function called from within Lua to create a new Envelope instance
int zippylog_lua_envelope_new(lua_State *L)
{
    envelope_udata *ud = (envelope_udata *)lua_newuserdata(L, sizeof(envelope_udata));
    ud->e = new Envelope();
    luaL_getmetatable(L, LUA_ENVELOPE_METHOD_TABLENAME);
    lua_setmetatable(L, -2);

    return 1;
}

/// Garbage collection function for envelope instances
int zippylog_lua_envelope_gc(lua_State *L)
{
    envelope_udata *ud = (envelope_udata *)luaL_checkudata(L, 1, LUA_ENVELOPE_METHOD_TABLENAME);
    if (ud->e) {
        delete ud->e;
        ud->e = NULL;
    }

    return 1;
}

int zippylog_lua_envelope_serialize(lua_State *L)
{
    envelope_udata *ud = (envelope_udata *)luaL_checkudata(L, 1, LUA_ENVELOPE_METHOD_TABLENAME);

    string serialized;
    ud->e->Serialize(serialized);

    lua_pushlstring(L, serialized.c_str(), serialized.size());
    return 1;
}

int zippylog_lua_envelope_message_count(lua_State *L)
{
    envelope_udata *ud = (envelope_udata *)luaL_checkudata(L, 1, LUA_ENVELOPE_METHOD_TABLENAME);

    lua_pushinteger(L, ud->e->MessageCount());
    return 1;
}

int zippylog_lua_envelope_message_enumeration(lua_State *L)
{
    envelope_udata *ud = (envelope_udata *)luaL_checkudata(L, 1, LUA_ENVELOPE_METHOD_TABLENAME);

    if (lua_gettop(L) != 2) {
        return luaL_error(L, "message_enumeration() requires one integer argument. none given");
    }

    lua_Integer offset = luaL_checkinteger(L, 2);
    int current_size = ud->e->MessageCount();
    if (offset < 1 || offset > current_size) {
        return luaL_error(L,
            "offset passed to message_enumeration() must be between 1 and %d. %d given",
            current_size + 1, offset);
    }

    uint32 ns = ud->e->MessageNamespace(offset - 1);
    uint32 type = ud->e->MessageType(offset - 1);

    lua_pushinteger(L, ns);
    lua_pushinteger(L, type);

    return 2;
}

int zippylog_lua_envelope_tag_count(lua_State *L)
{
    envelope_udata *ud = (envelope_udata *)luaL_checkudata(L, 1, LUA_ENVELOPE_METHOD_TABLENAME);

    lua_pushinteger(L, ud->e->TagSize());
    return 1;
}

int zippylog_lua_envelope_add_tag(lua_State *L)
{
    envelope_udata *ud = (envelope_udata *)luaL_checkudata(L, 1, LUA_ENVELOPE_METHOD_TABLENAME);

    if (lua_gettop(L) != 2) {
        return luaL_error(L, "add_tag() requires one string argument. none given");
    }

    size_t len;
    const char *s = luaL_checklstring(L, 2, &len);

    ud->e->AddTag(string(s, len));

    return 0;
}

int zippylog_lua_envelope_get_tag(lua_State *L)
{
    envelope_udata *ud = (envelope_udata *)luaL_checkudata(L, 1, LUA_ENVELOPE_METHOD_TABLENAME);

    if (lua_gettop(L) != 2) {
        return luaL_error(L, "get_tag() requires one integer argument. none given");
    }

    lua_Integer offset = luaL_checkinteger(L, 2);
    int current_size = ud->e->TagSize();
    if (offset < 1 || offset > current_size) {
        return luaL_error(L,
            "offset passed to get_tag() must be between 1 and %d. %d given",
            current_size + 1, offset);
    }

    string tag = ud->e->GetTag(offset - 1);
    lua_pushlstring(L, tag.c_str(), tag.size());

    return 1;
}

int zippylog_lua_envelope_serialized_byte_size(lua_State *L)
{
    envelope_udata *ud = (envelope_udata *)luaL_checkudata(L, 1, LUA_ENVELOPE_METHOD_TABLENAME);

    lua_pushinteger(L, ud->e->SerializedByteSize());

    return 1;
}

int zippylog_lua_envelope_get_string_value(lua_State *L)
{
    envelope_udata *ud = (envelope_udata *)luaL_checkudata(L, 1, LUA_ENVELOPE_METHOD_TABLENAME);

    if (!ud->e->HasStringValue()) {
        lua_pushnil(L);
    }
    else {
        string s = ud->e->GetStringValueField();
        lua_pushlstring(L, s.c_str(), s.size());
    }

    return 1;
}

int zippylog_lua_envelope_set_string_value(lua_State *L)
{
    envelope_udata *ud = (envelope_udata *)luaL_checkudata(L, 1, LUA_ENVELOPE_METHOD_TABLENAME);

    if (lua_gettop(L) != 2) {
        return luaL_error(L, "set_string_value() expects one string argument. none given");
    }

    size_t len;
    const char * value = luaL_checklstring(L, 2, &len);

    ud->e->SetStringValueField(string(value, len));

    return 0;
}

int zippylog_lua_envelope_get_message(lua_State *L)
{
    return luaL_error(L, "get_message() is not yet implemented");
}

int zippylog_lua_envelope_add_message(lua_State *L)
{
    return luaL_error(L, "add_message() is not yet implemented");
}

} // end of extern C

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
    have_load_string(false),
    have_subscription_envelope_filter(false),
    have_subscription_timer(false)
{
    this->L = lua_newstate(LuaState::LuaAlloc, (void *)this);

    this->InitializeState();
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

// TODO verify we can't load binary Lua code (textual only) b/c bytecode
// verification is gone in Lua 5.2 and 5.1 was apparently buggy anyway

bool LuaState::LoadLuaCode(const string &code)
{
    if (luaL_loadstring(this->L, code.c_str()) != 0) {
        // error on top of stack
        lua_pop(this->L, 1);
        return false;
    }

    if (lua_pcall(L, 0, LUA_MULTRET, 0) != 0) {
        // error on top of stack
        lua_pop(this->L, 1);
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
    lua_pop(this->L, 1);

    return true;
}

bool LuaState::DetermineCapabilities()
{
    lua_getglobal(this->L, LOAD_STRING_FUNCTION);
    this->have_load_string = lua_isfunction(this->L, -1);
    lua_pop(this->L, 1);

    lua_getglobal(this->L, SUBSCRIPTION_FILTER_ENVELOPE);
    this->have_subscription_envelope_filter = lua_isfunction(this->L, -1);
    lua_pop(this->L, 1);

    lua_getglobal(this->L, SUBSCRIPTION_TIMER_INTERVAL);
    lua_getglobal(this->L, SUBSCRIPTION_TIMER_SIGNALED);
    this->have_subscription_timer = lua_isnumber(this->L, -2) && lua_isfunction(this->L, -1);
    if (!this->have_subscription_timer) {
        if (lua_isnumber(this->L, -2) || lua_isfunction(this->L, -1)) {
            lua_pop(this->L, 2);
            return false;
        }
    }
    lua_pop(this->L, 2);

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
        result.return_type = result.INVALID;
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

    int stpos = -1 * nresults;
    int value_results = nresults;
    bool got_type = false;

    // handle the one off case for a single nil
    if (nresults == 1 && lua_isnil(this->L, -1)) {
        result.return_type = result.NIL;
        goto LOAD_STRING_POP_AND_RETURN;
    }

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

        void *ud = lua_touserdata(this->L, i);
        if (ud == NULL) {
            result.return_type = result.INVALID;
            break;
        }

        if (lua_getmetatable(this->L, i) == 0) {
            result.return_type = result.INVALID;
            break;
        }

        // is it an envelope type
        lua_getfield(this->L, LUA_REGISTRYINDEX, LUA_ENVELOPE_METHOD_TABLENAME);
        if (lua_rawequal(this->L, -2, -1)) {
            // don't need the metatables
            lua_pop(this->L, 2);

            if (got_type && result.return_type != result.ENVELOPE) {
                result.return_type = result.INVALID;
                goto LOAD_STRING_POP_AND_RETURN;
            }

            envelope_udata *ed = (envelope_udata *)ud;

            // we copy the envelope since poorly designed Lua scripts could
            // do evil things with the envelope in Lua land and we don't want
            // that to impact us here
            result.envelopes.push_back(Envelope(*ed->e));

            result.return_type = result.ENVELOPE;
            got_type = true;
            continue;
        }

        // remove metatables
        lua_pop(this->L, 2);

        // TODO handle message case

        result.return_type = result.INVALID;
        break;
    }

LOAD_STRING_POP_AND_RETURN:
    lua_pop(this->L, nresults);

    return true;
}

bool LuaState::ExecuteSubscriptionEnvelopeFilter(const Envelope &e, const string &path, EnvelopeFilterResult &result)
{
    if (!this->have_subscription_envelope_filter) return false;

    string bucket, stream_set, stream;
    if (!Store::ParsePath(path, bucket, stream_set, stream)) {
        // TODO is this right?
        throw new Exception("unable to parse path. this should never happen");
    }

    // f(envelope, bucket, stream_set, stream)
    lua_getglobal(this->L, SUBSCRIPTION_FILTER_ENVELOPE);
    this->PushEnvelope(e);
    lua_pushlstring(this->L, bucket.c_str(), bucket.length());
    lua_pushlstring(this->L, stream_set.c_str(), stream_set.length());
    lua_pushlstring(this->L, stream.c_str(), stream.length());

    if (lua_pcall(this->L, 4, 1, 0) != 0) {
        result.execution_success = false;
        size_t len = 0;
        const char *error = lua_tolstring(this->L, -1, &len);
        result.lua_error = string(error, len);
        lua_pop(this->L, 1);
        return true;
    }

    result.execution_success = true;

    if (!lua_isboolean(this->L, -1)) {
        result.return_type = EnvelopeFilterResult::OTHER;
        lua_pop(this->L, 1);
        return true;
    }

    result.return_type = lua_toboolean(this->L, -1) ? EnvelopeFilterResult::BOOLTRUE : EnvelopeFilterResult::BOOLFALSE;
    lua_pop(this->L, 1);

    return true;
}

void LuaState::InitializeState()
{
    this->RegisterEnvelopeType();
}

bool LuaState::GetGlobal(const string &s, int64 &value)
{
    lua_getglobal(this->L, s.c_str());
    bool result = false;
    if (lua_isnumber(this->L, -1)) {
        result = true;
        value = lua_tointeger(this->L, -1);
    }

    lua_pop(this->L, 1);

    return result;
}

bool LuaState::GetGlobal(const string &s, string &value)
{
    lua_getglobal(this->L, s.c_str());
    bool result = false;
    size_t len;
    if (lua_isstring(L, -1)) {
        result = true;
        const char *r = lua_tolstring(L, -1, &len);
        value.assign(r, len);
    }

    lua_pop(this->L, 1);

    return result;
}

static const struct luaL_Reg EnvelopeFunctions [] = {
    { "new", zippylog_lua_envelope_new },
    { NULL, NULL }
};

static const struct luaL_Reg EnvelopeMethods [] = {
    { "__gc", zippylog_lua_envelope_gc },
    { "serialize", zippylog_lua_envelope_serialize },
    { "message_count", zippylog_lua_envelope_message_count },
    { "message_enumeration", zippylog_lua_envelope_message_enumeration },
    { "tag_count", zippylog_lua_envelope_tag_count },
    { "add_tag", zippylog_lua_envelope_add_tag },
    { "get_tag", zippylog_lua_envelope_get_tag },
    { "serialized_byte_size", zippylog_lua_envelope_serialized_byte_size },
    { "get_string_value", zippylog_lua_envelope_get_string_value },
    { "set_string_value", zippylog_lua_envelope_set_string_value },
    { "get_message", zippylog_lua_envelope_get_message },
    { "add_message", zippylog_lua_envelope_add_message },
    { NULL, NULL }
};

int LuaState::RegisterEnvelopeType()
{
    luaL_newmetatable(this->L, LUA_ENVELOPE_METHOD_TABLENAME);
    lua_pushvalue(this->L, -1);
    lua_setfield(this->L, -2, "__index");
    luaL_register(this->L, NULL, EnvelopeMethods);
    luaL_register(this->L, LUA_ENVELOPE_FUNCTION_TABLENAME, EnvelopeFunctions);
    lua_pop(this->L, 2);
    return 1;
}

bool LuaState::PushEnvelope(const Envelope &e)
{
    envelope_udata *ud = (envelope_udata *)lua_newuserdata(L, sizeof(envelope_udata));
    ud->e = new Envelope(e);
    luaL_getmetatable(L, LUA_ENVELOPE_METHOD_TABLENAME);
    lua_setmetatable(L, -2);

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

int LuaState::LuaPanic(lua_State *)
{
    return 0;
}

}} // namespaces