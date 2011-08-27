// Generated by the lua-protobuf compiler
// You shouldn't edit this file manually
//
// source proto file: zippylog/device/store_watcher.proto

#include "zippylog/device/store_watcher.pb-lua.h"

#ifdef __cplusplus
extern "C" { // make sure functions treated with C naming
#endif

#include <lauxlib.h>

#ifdef __cplusplus
}
#endif

#include <string>

// this represents Lua udata for a protocol buffer message
// we record where a message came from so we can GC it properly
typedef struct msg_udata { // confuse over-simplified pretty-printer
    ::google::protobuf::Message * msg;
    bool lua_owns;
    lua_protobuf_gc_callback gc_callback;
    void * callback_data;
} msg_udata;

using ::std::string;

int lua_protobuf_zippylog_device_store_watcher_open(lua_State *L)
{
    const char *table = luaL_findtable(L, LUA_GLOBALSINDEX, "protobuf.zippylog.device.store_watcher", 1);
    if (table) {
        return luaL_error(L, "could not create parent Lua tables");
    }
    if (!lua_istable(L, -1)) {
        lua_newtable(L);
        lua_setfield(L, -2, "zippylog.device.store_watcher");
    }
    lua_pop(L, 1);
    luaL_Reg funcs [] = { { NULL, NULL } };
    luaL_register(L, "protobuf.zippylog.device.store_watcher", funcs);
    lua_protobuf_zippylog_device_store_watcher_Create_open(L);
    lua_protobuf_zippylog_device_store_watcher_Destroy_open(L);
    lua_protobuf_zippylog_device_store_watcher_RunStart_open(L);
    lua_protobuf_zippylog_device_store_watcher_RunFinish_open(L);
    return 1;
}


static const struct luaL_Reg Create_functions [] = {
    {"new", lua_protobuf_zippylog_device_store_watcher_Create_new},
    {"parsefromstring", lua_protobuf_zippylog_device_store_watcher_Create_parsefromstring},
    {NULL, NULL}
};

static const struct luaL_Reg Create_methods [] = {
    {"serialized", lua_protobuf_zippylog_device_store_watcher_Create_serialized},
    {"clear", lua_protobuf_zippylog_device_store_watcher_Create_clear},
    {"__gc", lua_protobuf_zippylog_device_store_watcher_Create_gc},
    {"clear_id", lua_protobuf_zippylog_device_store_watcher_Create_clear_id},
    {"get_id", lua_protobuf_zippylog_device_store_watcher_Create_get_id},
    {"set_id", lua_protobuf_zippylog_device_store_watcher_Create_set_id},
    {"has_id", lua_protobuf_zippylog_device_store_watcher_Create_has_id},
    {NULL, NULL},
};

int lua_protobuf_zippylog_device_store_watcher_Create_open(lua_State *L)
{
    luaL_newmetatable(L, "protobuf_.zippylog.device.store_watcher.Create");
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
    luaL_register(L, NULL, Create_methods);
    luaL_register(L, "protobuf.zippylog.device.store_watcher.Create", Create_functions);
    lua_pop(L, 1);
    return 1;
}


bool lua_protobuf_zippylog_device_store_watcher_Create_pushcopy(lua_State *L, const ::zippylog::device::store_watcher::Create &from)
{
    msg_udata * ud = (msg_udata *)lua_newuserdata(L, sizeof(msg_udata));
    ud->lua_owns = true;
    ud->msg = new ::zippylog::device::store_watcher::Create(from);
    ud->gc_callback = NULL;
    ud->callback_data = NULL;
    luaL_getmetatable(L, "protobuf_.zippylog.device.store_watcher.Create");
    lua_setmetatable(L, -2);
    return true;
}
bool lua_protobuf_zippylog_device_store_watcher_Create_pushreference(lua_State *L, ::zippylog::device::store_watcher::Create *msg, lua_protobuf_gc_callback f, void *data)
{
    msg_udata * ud = (msg_udata *)lua_newuserdata(L, sizeof(msg_udata));
    ud->lua_owns = false;
    ud->msg = msg;
    ud->gc_callback = f;
    ud->callback_data = data;
    luaL_getmetatable(L, "protobuf_.zippylog.device.store_watcher.Create");
    lua_setmetatable(L, -2);
    return true;
}
int lua_protobuf_zippylog_device_store_watcher_Create_new(lua_State *L)
{
    msg_udata * ud = (msg_udata *)lua_newuserdata(L, sizeof(msg_udata));
    ud->lua_owns = true;
    ud->msg = new ::zippylog::device::store_watcher::Create();
    ud->gc_callback = NULL;
    ud->callback_data = NULL;
    luaL_getmetatable(L, "protobuf_.zippylog.device.store_watcher.Create");
    lua_setmetatable(L, -2);
    return 1;
}

