// Generated by the lua-protobuf compiler.
// You shouldn't be editing this file manually
//
// source proto file: zippylog/zippylogd.proto

#ifndef LUA_PROTOBUF_zippylog_zippylogd_H
#define LUA_PROTOBUF_zippylog_zippylogd_H

#include "lua-protobuf.h"
#include <zippylog/zippylogd.pb.h>

#ifdef __cplusplus
extern "C" {
#endif

#include <lua.h>

// register all messages in this package to a Lua state
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_zippylogd_open(lua_State *L);

// Message BrokerStartup
// registers the message type with Lua
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_zippylogd_BrokerStartup_open(lua_State *L);


// push a copy of the message to the Lua stack
// caller is free to use original message however she wants, but changes will not
// be reflected in Lua and vice-verse
LUA_PROTOBUF_EXPORT bool lua_protobuf_zippylog_zippylogd_BrokerStartup_pushcopy(lua_State *L, const ::zippylog::zippylogd::BrokerStartup &msg);

// push a reference of the message to the Lua stack
// the 3rd and 4th arguments define a callback that can be invoked just before Lua
// garbage collects the message. If the 3rd argument is NULL, Lua will *NOT* free
// memory. If the second argument points to a function, that function is called when
// Lua garbage collects the object. The function is sent a pointer to the message being
// collected and the 4th argument to this function. If the function returns true,
// Lua will free the memory. If false (0), Lua will not free the memory.
LUA_PROTOBUF_EXPORT bool lua_protobuf_zippylog_zippylogd_BrokerStartup_pushreference(lua_State *L, ::zippylog::zippylogd::BrokerStartup *msg, lua_protobuf_gc_callback callback, void *data);


// The following functions are called by Lua. Many people will not need them,
// but they are exported for those that do.


// constructor called from Lua
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_zippylogd_BrokerStartup_new(lua_State *L);

// obtain instance from a serialized string
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_zippylogd_BrokerStartup_parsefromstring(lua_State *L);

// garbage collects message instance in Lua
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_zippylogd_BrokerStartup_gc(lua_State *L);

// obtain serialized representation of instance
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_zippylogd_BrokerStartup_serialized(lua_State *L);

// clear all fields in the message
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_zippylogd_BrokerStartup_clear(lua_State *L);

// optional bytes id = 1
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_zippylogd_BrokerStartup_clear_id(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_zippylogd_BrokerStartup_get_id(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_zippylogd_BrokerStartup_set_id(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_zippylogd_BrokerStartup_has_id(lua_State *L);

// optional string store_path = 2
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_zippylogd_BrokerStartup_clear_store_path(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_zippylogd_BrokerStartup_get_store_path(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_zippylogd_BrokerStartup_set_store_path(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_zippylogd_BrokerStartup_has_store_path(lua_State *L);

// repeated string listen_endpoints = 3
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_zippylogd_BrokerStartup_clear_listen_endpoints(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_zippylogd_BrokerStartup_get_listen_endpoints(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_zippylogd_BrokerStartup_set_listen_endpoints(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_zippylogd_BrokerStartup_size_listen_endpoints(lua_State *L);

// end of message BrokerStartup

// Message BrokerShutdown
// registers the message type with Lua
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_zippylogd_BrokerShutdown_open(lua_State *L);


// push a copy of the message to the Lua stack
// caller is free to use original message however she wants, but changes will not
// be reflected in Lua and vice-verse
LUA_PROTOBUF_EXPORT bool lua_protobuf_zippylog_zippylogd_BrokerShutdown_pushcopy(lua_State *L, const ::zippylog::zippylogd::BrokerShutdown &msg);

// push a reference of the message to the Lua stack
// the 3rd and 4th arguments define a callback that can be invoked just before Lua
// garbage collects the message. If the 3rd argument is NULL, Lua will *NOT* free
// memory. If the second argument points to a function, that function is called when
// Lua garbage collects the object. The function is sent a pointer to the message being
// collected and the 4th argument to this function. If the function returns true,
// Lua will free the memory. If false (0), Lua will not free the memory.
LUA_PROTOBUF_EXPORT bool lua_protobuf_zippylog_zippylogd_BrokerShutdown_pushreference(lua_State *L, ::zippylog::zippylogd::BrokerShutdown *msg, lua_protobuf_gc_callback callback, void *data);


// The following functions are called by Lua. Many people will not need them,
// but they are exported for those that do.


// constructor called from Lua
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_zippylogd_BrokerShutdown_new(lua_State *L);

// obtain instance from a serialized string
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_zippylogd_BrokerShutdown_parsefromstring(lua_State *L);

// garbage collects message instance in Lua
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_zippylogd_BrokerShutdown_gc(lua_State *L);

// obtain serialized representation of instance
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_zippylogd_BrokerShutdown_serialized(lua_State *L);

// clear all fields in the message
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_zippylogd_BrokerShutdown_clear(lua_State *L);

// optional bytes id = 1
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_zippylogd_BrokerShutdown_clear_id(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_zippylogd_BrokerShutdown_get_id(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_zippylogd_BrokerShutdown_set_id(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_zippylogd_BrokerShutdown_has_id(lua_State *L);

// end of message BrokerShutdown

// Message BrokerReceiveClientMessage
// registers the message type with Lua
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_zippylogd_BrokerReceiveClientMessage_open(lua_State *L);


// push a copy of the message to the Lua stack
// caller is free to use original message however she wants, but changes will not
// be reflected in Lua and vice-verse
LUA_PROTOBUF_EXPORT bool lua_protobuf_zippylog_zippylogd_BrokerReceiveClientMessage_pushcopy(lua_State *L, const ::zippylog::zippylogd::BrokerReceiveClientMessage &msg);

// push a reference of the message to the Lua stack
// the 3rd and 4th arguments define a callback that can be invoked just before Lua
// garbage collects the message. If the 3rd argument is NULL, Lua will *NOT* free
// memory. If the second argument points to a function, that function is called when
// Lua garbage collects the object. The function is sent a pointer to the message being
// collected and the 4th argument to this function. If the function returns true,
// Lua will free the memory. If false (0), Lua will not free the memory.
LUA_PROTOBUF_EXPORT bool lua_protobuf_zippylog_zippylogd_BrokerReceiveClientMessage_pushreference(lua_State *L, ::zippylog::zippylogd::BrokerReceiveClientMessage *msg, lua_protobuf_gc_callback callback, void *data);


// The following functions are called by Lua. Many people will not need them,
// but they are exported for those that do.


// constructor called from Lua
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_zippylogd_BrokerReceiveClientMessage_new(lua_State *L);

// obtain instance from a serialized string
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_zippylogd_BrokerReceiveClientMessage_parsefromstring(lua_State *L);

// garbage collects message instance in Lua
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_zippylogd_BrokerReceiveClientMessage_gc(lua_State *L);

// obtain serialized representation of instance
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_zippylogd_BrokerReceiveClientMessage_serialized(lua_State *L);

// clear all fields in the message
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_zippylogd_BrokerReceiveClientMessage_clear(lua_State *L);

// end of message BrokerReceiveClientMessage

// Message BrokerFlushOutputStreams
// registers the message type with Lua
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_zippylogd_BrokerFlushOutputStreams_open(lua_State *L);


// push a copy of the message to the Lua stack
// caller is free to use original message however she wants, but changes will not
// be reflected in Lua and vice-verse
LUA_PROTOBUF_EXPORT bool lua_protobuf_zippylog_zippylogd_BrokerFlushOutputStreams_pushcopy(lua_State *L, const ::zippylog::zippylogd::BrokerFlushOutputStreams &msg);

// push a reference of the message to the Lua stack
// the 3rd and 4th arguments define a callback that can be invoked just before Lua
// garbage collects the message. If the 3rd argument is NULL, Lua will *NOT* free
// memory. If the second argument points to a function, that function is called when
// Lua garbage collects the object. The function is sent a pointer to the message being
// collected and the 4th argument to this function. If the function returns true,
// Lua will free the memory. If false (0), Lua will not free the memory.
LUA_PROTOBUF_EXPORT bool lua_protobuf_zippylog_zippylogd_BrokerFlushOutputStreams_pushreference(lua_State *L, ::zippylog::zippylogd::BrokerFlushOutputStreams *msg, lua_protobuf_gc_callback callback, void *data);


// The following functions are called by Lua. Many people will not need them,
// but they are exported for those that do.


// constructor called from Lua
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_zippylogd_BrokerFlushOutputStreams_new(lua_State *L);

// obtain instance from a serialized string
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_zippylogd_BrokerFlushOutputStreams_parsefromstring(lua_State *L);

// garbage collects message instance in Lua
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_zippylogd_BrokerFlushOutputStreams_gc(lua_State *L);

// obtain serialized representation of instance
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_zippylogd_BrokerFlushOutputStreams_serialized(lua_State *L);

// clear all fields in the message
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_zippylogd_BrokerFlushOutputStreams_clear(lua_State *L);

// optional bytes id = 1
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_zippylogd_BrokerFlushOutputStreams_clear_id(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_zippylogd_BrokerFlushOutputStreams_get_id(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_zippylogd_BrokerFlushOutputStreams_set_id(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_zippylogd_BrokerFlushOutputStreams_has_id(lua_State *L);

// end of message BrokerFlushOutputStreams

// Message StoreWatcherStartup
// registers the message type with Lua
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_zippylogd_StoreWatcherStartup_open(lua_State *L);


// push a copy of the message to the Lua stack
// caller is free to use original message however she wants, but changes will not
// be reflected in Lua and vice-verse
LUA_PROTOBUF_EXPORT bool lua_protobuf_zippylog_zippylogd_StoreWatcherStartup_pushcopy(lua_State *L, const ::zippylog::zippylogd::StoreWatcherStartup &msg);

// push a reference of the message to the Lua stack
// the 3rd and 4th arguments define a callback that can be invoked just before Lua
// garbage collects the message. If the 3rd argument is NULL, Lua will *NOT* free
// memory. If the second argument points to a function, that function is called when
// Lua garbage collects the object. The function is sent a pointer to the message being
// collected and the 4th argument to this function. If the function returns true,
// Lua will free the memory. If false (0), Lua will not free the memory.
LUA_PROTOBUF_EXPORT bool lua_protobuf_zippylog_zippylogd_StoreWatcherStartup_pushreference(lua_State *L, ::zippylog::zippylogd::StoreWatcherStartup *msg, lua_protobuf_gc_callback callback, void *data);


// The following functions are called by Lua. Many people will not need them,
// but they are exported for those that do.


// constructor called from Lua
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_zippylogd_StoreWatcherStartup_new(lua_State *L);

// obtain instance from a serialized string
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_zippylogd_StoreWatcherStartup_parsefromstring(lua_State *L);

// garbage collects message instance in Lua
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_zippylogd_StoreWatcherStartup_gc(lua_State *L);

// obtain serialized representation of instance
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_zippylogd_StoreWatcherStartup_serialized(lua_State *L);

// clear all fields in the message
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_zippylogd_StoreWatcherStartup_clear(lua_State *L);

// optional bytes id = 1
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_zippylogd_StoreWatcherStartup_clear_id(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_zippylogd_StoreWatcherStartup_get_id(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_zippylogd_StoreWatcherStartup_set_id(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_zippylogd_StoreWatcherStartup_has_id(lua_State *L);

// end of message StoreWatcherStartup

// Message StoreWatcherShutdown
// registers the message type with Lua
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_zippylogd_StoreWatcherShutdown_open(lua_State *L);


// push a copy of the message to the Lua stack
// caller is free to use original message however she wants, but changes will not
// be reflected in Lua and vice-verse
LUA_PROTOBUF_EXPORT bool lua_protobuf_zippylog_zippylogd_StoreWatcherShutdown_pushcopy(lua_State *L, const ::zippylog::zippylogd::StoreWatcherShutdown &msg);

// push a reference of the message to the Lua stack
// the 3rd and 4th arguments define a callback that can be invoked just before Lua
// garbage collects the message. If the 3rd argument is NULL, Lua will *NOT* free
// memory. If the second argument points to a function, that function is called when
// Lua garbage collects the object. The function is sent a pointer to the message being
// collected and the 4th argument to this function. If the function returns true,
// Lua will free the memory. If false (0), Lua will not free the memory.
LUA_PROTOBUF_EXPORT bool lua_protobuf_zippylog_zippylogd_StoreWatcherShutdown_pushreference(lua_State *L, ::zippylog::zippylogd::StoreWatcherShutdown *msg, lua_protobuf_gc_callback callback, void *data);


// The following functions are called by Lua. Many people will not need them,
// but they are exported for those that do.


// constructor called from Lua
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_zippylogd_StoreWatcherShutdown_new(lua_State *L);

// obtain instance from a serialized string
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_zippylogd_StoreWatcherShutdown_parsefromstring(lua_State *L);

// garbage collects message instance in Lua
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_zippylogd_StoreWatcherShutdown_gc(lua_State *L);

// obtain serialized representation of instance
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_zippylogd_StoreWatcherShutdown_serialized(lua_State *L);

// clear all fields in the message
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_zippylogd_StoreWatcherShutdown_clear(lua_State *L);

// optional bytes id = 1
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_zippylogd_StoreWatcherShutdown_clear_id(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_zippylogd_StoreWatcherShutdown_get_id(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_zippylogd_StoreWatcherShutdown_set_id(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_zippylogd_StoreWatcherShutdown_has_id(lua_State *L);

// end of message StoreWatcherShutdown

#ifdef __cplusplus
}
#endif

#endif