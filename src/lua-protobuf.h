
#ifndef LUA_PROTOBUF_H
#define LUA_PROTOBUF_H

#include <google/protobuf/message.h>

#ifdef __cplusplus
extern "C" {
#endif

#include <lua.h>

#ifdef WINDOWS
#define LUA_PROTOBUF_EXPORT __declspec(dllexport)
#else
#define LUA_PROTOBUF_EXPORT
#endif

// type for callback function that is executed before Lua performs garbage
// collection on a message instance.
// if called function returns 1, Lua will free the memory backing the object
// if returns 0, Lua will not free the memory
typedef int (*lua_protobuf_gc_callback)(::google::protobuf::Message *msg, void *userdata);

// __index and __newindex functions for enum tables
LUA_PROTOBUF_EXPORT int lua_protobuf_enum_index(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_enum_newindex(lua_State *L);

// GC callback function that always returns true
LUA_PROTOBUF_EXPORT int lua_protobuf_gc_always_free(::google::protobuf::Message *msg, void *userdata);

#ifdef __cplusplus
}
#endif

#endif