int lua_protobuf_zippylog_device_store_watcher_Create_parsefromstring(lua_State *L)
{
    if (lua_gettop(L) != 1) {
        return luaL_error(L, "parsefromstring() requires a string argument. none given");
    }
    size_t len;
    const char *s = luaL_checklstring(L, -1, &len);
    ::zippylog::device::store_watcher::Create * msg = new ::zippylog::device::store_watcher::Create();
    if (!msg->ParseFromArray((const void *)s, len)) {
        return luaL_error(L, "error deserializing message");
    }
    msg_udata * ud = (msg_udata *)lua_newuserdata(L, sizeof(msg_udata));
    ud->lua_owns = true;
    ud->msg = msg;
    ud->gc_callback = NULL;
    ud->callback_data = NULL;
    luaL_getmetatable(L, "protobuf_.zippylog.device.store_watcher.Create");
    lua_setmetatable(L, -2);
    return 1;
}
int lua_protobuf_zippylog_device_store_watcher_Create_gc(lua_State *L)
{
    msg_udata * mud = (msg_udata *)luaL_checkudata(L, 1, "protobuf_.zippylog.device.store_watcher.Create");
    ::zippylog::device::store_watcher::Create *m = (::zippylog::device::store_watcher::Create *)mud->msg;
    if (mud->lua_owns) {
        delete mud->msg;
        mud->msg = NULL;
        return 0;
    }
    if (mud->gc_callback && mud->gc_callback(m, mud->callback_data)) {
        delete mud->msg;
        mud->msg = NULL;
        return 0;
    }
    return 0;
}
int lua_protobuf_zippylog_device_store_watcher_Create_clear(lua_State *L)
{
    msg_udata * mud = (msg_udata *)luaL_checkudata(L, 1, "protobuf_.zippylog.device.store_watcher.Create");
    ::zippylog::device::store_watcher::Create *m = (::zippylog::device::store_watcher::Create *)mud->msg;
    m->Clear();
    return 0;
}
int lua_protobuf_zippylog_device_store_watcher_Create_serialized(lua_State *L)
{
    msg_udata * mud = (msg_udata *)luaL_checkudata(L, 1, "protobuf_.zippylog.device.store_watcher.Create");
    ::zippylog::device::store_watcher::Create *m = (::zippylog::device::store_watcher::Create *)mud->msg;
    string s;
    if (!m->SerializeToString(&s)) {
        return luaL_error(L, "error serializing message");
    }
    lua_pushlstring(L, s.c_str(), s.length());
    return 1;
}
int lua_protobuf_zippylog_device_store_watcher_Create_clear_id(lua_State *L)
{
    msg_udata * mud = (msg_udata *)luaL_checkudata(L, 1, "protobuf_.zippylog.device.store_watcher.Create");
    ::zippylog::device::store_watcher::Create *m = (::zippylog::device::store_watcher::Create *)mud->msg;
    m->clear_id();
    return 0;
}

int lua_protobuf_zippylog_device_store_watcher_Create_get_id(lua_State *L)
{
    msg_udata * mud = (msg_udata *)luaL_checkudata(L, 1, "protobuf_.zippylog.device.store_watcher.Create");
    ::zippylog::device::store_watcher::Create *m = (::zippylog::device::store_watcher::Create *)mud->msg;
    string s = m->id();
    m->has_id() ? lua_pushlstring(L, s.c_str(), s.size()) : lua_pushnil(L);
    return 1;
}

