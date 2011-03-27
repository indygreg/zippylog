
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

// Lua userdata type for all messages
typedef struct lua_protobuf_udata_t {
    // the underlying message instance
    ::google::protobuf::Message * msg;

    // whether Lua is the explicit owner of the message instance
    bool lua_owns;

    // garbage collection callback. may be NULL
    lua_protobuf_gc_callback gc_callback;

    // pointer to user-supplied data to be passed to callback
    void * callback_data;
} lua_protobuf_udata_t;

// __index and __newindex functions for enum tables
LUA_PROTOBUF_EXPORT int lua_protobuf_enum_index(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_enum_newindex(lua_State *L);

// GC callback function that always returns true
LUA_PROTOBUF_EXPORT int lua_protobuf_gc_always_free(::google::protobuf::Message *msg, void *userdata);

// Returns 1 if the value at the given index is a protocol buffer message type
// and 0 otherwise.
LUA_PROTOBUF_EXPORT int lua_protobuf_ismessage(lua_State *L, int index);

// Converts the Lua value at the given index to a protocol buffer message
// Returns NULL if the value is not a protocol buffer message
LUA_PROTOBUF_EXPORT ::google::protobuf::Message * lua_protobuf_tomessage(lua_State *L, int index);

#ifdef __cplusplus
}
#endif

#endif

