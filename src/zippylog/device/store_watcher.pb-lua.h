// Generated by the lua-protobuf compiler.
// You shouldn't be editing this file manually
//
// source proto file: zippylog/device/store_watcher.proto

#ifndef LUA_PROTOBUF_zippylog_device_store_watcher_H
#define LUA_PROTOBUF_zippylog_device_store_watcher_H

#include "lua-protobuf.h"
#include <zippylog/device/store_watcher.pb.h>

#ifdef __cplusplus
extern "C" {
#endif

#include <lua.h>

// register all messages in this package to a Lua state
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_device_store_watcher_open(lua_State *L);

// Message Create
// registers the message type with Lua
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_device_store_watcher_Create_open(lua_State *L);


// push a copy of the message to the Lua stack
// caller is free to use original message however she wants, but changes will not
// be reflected in Lua and vice-verse
LUA_PROTOBUF_EXPORT bool lua_protobuf_zippylog_device_store_watcher_Create_pushcopy(lua_State *L, const ::zippylog::device::store_watcher::Create &msg);

// push a reference of the message to the Lua stack
// the 3rd and 4th arguments define a callback that can be invoked just before Lua
// garbage collects the message. If the 3rd argument is NULL, Lua will *NOT* free
// memory. If the second argument points to a function, that function is called when
// Lua garbage collects the object. The function is sent a pointer to the message being
// collected and the 4th argument to this function. If the function returns true,
// Lua will free the memory. If false (0), Lua will not free the memory.
LUA_PROTOBUF_EXPORT bool lua_protobuf_zippylog_device_store_watcher_Create_pushreference(lua_State *L, ::zippylog::device::store_watcher::Create *msg, lua_protobuf_gc_callback callback, void *data);


// The following functions are called by Lua. Many people will not need them,
// but they are exported for those that do.


// constructor called from Lua
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_device_store_watcher_Create_new(lua_State *L);

// obtain instance from a serialized string
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_device_store_watcher_Create_parsefromstring(lua_State *L);

// garbage collects message instance in Lua
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_device_store_watcher_Create_gc(lua_State *L);

// obtain serialized representation of instance
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_device_store_watcher_Create_serialized(lua_State *L);

// clear all fields in the message
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_device_store_watcher_Create_clear(lua_State *L);

// required bytes id = 1
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_device_store_watcher_Create_clear_id(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_device_store_watcher_Create_get_id(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_device_store_watcher_Create_set_id(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_device_store_watcher_Create_has_id(lua_State *L);

// end of message Create

// Message Destroy
// registers the message type with Lua
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_device_store_watcher_Destroy_open(lua_State *L);


// push a copy of the message to the Lua stack
// caller is free to use original message however she wants, but changes will not
// be reflected in Lua and vice-verse
LUA_PROTOBUF_EXPORT bool lua_protobuf_zippylog_device_store_watcher_Destroy_pushcopy(lua_State *L, const ::zippylog::device::store_watcher::Destroy &msg);

// push a reference of the message to the Lua stack
// the 3rd and 4th arguments define a callback that can be invoked just before Lua
// garbage collects the message. If the 3rd argument is NULL, Lua will *NOT* free
// memory. If the second argument points to a function, that function is called when
// Lua garbage collects the object. The function is sent a pointer to the message being
// collected and the 4th argument to this function. If the function returns true,
// Lua will free the memory. If false (0), Lua will not free the memory.
LUA_PROTOBUF_EXPORT bool lua_protobuf_zippylog_device_store_watcher_Destroy_pushreference(lua_State *L, ::zippylog::device::store_watcher::Destroy *msg, lua_protobuf_gc_callback callback, void *data);


// The following functions are called by Lua. Many people will not need them,
// but they are exported for those that do.


// constructor called from Lua
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_device_store_watcher_Destroy_new(lua_State *L);

// obtain instance from a serialized string
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_device_store_watcher_Destroy_parsefromstring(lua_State *L);

// garbage collects message instance in Lua
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_device_store_watcher_Destroy_gc(lua_State *L);

// obtain serialized representation of instance
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_device_store_watcher_Destroy_serialized(lua_State *L);

// clear all fields in the message
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_device_store_watcher_Destroy_clear(lua_State *L);

// required bytes id = 1
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_device_store_watcher_Destroy_clear_id(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_device_store_watcher_Destroy_get_id(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_device_store_watcher_Destroy_set_id(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_device_store_watcher_Destroy_has_id(lua_State *L);

// end of message Destroy

// Message RunStart
// registers the message type with Lua
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_device_store_watcher_RunStart_open(lua_State *L);


// push a copy of the message to the Lua stack
// caller is free to use original message however she wants, but changes will not
// be reflected in Lua and vice-verse
LUA_PROTOBUF_EXPORT bool lua_protobuf_zippylog_device_store_watcher_RunStart_pushcopy(lua_State *L, const ::zippylog::device::store_watcher::RunStart &msg);

// push a reference of the message to the Lua stack
// the 3rd and 4th arguments define a callback that can be invoked just before Lua
// garbage collects the message. If the 3rd argument is NULL, Lua will *NOT* free
// memory. If the second argument points to a function, that function is called when
// Lua garbage collects the object. The function is sent a pointer to the message being
// collected and the 4th argument to this function. If the function returns true,
// Lua will free the memory. If false (0), Lua will not free the memory.
LUA_PROTOBUF_EXPORT bool lua_protobuf_zippylog_device_store_watcher_RunStart_pushreference(lua_State *L, ::zippylog::device::store_watcher::RunStart *msg, lua_protobuf_gc_callback callback, void *data);


// The following functions are called by Lua. Many people will not need them,
// but they are exported for those that do.


// constructor called from Lua
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_device_store_watcher_RunStart_new(lua_State *L);

// obtain instance from a serialized string
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_device_store_watcher_RunStart_parsefromstring(lua_State *L);

// garbage collects message instance in Lua
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_device_store_watcher_RunStart_gc(lua_State *L);

// obtain serialized representation of instance
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_device_store_watcher_RunStart_serialized(lua_State *L);

// clear all fields in the message
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_device_store_watcher_RunStart_clear(lua_State *L);

// required bytes id = 1
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_device_store_watcher_RunStart_clear_id(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_device_store_watcher_RunStart_get_id(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_device_store_watcher_RunStart_set_id(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_device_store_watcher_RunStart_has_id(lua_State *L);

// end of message RunStart

// Message RunFinish
// registers the message type with Lua
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_device_store_watcher_RunFinish_open(lua_State *L);


// push a copy of the message to the Lua stack
// caller is free to use original message however she wants, but changes will not
// be reflected in Lua and vice-verse
LUA_PROTOBUF_EXPORT bool lua_protobuf_zippylog_device_store_watcher_RunFinish_pushcopy(lua_State *L, const ::zippylog::device::store_watcher::RunFinish &msg);

// push a reference of the message to the Lua stack
// the 3rd and 4th arguments define a callback that can be invoked just before Lua
// garbage collects the message. If the 3rd argument is NULL, Lua will *NOT* free
// memory. If the second argument points to a function, that function is called when
// Lua garbage collects the object. The function is sent a pointer to the message being
// collected and the 4th argument to this function. If the function returns true,
// Lua will free the memory. If false (0), Lua will not free the memory.
LUA_PROTOBUF_EXPORT bool lua_protobuf_zippylog_device_store_watcher_RunFinish_pushreference(lua_State *L, ::zippylog::device::store_watcher::RunFinish *msg, lua_protobuf_gc_callback callback, void *data);


// The following functions are called by Lua. Many people will not need them,
// but they are exported for those that do.


// constructor called from Lua
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_device_store_watcher_RunFinish_new(lua_State *L);

// obtain instance from a serialized string
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_device_store_watcher_RunFinish_parsefromstring(lua_State *L);

// garbage collects message instance in Lua
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_device_store_watcher_RunFinish_gc(lua_State *L);

// obtain serialized representation of instance
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_device_store_watcher_RunFinish_serialized(lua_State *L);

// clear all fields in the message
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_device_store_watcher_RunFinish_clear(lua_State *L);

// required bytes id = 1
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_device_store_watcher_RunFinish_clear_id(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_device_store_watcher_RunFinish_get_id(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_device_store_watcher_RunFinish_set_id(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_device_store_watcher_RunFinish_has_id(lua_State *L);

// end of message RunFinish

#ifdef __cplusplus
}
#endif

#endif