int lua_protobuf_zippylog_device_store_watcher_Create_set_id(lua_State *L)
{
    msg_udata * mud = (msg_udata *)luaL_checkudata(L, 1, "protobuf_.zippylog.device.store_watcher.Create");
    ::zippylog::device::store_watcher::Create *m = (::zippylog::device::store_watcher::Create *)mud->msg;
    if (lua_isnil(L, 2)) {
        m->clear_id();
        return 0;
    }
    
    if (!lua_isstring(L, 2)) return luaL_error(L, "passed value is not a string");
    size_t len;
    const char *s = lua_tolstring(L, 2, &len);
    if (!s) {
        luaL_error(L, "could not obtain string on stack. weird");
    }
    m->set_id(s, len);
    return 0;
}

int lua_protobuf_zippylog_device_store_watcher_Create_has_id(lua_State *L)
{
    msg_udata * mud = (msg_udata *)luaL_checkudata(L, 1, "protobuf_.zippylog.device.store_watcher.Create");
    ::zippylog::device::store_watcher::Create *m = (::zippylog::device::store_watcher::Create *)mud->msg;
    lua_pushboolean(L, m->has_id());
    return 1;
}

static const struct luaL_Reg Destroy_functions [] = {
    {"new", lua_protobuf_zippylog_device_store_watcher_Destroy_new},
    {"parsefromstring", lua_protobuf_zippylog_device_store_watcher_Destroy_parsefromstring},
    {NULL, NULL}
};

static const struct luaL_Reg Destroy_methods [] = {
    {"serialized", lua_protobuf_zippylog_device_store_watcher_Destroy_serialized},
    {"clear", lua_protobuf_zippylog_device_store_watcher_Destroy_clear},
    {"__gc", lua_protobuf_zippylog_device_store_watcher_Destroy_gc},
    {"clear_id", lua_protobuf_zippylog_device_store_watcher_Destroy_clear_id},
    {"get_id", lua_protobuf_zippylog_device_store_watcher_Destroy_get_id},
    {"set_id", lua_protobuf_zippylog_device_store_watcher_Destroy_set_id},
    {"has_id", lua_protobuf_zippylog_device_store_watcher_Destroy_has_id},
    {NULL, NULL},
};

int lua_protobuf_zippylog_device_store_watcher_Destroy_open(lua_State *L)
{
    luaL_newmetatable(L, "protobuf_.zippylog.device.store_watcher.Destroy");
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
    luaL_register(L, NULL, Destroy_methods);
    luaL_register(L, "protobuf.zippylog.device.store_watcher.Destroy", Destroy_functions);
    lua_pop(L, 1);
    return 1;
}


bool lua_protobuf_zippylog_device_store_watcher_Destroy_pushcopy(lua_State *L, const ::zippylog::device::store_watcher::Destroy &from)
{
    msg_udata * ud = (msg_udata *)lua_newuserdata(L, sizeof(msg_udata));
    ud->lua_owns = true;
    ud->msg = new ::zippylog::device::store_watcher::Destroy(from);
    ud->gc_callback = NULL;
    ud->callback_data = NULL;
    luaL_getmetatable(L, "protobuf_.zippylog.device.store_watcher.Destroy");
    lua_setmetatable(L, -2);
    return true;
}
bool lua_protobuf_zippylog_device_store_watcher_Destroy_pushreference(lua_State *L, ::zippylog::device::store_watcher::Destroy *msg, lua_protobuf_gc_callback f, void *data)
{
    msg_udata * ud = (msg_udata *)lua_newuserdata(L, sizeof(msg_udata));
    ud->lua_owns = false;
    ud->msg = msg;
    ud->gc_callback = f;
    ud->callback_data = data;
    luaL_getmetatable(L, "protobuf_.zippylog.device.store_watcher.Destroy");
    lua_setmetatable(L, -2);
    return true;
}
int lua_protobuf_zippylog_device_store_watcher_Destroy_new(lua_State *L)
{
    msg_udata * ud = (msg_udata *)lua_newuserdata(L, sizeof(msg_udata));
    ud->lua_owns = true;
    ud->msg = new ::zippylog::device::store_watcher::Destroy();
    ud->gc_callback = NULL;
    ud->callback_data = NULL;
    luaL_getmetatable(L, "protobuf_.zippylog.device.store_watcher.Destroy");
    lua_setmetatable(L, -2);
    return 1;
}

int lua_protobuf_zippylog_device_store_watcher_Destroy_parsefromstring(lua_State *L)
{
    if (lua_gettop(L) != 1) {
        return luaL_error(L, "parsefromstring() requires a string argument. none given");
    }
    size_t len;
    const char *s = luaL_checklstring(L, -1, &len);
    ::zippylog::device::store_watcher::Destroy * msg = new ::zippylog::device::store_watcher::Destroy();
    if (!msg->ParseFromArray((const void *)s, len)) {
        return luaL_error(L, "error deserializing message");
    }
    msg_udata * ud = (msg_udata *)lua_newuserdata(L, sizeof(msg_udata));
    ud->lua_owns = true;
    ud->msg = msg;
    ud->gc_callback = NULL;
    ud->callback_data = NULL;
    luaL_getmetatable(L, "protobuf_.zippylog.device.store_watcher.Destroy");
    lua_setmetatable(L, -2);
    return 1;
}
int lua_protobuf_zippylog_device_store_watcher_Destroy_gc(lua_State *L)
{
    msg_udata * mud = (msg_udata *)luaL_checkudata(L, 1, "protobuf_.zippylog.device.store_watcher.Destroy");
    ::zippylog::device::store_watcher::Destroy *m = (::zippylog::device::store_watcher::Destroy *)mud->msg;
    if (mud->lua_owns) {
        delete mud->msg;
        mud->msg = NULL;
        return 0;
    }
    if (mud->gc_callback && mud->gc_callback(m, mud->callback_data)) {
        delete mud->msg;
        mud->msg = NULL;
        return 0;
    }
    return 0;
}
int lua_protobuf_zippylog_device_store_watcher_Destroy_clear(lua_State *L)
{
    msg_udata * mud = (msg_udata *)luaL_checkudata(L, 1, "protobuf_.zippylog.device.store_watcher.Destroy");
    ::zippylog::device::store_watcher::Destroy *m = (::zippylog::device::store_watcher::Destroy *)mud->msg;
    m->Clear();
    return 0;
}
int lua_protobuf_zippylog_device_store_watcher_Destroy_serialized(lua_State *L)
{
    msg_udata * mud = (msg_udata *)luaL_checkudata(L, 1, "protobuf_.zippylog.device.store_watcher.Destroy");
    ::zippylog::device::store_watcher::Destroy *m = (::zippylog::device::store_watcher::Destroy *)mud->msg;
    string s;
    if (!m->SerializeToString(&s)) {
        return luaL_error(L, "error serializing message");
    }
    lua_pushlstring(L, s.c_str(), s.length());
    return 1;
}
int lua_protobuf_zippylog_device_store_watcher_Destroy_clear_id(lua_State *L)
{
    msg_udata * mud = (msg_udata *)luaL_checkudata(L, 1, "protobuf_.zippylog.device.store_watcher.Destroy");
    ::zippylog::device::store_watcher::Destroy *m = (::zippylog::device::store_watcher::Destroy *)mud->msg;
    m->clear_id();
    return 0;
}

int lua_protobuf_zippylog_device_store_watcher_Destroy_get_id(lua_State *L)
{
    msg_udata * mud = (msg_udata *)luaL_checkudata(L, 1, "protobuf_.zippylog.device.store_watcher.Destroy");
    ::zippylog::device::store_watcher::Destroy *m = (::zippylog::device::store_watcher::Destroy *)mud->msg;
    string s = m->id();
    m->has_id() ? lua_pushlstring(L, s.c_str(), s.size()) : lua_pushnil(L);
    return 1;
}

int lua_protobuf_zippylog_device_store_watcher_Destroy_set_id(lua_State *L)
{
    msg_udata * mud = (msg_udata *)luaL_checkudata(L, 1, "protobuf_.zippylog.device.store_watcher.Destroy");
    ::zippylog::device::store_watcher::Destroy *m = (::zippylog::device::store_watcher::Destroy *)mud->msg;
    if (lua_isnil(L, 2)) {
        m->clear_id();
        return 0;
    }
    
    if (!lua_isstring(L, 2)) return luaL_error(L, "passed value is not a string");
    size_t len;
    const char *s = lua_tolstring(L, 2, &len);
    if (!s) {
        luaL_error(L, "could not obtain string on stack. weird");
    }
    m->set_id(s, len);
    return 0;
}

int lua_protobuf_zippylog_device_store_watcher_Destroy_has_id(lua_State *L)
{
    msg_udata * mud = (msg_udata *)luaL_checkudata(L, 1, "protobuf_.zippylog.device.store_watcher.Destroy");
    ::zippylog::device::store_watcher::Destroy *m = (::zippylog::device::store_watcher::Destroy *)mud->msg;
    lua_pushboolean(L, m->has_id());
    return 1;
}

static const struct luaL_Reg RunStart_functions [] = {
    {"new", lua_protobuf_zippylog_device_store_watcher_RunStart_new},
    {"parsefromstring", lua_protobuf_zippylog_device_store_watcher_RunStart_parsefromstring},
    {NULL, NULL}
};

static const struct luaL_Reg RunStart_methods [] = {
    {"serialized", lua_protobuf_zippylog_device_store_watcher_RunStart_serialized},
    {"clear", lua_protobuf_zippylog_device_store_watcher_RunStart_clear},
    {"__gc", lua_protobuf_zippylog_device_store_watcher_RunStart_gc},
    {"clear_id", lua_protobuf_zippylog_device_store_watcher_RunStart_clear_id},
    {"get_id", lua_protobuf_zippylog_device_store_watcher_RunStart_get_id},
    {"set_id", lua_protobuf_zippylog_device_store_watcher_RunStart_set_id},
    {"has_id", lua_protobuf_zippylog_device_store_watcher_RunStart_has_id},
    {NULL, NULL},
};

int lua_protobuf_zippylog_device_store_watcher_RunStart_open(lua_State *L)
{
    luaL_newmetatable(L, "protobuf_.zippylog.device.store_watcher.RunStart");
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
    luaL_register(L, NULL, RunStart_methods);
    luaL_register(L, "protobuf.zippylog.device.store_watcher.RunStart", RunStart_functions);
    lua_pop(L, 1);
    return 1;
}


bool lua_protobuf_zippylog_device_store_watcher_RunStart_pushcopy(lua_State *L, const ::zippylog::device::store_watcher::RunStart &from)
{
    msg_udata * ud = (msg_udata *)lua_newuserdata(L, sizeof(msg_udata));
    ud->lua_owns = true;
    ud->msg = new ::zippylog::device::store_watcher::RunStart(from);
    ud->gc_callback = NULL;
    ud->callback_data = NULL;
    luaL_getmetatable(L, "protobuf_.zippylog.device.store_watcher.RunStart");
    lua_setmetatable(L, -2);
    return true;
}
bool lua_protobuf_zippylog_device_store_watcher_RunStart_pushreference(lua_State *L, ::zippylog::device::store_watcher::RunStart *msg, lua_protobuf_gc_callback f, void *data)
{
    msg_udata * ud = (msg_udata *)lua_newuserdata(L, sizeof(msg_udata));
    ud->lua_owns = false;
    ud->msg = msg;
    ud->gc_callback = f;
    ud->callback_data = data;
    luaL_getmetatable(L, "protobuf_.zippylog.device.store_watcher.RunStart");
    lua_setmetatable(L, -2);
    return true;
}
int lua_protobuf_zippylog_device_store_watcher_RunStart_new(lua_State *L)
{
    msg_udata * ud = (msg_udata *)lua_newuserdata(L, sizeof(msg_udata));
    ud->lua_owns = true;
    ud->msg = new ::zippylog::device::store_watcher::RunStart();
    ud->gc_callback = NULL;
    ud->callback_data = NULL;
    luaL_getmetatable(L, "protobuf_.zippylog.device.store_watcher.RunStart");
    lua_setmetatable(L, -2);
    return 1;
}

int lua_protobuf_zippylog_device_store_watcher_RunStart_parsefromstring(lua_State *L)
{
    if (lua_gettop(L) != 1) {
        return luaL_error(L, "parsefromstring() requires a string argument. none given");
    }
    size_t len;
    const char *s = luaL_checklstring(L, -1, &len);
    ::zippylog::device::store_watcher::RunStart * msg = new ::zippylog::device::store_watcher::RunStart();
    if (!msg->ParseFromArray((const void *)s, len)) {
        return luaL_error(L, "error deserializing message");
    }
    msg_udata * ud = (msg_udata *)lua_newuserdata(L, sizeof(msg_udata));
    ud->lua_owns = true;
    ud->msg = msg;
    ud->gc_callback = NULL;
    ud->callback_data = NULL;
    luaL_getmetatable(L, "protobuf_.zippylog.device.store_watcher.RunStart");
    lua_setmetatable(L, -2);
    return 1;
}
int lua_protobuf_zippylog_device_store_watcher_RunStart_gc(lua_State *L)
{
    msg_udata * mud = (msg_udata *)luaL_checkudata(L, 1, "protobuf_.zippylog.device.store_watcher.RunStart");
    ::zippylog::device::store_watcher::RunStart *m = (::zippylog::device::store_watcher::RunStart *)mud->msg;
    if (mud->lua_owns) {
        delete mud->msg;
        mud->msg = NULL;
        return 0;
    }
    if (mud->gc_callback && mud->gc_callback(m, mud->callback_data)) {
        delete mud->msg;
        mud->msg = NULL;
        return 0;
    }
    return 0;
}
int lua_protobuf_zippylog_device_store_watcher_RunStart_clear(lua_State *L)
{
    msg_udata * mud = (msg_udata *)luaL_checkudata(L, 1, "protobuf_.zippylog.device.store_watcher.RunStart");
    ::zippylog::device::store_watcher::RunStart *m = (::zippylog::device::store_watcher::RunStart *)mud->msg;
    m->Clear();
    return 0;
}
int lua_protobuf_zippylog_device_store_watcher_RunStart_serialized(lua_State *L)
{
    msg_udata * mud = (msg_udata *)luaL_checkudata(L, 1, "protobuf_.zippylog.device.store_watcher.RunStart");
    ::zippylog::device::store_watcher::RunStart *m = (::zippylog::device::store_watcher::RunStart *)mud->msg;
    string s;
    if (!m->SerializeToString(&s)) {
        return luaL_error(L, "error serializing message");
    }
    lua_pushlstring(L, s.c_str(), s.length());
    return 1;
}
int lua_protobuf_zippylog_device_store_watcher_RunStart_clear_id(lua_State *L)
{
    msg_udata * mud = (msg_udata *)luaL_checkudata(L, 1, "protobuf_.zippylog.device.store_watcher.RunStart");
    ::zippylog::device::store_watcher::RunStart *m = (::zippylog::device::store_watcher::RunStart *)mud->msg;
    m->clear_id();
    return 0;
}

int lua_protobuf_zippylog_device_store_watcher_RunStart_get_id(lua_State *L)
{
    msg_udata * mud = (msg_udata *)luaL_checkudata(L, 1, "protobuf_.zippylog.device.store_watcher.RunStart");
    ::zippylog::device::store_watcher::RunStart *m = (::zippylog::device::store_watcher::RunStart *)mud->msg;
    string s = m->id();
    m->has_id() ? lua_pushlstring(L, s.c_str(), s.size()) : lua_pushnil(L);
    return 1;
}

int lua_protobuf_zippylog_device_store_watcher_RunStart_set_id(lua_State *L)
{
    msg_udata * mud = (msg_udata *)luaL_checkudata(L, 1, "protobuf_.zippylog.device.store_watcher.RunStart");
    ::zippylog::device::store_watcher::RunStart *m = (::zippylog::device::store_watcher::RunStart *)mud->msg;
    if (lua_isnil(L, 2)) {
        m->clear_id();
        return 0;
    }
    
    if (!lua_isstring(L, 2)) return luaL_error(L, "passed value is not a string");
    size_t len;
    const char *s = lua_tolstring(L, 2, &len);
    if (!s) {
        luaL_error(L, "could not obtain string on stack. weird");
    }
    m->set_id(s, len);
    return 0;
}

int lua_protobuf_zippylog_device_store_watcher_RunStart_has_id(lua_State *L)
{
    msg_udata * mud = (msg_udata *)luaL_checkudata(L, 1, "protobuf_.zippylog.device.store_watcher.RunStart");
    ::zippylog::device::store_watcher::RunStart *m = (::zippylog::device::store_watcher::RunStart *)mud->msg;
    lua_pushboolean(L, m->has_id());
    return 1;
}

static const struct luaL_Reg RunFinish_functions [] = {
    {"new", lua_protobuf_zippylog_device_store_watcher_RunFinish_new},
    {"parsefromstring", lua_protobuf_zippylog_device_store_watcher_RunFinish_parsefromstring},
    {NULL, NULL}
};

static const struct luaL_Reg RunFinish_methods [] = {
    {"serialized", lua_protobuf_zippylog_device_store_watcher_RunFinish_serialized},
    {"clear", lua_protobuf_zippylog_device_store_watcher_RunFinish_clear},
    {"__gc", lua_protobuf_zippylog_device_store_watcher_RunFinish_gc},
    {"clear_id", lua_protobuf_zippylog_device_store_watcher_RunFinish_clear_id},
    {"get_id", lua_protobuf_zippylog_device_store_watcher_RunFinish_get_id},
    {"set_id", lua_protobuf_zippylog_device_store_watcher_RunFinish_set_id},
    {"has_id", lua_protobuf_zippylog_device_store_watcher_RunFinish_has_id},
    {NULL, NULL},
};

int lua_protobuf_zippylog_device_store_watcher_RunFinish_open(lua_State *L)
{
    luaL_newmetatable(L, "protobuf_.zippylog.device.store_watcher.RunFinish");
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
    luaL_register(L, NULL, RunFinish_methods);
    luaL_register(L, "protobuf.zippylog.device.store_watcher.RunFinish", RunFinish_functions);
    lua_pop(L, 1);
    return 1;
}


bool lua_protobuf_zippylog_device_store_watcher_RunFinish_pushcopy(lua_State *L, const ::zippylog::device::store_watcher::RunFinish &from)
{
    msg_udata * ud = (msg_udata *)lua_newuserdata(L, sizeof(msg_udata));
    ud->lua_owns = true;
    ud->msg = new ::zippylog::device::store_watcher::RunFinish(from);
    ud->gc_callback = NULL;
    ud->callback_data = NULL;
    luaL_getmetatable(L, "protobuf_.zippylog.device.store_watcher.RunFinish");
    lua_setmetatable(L, -2);
    return true;
}
bool lua_protobuf_zippylog_device_store_watcher_RunFinish_pushreference(lua_State *L, ::zippylog::device::store_watcher::RunFinish *msg, lua_protobuf_gc_callback f, void *data)
{
    msg_udata * ud = (msg_udata *)lua_newuserdata(L, sizeof(msg_udata));
    ud->lua_owns = false;
    ud->msg = msg;
    ud->gc_callback = f;
    ud->callback_data = data;
    luaL_getmetatable(L, "protobuf_.zippylog.device.store_watcher.RunFinish");
    lua_setmetatable(L, -2);
    return true;
}
int lua_protobuf_zippylog_device_store_watcher_RunFinish_new(lua_State *L)
{
    msg_udata * ud = (msg_udata *)lua_newuserdata(L, sizeof(msg_udata));
    ud->lua_owns = true;
    ud->msg = new ::zippylog::device::store_watcher::RunFinish();
    ud->gc_callback = NULL;
    ud->callback_data = NULL;
    luaL_getmetatable(L, "protobuf_.zippylog.device.store_watcher.RunFinish");
    lua_setmetatable(L, -2);
    return 1;
}

int lua_protobuf_zippylog_device_store_watcher_RunFinish_parsefromstring(lua_State *L)
{
    if (lua_gettop(L) != 1) {
        return luaL_error(L, "parsefromstring() requires a string argument. none given");
    }
    size_t len;
    const char *s = luaL_checklstring(L, -1, &len);
    ::zippylog::device::store_watcher::RunFinish * msg = new ::zippylog::device::store_watcher::RunFinish();
    if (!msg->ParseFromArray((const void *)s, len)) {
        return luaL_error(L, "error deserializing message");
    }
    msg_udata * ud = (msg_udata *)lua_newuserdata(L, sizeof(msg_udata));
    ud->lua_owns = true;
    ud->msg = msg;
    ud->gc_callback = NULL;
    ud->callback_data = NULL;
    luaL_getmetatable(L, "protobuf_.zippylog.device.store_watcher.RunFinish");
    lua_setmetatable(L, -2);
    return 1;
}
int lua_protobuf_zippylog_device_store_watcher_RunFinish_gc(lua_State *L)
{
    msg_udata * mud = (msg_udata *)luaL_checkudata(L, 1, "protobuf_.zippylog.device.store_watcher.RunFinish");
    ::zippylog::device::store_watcher::RunFinish *m = (::zippylog::device::store_watcher::RunFinish *)mud->msg;
    if (mud->lua_owns) {
        delete mud->msg;
        mud->msg = NULL;
        return 0;
    }
    if (mud->gc_callback && mud->gc_callback(m, mud->callback_data)) {
        delete mud->msg;
        mud->msg = NULL;
        return 0;
    }
    return 0;
}
int lua_protobuf_zippylog_device_store_watcher_RunFinish_clear(lua_State *L)
{
    msg_udata * mud = (msg_udata *)luaL_checkudata(L, 1, "protobuf_.zippylog.device.store_watcher.RunFinish");
    ::zippylog::device::store_watcher::RunFinish *m = (::zippylog::device::store_watcher::RunFinish *)mud->msg;
    m->Clear();
    return 0;
}
int lua_protobuf_zippylog_device_store_watcher_RunFinish_serialized(lua_State *L)
{
    msg_udata * mud = (msg_udata *)luaL_checkudata(L, 1, "protobuf_.zippylog.device.store_watcher.RunFinish");
    ::zippylog::device::store_watcher::RunFinish *m = (::zippylog::device::store_watcher::RunFinish *)mud->msg;
    string s;
    if (!m->SerializeToString(&s)) {
        return luaL_error(L, "error serializing message");
    }
    lua_pushlstring(L, s.c_str(), s.length());
    return 1;
}
int lua_protobuf_zippylog_device_store_watcher_RunFinish_clear_id(lua_State *L)
{
    msg_udata * mud = (msg_udata *)luaL_checkudata(L, 1, "protobuf_.zippylog.device.store_watcher.RunFinish");
    ::zippylog::device::store_watcher::RunFinish *m = (::zippylog::device::store_watcher::RunFinish *)mud->msg;
    m->clear_id();
    return 0;
}

int lua_protobuf_zippylog_device_store_watcher_RunFinish_get_id(lua_State *L)
{
    msg_udata * mud = (msg_udata *)luaL_checkudata(L, 1, "protobuf_.zippylog.device.store_watcher.RunFinish");
    ::zippylog::device::store_watcher::RunFinish *m = (::zippylog::device::store_watcher::RunFinish *)mud->msg;
    string s = m->id();
    m->has_id() ? lua_pushlstring(L, s.c_str(), s.size()) : lua_pushnil(L);
    return 1;
}

int lua_protobuf_zippylog_device_store_watcher_RunFinish_set_id(lua_State *L)
{
    msg_udata * mud = (msg_udata *)luaL_checkudata(L, 1, "protobuf_.zippylog.device.store_watcher.RunFinish");
    ::zippylog::device::store_watcher::RunFinish *m = (::zippylog::device::store_watcher::RunFinish *)mud->msg;
    if (lua_isnil(L, 2)) {
        m->clear_id();
        return 0;
    }
    
    if (!lua_isstring(L, 2)) return luaL_error(L, "passed value is not a string");
    size_t len;
    const char *s = lua_tolstring(L, 2, &len);
    if (!s) {
        luaL_error(L, "could not obtain string on stack. weird");
    }
    m->set_id(s, len);
    return 0;
}

int lua_protobuf_zippylog_device_store_watcher_RunFinish_has_id(lua_State *L)
{
    msg_udata * mud = (msg_udata *)luaL_checkudata(L, 1, "protobuf_.zippylog.device.store_watcher.RunFinish");
    ::zippylog::device::store_watcher::RunFinish *m = (::zippylog::device::store_watcher::RunFinish *)mud->msg;
    lua_pushboolean(L, m->has_id());
    return 1;
}