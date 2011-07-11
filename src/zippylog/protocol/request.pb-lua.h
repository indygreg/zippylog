// Generated by the lua-protobuf compiler.
// You shouldn't be editing this file manually
//
// source proto file: zippylog/protocol/request.proto

#ifndef LUA_PROTOBUF_zippylog_protocol_request_H
#define LUA_PROTOBUF_zippylog_protocol_request_H

#include "lua-protobuf.h"
#include <zippylog/protocol/request.pb.h>

#ifdef __cplusplus
extern "C" {
#endif

#include <lua.h>

// register all messages in this package to a Lua state
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_open(lua_State *L);

// Message PingV1
// registers the message type with Lua
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_PingV1_open(lua_State *L);


// push a copy of the message to the Lua stack
// caller is free to use original message however she wants, but changes will not
// be reflected in Lua and vice-verse
LUA_PROTOBUF_EXPORT bool lua_protobuf_zippylog_protocol_request_PingV1_pushcopy(lua_State *L, const ::zippylog::protocol::request::PingV1 &msg);

// push a reference of the message to the Lua stack
// the 3rd and 4th arguments define a callback that can be invoked just before Lua
// garbage collects the message. If the 3rd argument is NULL, Lua will *NOT* free
// memory. If the second argument points to a function, that function is called when
// Lua garbage collects the object. The function is sent a pointer to the message being
// collected and the 4th argument to this function. If the function returns true,
// Lua will free the memory. If false (0), Lua will not free the memory.
LUA_PROTOBUF_EXPORT bool lua_protobuf_zippylog_protocol_request_PingV1_pushreference(lua_State *L, ::zippylog::protocol::request::PingV1 *msg, lua_protobuf_gc_callback callback, void *data);


// The following functions are called by Lua. Many people will not need them,
// but they are exported for those that do.


// constructor called from Lua
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_PingV1_new(lua_State *L);

// obtain instance from a serialized string
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_PingV1_parsefromstring(lua_State *L);

// garbage collects message instance in Lua
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_PingV1_gc(lua_State *L);

// obtain serialized representation of instance
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_PingV1_serialized(lua_State *L);

// clear all fields in the message
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_PingV1_clear(lua_State *L);

// end of message PingV1

// Message GetFeaturesV1
// registers the message type with Lua
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_GetFeaturesV1_open(lua_State *L);


// push a copy of the message to the Lua stack
// caller is free to use original message however she wants, but changes will not
// be reflected in Lua and vice-verse
LUA_PROTOBUF_EXPORT bool lua_protobuf_zippylog_protocol_request_GetFeaturesV1_pushcopy(lua_State *L, const ::zippylog::protocol::request::GetFeaturesV1 &msg);

// push a reference of the message to the Lua stack
// the 3rd and 4th arguments define a callback that can be invoked just before Lua
// garbage collects the message. If the 3rd argument is NULL, Lua will *NOT* free
// memory. If the second argument points to a function, that function is called when
// Lua garbage collects the object. The function is sent a pointer to the message being
// collected and the 4th argument to this function. If the function returns true,
// Lua will free the memory. If false (0), Lua will not free the memory.
LUA_PROTOBUF_EXPORT bool lua_protobuf_zippylog_protocol_request_GetFeaturesV1_pushreference(lua_State *L, ::zippylog::protocol::request::GetFeaturesV1 *msg, lua_protobuf_gc_callback callback, void *data);


// The following functions are called by Lua. Many people will not need them,
// but they are exported for those that do.


// constructor called from Lua
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_GetFeaturesV1_new(lua_State *L);

// obtain instance from a serialized string
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_GetFeaturesV1_parsefromstring(lua_State *L);

// garbage collects message instance in Lua
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_GetFeaturesV1_gc(lua_State *L);

// obtain serialized representation of instance
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_GetFeaturesV1_serialized(lua_State *L);

// clear all fields in the message
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_GetFeaturesV1_clear(lua_State *L);

// end of message GetFeaturesV1

// Message GetStoreInfoV1
// registers the message type with Lua
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_GetStoreInfoV1_open(lua_State *L);


// push a copy of the message to the Lua stack
// caller is free to use original message however she wants, but changes will not
// be reflected in Lua and vice-verse
LUA_PROTOBUF_EXPORT bool lua_protobuf_zippylog_protocol_request_GetStoreInfoV1_pushcopy(lua_State *L, const ::zippylog::protocol::request::GetStoreInfoV1 &msg);

// push a reference of the message to the Lua stack
// the 3rd and 4th arguments define a callback that can be invoked just before Lua
// garbage collects the message. If the 3rd argument is NULL, Lua will *NOT* free
// memory. If the second argument points to a function, that function is called when
// Lua garbage collects the object. The function is sent a pointer to the message being
// collected and the 4th argument to this function. If the function returns true,
// Lua will free the memory. If false (0), Lua will not free the memory.
LUA_PROTOBUF_EXPORT bool lua_protobuf_zippylog_protocol_request_GetStoreInfoV1_pushreference(lua_State *L, ::zippylog::protocol::request::GetStoreInfoV1 *msg, lua_protobuf_gc_callback callback, void *data);


// The following functions are called by Lua. Many people will not need them,
// but they are exported for those that do.


// constructor called from Lua
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_GetStoreInfoV1_new(lua_State *L);

// obtain instance from a serialized string
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_GetStoreInfoV1_parsefromstring(lua_State *L);

// garbage collects message instance in Lua
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_GetStoreInfoV1_gc(lua_State *L);

// obtain serialized representation of instance
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_GetStoreInfoV1_serialized(lua_State *L);

// clear all fields in the message
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_GetStoreInfoV1_clear(lua_State *L);

// end of message GetStoreInfoV1

// Message GetBucketInfoV1
// registers the message type with Lua
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_GetBucketInfoV1_open(lua_State *L);


// push a copy of the message to the Lua stack
// caller is free to use original message however she wants, but changes will not
// be reflected in Lua and vice-verse
LUA_PROTOBUF_EXPORT bool lua_protobuf_zippylog_protocol_request_GetBucketInfoV1_pushcopy(lua_State *L, const ::zippylog::protocol::request::GetBucketInfoV1 &msg);

// push a reference of the message to the Lua stack
// the 3rd and 4th arguments define a callback that can be invoked just before Lua
// garbage collects the message. If the 3rd argument is NULL, Lua will *NOT* free
// memory. If the second argument points to a function, that function is called when
// Lua garbage collects the object. The function is sent a pointer to the message being
// collected and the 4th argument to this function. If the function returns true,
// Lua will free the memory. If false (0), Lua will not free the memory.
LUA_PROTOBUF_EXPORT bool lua_protobuf_zippylog_protocol_request_GetBucketInfoV1_pushreference(lua_State *L, ::zippylog::protocol::request::GetBucketInfoV1 *msg, lua_protobuf_gc_callback callback, void *data);


// The following functions are called by Lua. Many people will not need them,
// but they are exported for those that do.


// constructor called from Lua
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_GetBucketInfoV1_new(lua_State *L);

// obtain instance from a serialized string
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_GetBucketInfoV1_parsefromstring(lua_State *L);

// garbage collects message instance in Lua
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_GetBucketInfoV1_gc(lua_State *L);

// obtain serialized representation of instance
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_GetBucketInfoV1_serialized(lua_State *L);

// clear all fields in the message
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_GetBucketInfoV1_clear(lua_State *L);

// required string path = 1
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_GetBucketInfoV1_clear_path(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_GetBucketInfoV1_get_path(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_GetBucketInfoV1_set_path(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_GetBucketInfoV1_has_path(lua_State *L);

// end of message GetBucketInfoV1

// Message GetStreamSetInfoV1
// registers the message type with Lua
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_GetStreamSetInfoV1_open(lua_State *L);


// push a copy of the message to the Lua stack
// caller is free to use original message however she wants, but changes will not
// be reflected in Lua and vice-verse
LUA_PROTOBUF_EXPORT bool lua_protobuf_zippylog_protocol_request_GetStreamSetInfoV1_pushcopy(lua_State *L, const ::zippylog::protocol::request::GetStreamSetInfoV1 &msg);

// push a reference of the message to the Lua stack
// the 3rd and 4th arguments define a callback that can be invoked just before Lua
// garbage collects the message. If the 3rd argument is NULL, Lua will *NOT* free
// memory. If the second argument points to a function, that function is called when
// Lua garbage collects the object. The function is sent a pointer to the message being
// collected and the 4th argument to this function. If the function returns true,
// Lua will free the memory. If false (0), Lua will not free the memory.
LUA_PROTOBUF_EXPORT bool lua_protobuf_zippylog_protocol_request_GetStreamSetInfoV1_pushreference(lua_State *L, ::zippylog::protocol::request::GetStreamSetInfoV1 *msg, lua_protobuf_gc_callback callback, void *data);


// The following functions are called by Lua. Many people will not need them,
// but they are exported for those that do.


// constructor called from Lua
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_GetStreamSetInfoV1_new(lua_State *L);

// obtain instance from a serialized string
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_GetStreamSetInfoV1_parsefromstring(lua_State *L);

// garbage collects message instance in Lua
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_GetStreamSetInfoV1_gc(lua_State *L);

// obtain serialized representation of instance
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_GetStreamSetInfoV1_serialized(lua_State *L);

// clear all fields in the message
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_GetStreamSetInfoV1_clear(lua_State *L);

// required string path = 1
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_GetStreamSetInfoV1_clear_path(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_GetStreamSetInfoV1_get_path(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_GetStreamSetInfoV1_set_path(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_GetStreamSetInfoV1_has_path(lua_State *L);

// end of message GetStreamSetInfoV1

// Message GetStreamInfoV1
// registers the message type with Lua
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_GetStreamInfoV1_open(lua_State *L);


// push a copy of the message to the Lua stack
// caller is free to use original message however she wants, but changes will not
// be reflected in Lua and vice-verse
LUA_PROTOBUF_EXPORT bool lua_protobuf_zippylog_protocol_request_GetStreamInfoV1_pushcopy(lua_State *L, const ::zippylog::protocol::request::GetStreamInfoV1 &msg);

// push a reference of the message to the Lua stack
// the 3rd and 4th arguments define a callback that can be invoked just before Lua
// garbage collects the message. If the 3rd argument is NULL, Lua will *NOT* free
// memory. If the second argument points to a function, that function is called when
// Lua garbage collects the object. The function is sent a pointer to the message being
// collected and the 4th argument to this function. If the function returns true,
// Lua will free the memory. If false (0), Lua will not free the memory.
LUA_PROTOBUF_EXPORT bool lua_protobuf_zippylog_protocol_request_GetStreamInfoV1_pushreference(lua_State *L, ::zippylog::protocol::request::GetStreamInfoV1 *msg, lua_protobuf_gc_callback callback, void *data);


// The following functions are called by Lua. Many people will not need them,
// but they are exported for those that do.


// constructor called from Lua
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_GetStreamInfoV1_new(lua_State *L);

// obtain instance from a serialized string
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_GetStreamInfoV1_parsefromstring(lua_State *L);

// garbage collects message instance in Lua
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_GetStreamInfoV1_gc(lua_State *L);

// obtain serialized representation of instance
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_GetStreamInfoV1_serialized(lua_State *L);

// clear all fields in the message
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_GetStreamInfoV1_clear(lua_State *L);

// required string path = 1
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_GetStreamInfoV1_clear_path(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_GetStreamInfoV1_get_path(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_GetStreamInfoV1_set_path(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_GetStreamInfoV1_has_path(lua_State *L);

// end of message GetStreamInfoV1

// Message GetStreamSegmentV1
// registers the message type with Lua
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_GetStreamSegmentV1_open(lua_State *L);


// push a copy of the message to the Lua stack
// caller is free to use original message however she wants, but changes will not
// be reflected in Lua and vice-verse
LUA_PROTOBUF_EXPORT bool lua_protobuf_zippylog_protocol_request_GetStreamSegmentV1_pushcopy(lua_State *L, const ::zippylog::protocol::request::GetStreamSegmentV1 &msg);

// push a reference of the message to the Lua stack
// the 3rd and 4th arguments define a callback that can be invoked just before Lua
// garbage collects the message. If the 3rd argument is NULL, Lua will *NOT* free
// memory. If the second argument points to a function, that function is called when
// Lua garbage collects the object. The function is sent a pointer to the message being
// collected and the 4th argument to this function. If the function returns true,
// Lua will free the memory. If false (0), Lua will not free the memory.
LUA_PROTOBUF_EXPORT bool lua_protobuf_zippylog_protocol_request_GetStreamSegmentV1_pushreference(lua_State *L, ::zippylog::protocol::request::GetStreamSegmentV1 *msg, lua_protobuf_gc_callback callback, void *data);


// The following functions are called by Lua. Many people will not need them,
// but they are exported for those that do.


// constructor called from Lua
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_GetStreamSegmentV1_new(lua_State *L);

// obtain instance from a serialized string
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_GetStreamSegmentV1_parsefromstring(lua_State *L);

// garbage collects message instance in Lua
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_GetStreamSegmentV1_gc(lua_State *L);

// obtain serialized representation of instance
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_GetStreamSegmentV1_serialized(lua_State *L);

// clear all fields in the message
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_GetStreamSegmentV1_clear(lua_State *L);

// required string path = 1
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_GetStreamSegmentV1_clear_path(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_GetStreamSegmentV1_get_path(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_GetStreamSegmentV1_set_path(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_GetStreamSegmentV1_has_path(lua_State *L);

// required uint64 start_offset = 2
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_GetStreamSegmentV1_clear_start_offset(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_GetStreamSegmentV1_get_start_offset(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_GetStreamSegmentV1_set_start_offset(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_GetStreamSegmentV1_has_start_offset(lua_State *L);

// optional uint32 max_response_bytes = 3
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_GetStreamSegmentV1_clear_max_response_bytes(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_GetStreamSegmentV1_get_max_response_bytes(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_GetStreamSegmentV1_set_max_response_bytes(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_GetStreamSegmentV1_has_max_response_bytes(lua_State *L);

// optional uint32 max_response_envelopes = 4
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_GetStreamSegmentV1_clear_max_response_envelopes(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_GetStreamSegmentV1_get_max_response_envelopes(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_GetStreamSegmentV1_set_max_response_envelopes(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_GetStreamSegmentV1_has_max_response_envelopes(lua_State *L);

// end of message GetStreamSegmentV1

// Message WriteEnvelopeV1
// registers the message type with Lua
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_WriteEnvelopeV1_open(lua_State *L);


// push a copy of the message to the Lua stack
// caller is free to use original message however she wants, but changes will not
// be reflected in Lua and vice-verse
LUA_PROTOBUF_EXPORT bool lua_protobuf_zippylog_protocol_request_WriteEnvelopeV1_pushcopy(lua_State *L, const ::zippylog::protocol::request::WriteEnvelopeV1 &msg);

// push a reference of the message to the Lua stack
// the 3rd and 4th arguments define a callback that can be invoked just before Lua
// garbage collects the message. If the 3rd argument is NULL, Lua will *NOT* free
// memory. If the second argument points to a function, that function is called when
// Lua garbage collects the object. The function is sent a pointer to the message being
// collected and the 4th argument to this function. If the function returns true,
// Lua will free the memory. If false (0), Lua will not free the memory.
LUA_PROTOBUF_EXPORT bool lua_protobuf_zippylog_protocol_request_WriteEnvelopeV1_pushreference(lua_State *L, ::zippylog::protocol::request::WriteEnvelopeV1 *msg, lua_protobuf_gc_callback callback, void *data);


// The following functions are called by Lua. Many people will not need them,
// but they are exported for those that do.


// constructor called from Lua
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_WriteEnvelopeV1_new(lua_State *L);

// obtain instance from a serialized string
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_WriteEnvelopeV1_parsefromstring(lua_State *L);

// garbage collects message instance in Lua
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_WriteEnvelopeV1_gc(lua_State *L);

// obtain serialized representation of instance
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_WriteEnvelopeV1_serialized(lua_State *L);

// clear all fields in the message
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_WriteEnvelopeV1_clear(lua_State *L);

// required string path = 1
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_WriteEnvelopeV1_clear_path(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_WriteEnvelopeV1_get_path(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_WriteEnvelopeV1_set_path(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_WriteEnvelopeV1_has_path(lua_State *L);

// repeated bytes envelope = 2
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_WriteEnvelopeV1_clear_envelope(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_WriteEnvelopeV1_get_envelope(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_WriteEnvelopeV1_set_envelope(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_WriteEnvelopeV1_size_envelope(lua_State *L);

// optional bool acknowledge = 3
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_WriteEnvelopeV1_clear_acknowledge(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_WriteEnvelopeV1_get_acknowledge(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_WriteEnvelopeV1_set_acknowledge(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_WriteEnvelopeV1_has_acknowledge(lua_State *L);

// optional bool synchronous = 4
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_WriteEnvelopeV1_clear_synchronous(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_WriteEnvelopeV1_get_synchronous(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_WriteEnvelopeV1_set_synchronous(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_WriteEnvelopeV1_has_synchronous(lua_State *L);

// end of message WriteEnvelopeV1

// Message WriteEnvelopeFromMessagesV1
// registers the message type with Lua
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_WriteEnvelopeFromMessagesV1_open(lua_State *L);


// push a copy of the message to the Lua stack
// caller is free to use original message however she wants, but changes will not
// be reflected in Lua and vice-verse
LUA_PROTOBUF_EXPORT bool lua_protobuf_zippylog_protocol_request_WriteEnvelopeFromMessagesV1_pushcopy(lua_State *L, const ::zippylog::protocol::request::WriteEnvelopeFromMessagesV1 &msg);

// push a reference of the message to the Lua stack
// the 3rd and 4th arguments define a callback that can be invoked just before Lua
// garbage collects the message. If the 3rd argument is NULL, Lua will *NOT* free
// memory. If the second argument points to a function, that function is called when
// Lua garbage collects the object. The function is sent a pointer to the message being
// collected and the 4th argument to this function. If the function returns true,
// Lua will free the memory. If false (0), Lua will not free the memory.
LUA_PROTOBUF_EXPORT bool lua_protobuf_zippylog_protocol_request_WriteEnvelopeFromMessagesV1_pushreference(lua_State *L, ::zippylog::protocol::request::WriteEnvelopeFromMessagesV1 *msg, lua_protobuf_gc_callback callback, void *data);


// The following functions are called by Lua. Many people will not need them,
// but they are exported for those that do.


// constructor called from Lua
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_WriteEnvelopeFromMessagesV1_new(lua_State *L);

// obtain instance from a serialized string
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_WriteEnvelopeFromMessagesV1_parsefromstring(lua_State *L);

// garbage collects message instance in Lua
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_WriteEnvelopeFromMessagesV1_gc(lua_State *L);

// obtain serialized representation of instance
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_WriteEnvelopeFromMessagesV1_serialized(lua_State *L);

// clear all fields in the message
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_WriteEnvelopeFromMessagesV1_clear(lua_State *L);

// required string path = 1
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_WriteEnvelopeFromMessagesV1_clear_path(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_WriteEnvelopeFromMessagesV1_get_path(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_WriteEnvelopeFromMessagesV1_set_path(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_WriteEnvelopeFromMessagesV1_has_path(lua_State *L);

// repeated bytes message = 2
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_WriteEnvelopeFromMessagesV1_clear_message(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_WriteEnvelopeFromMessagesV1_get_message(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_WriteEnvelopeFromMessagesV1_set_message(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_WriteEnvelopeFromMessagesV1_size_message(lua_State *L);

// repeated uint32 message_namespace = 3
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_WriteEnvelopeFromMessagesV1_clear_message_namespace(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_WriteEnvelopeFromMessagesV1_get_message_namespace(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_WriteEnvelopeFromMessagesV1_set_message_namespace(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_WriteEnvelopeFromMessagesV1_size_message_namespace(lua_State *L);

// repeated uint32 message_type = 4
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_WriteEnvelopeFromMessagesV1_clear_message_type(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_WriteEnvelopeFromMessagesV1_get_message_type(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_WriteEnvelopeFromMessagesV1_set_message_type(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_WriteEnvelopeFromMessagesV1_size_message_type(lua_State *L);

// end of message WriteEnvelopeFromMessagesV1

// Message SubscribeStoreChangesV1
// registers the message type with Lua
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_SubscribeStoreChangesV1_open(lua_State *L);


// push a copy of the message to the Lua stack
// caller is free to use original message however she wants, but changes will not
// be reflected in Lua and vice-verse
LUA_PROTOBUF_EXPORT bool lua_protobuf_zippylog_protocol_request_SubscribeStoreChangesV1_pushcopy(lua_State *L, const ::zippylog::protocol::request::SubscribeStoreChangesV1 &msg);

// push a reference of the message to the Lua stack
// the 3rd and 4th arguments define a callback that can be invoked just before Lua
// garbage collects the message. If the 3rd argument is NULL, Lua will *NOT* free
// memory. If the second argument points to a function, that function is called when
// Lua garbage collects the object. The function is sent a pointer to the message being
// collected and the 4th argument to this function. If the function returns true,
// Lua will free the memory. If false (0), Lua will not free the memory.
LUA_PROTOBUF_EXPORT bool lua_protobuf_zippylog_protocol_request_SubscribeStoreChangesV1_pushreference(lua_State *L, ::zippylog::protocol::request::SubscribeStoreChangesV1 *msg, lua_protobuf_gc_callback callback, void *data);


// The following functions are called by Lua. Many people will not need them,
// but they are exported for those that do.


// constructor called from Lua
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_SubscribeStoreChangesV1_new(lua_State *L);

// obtain instance from a serialized string
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_SubscribeStoreChangesV1_parsefromstring(lua_State *L);

// garbage collects message instance in Lua
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_SubscribeStoreChangesV1_gc(lua_State *L);

// obtain serialized representation of instance
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_SubscribeStoreChangesV1_serialized(lua_State *L);

// clear all fields in the message
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_SubscribeStoreChangesV1_clear(lua_State *L);

// repeated string path = 1
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_SubscribeStoreChangesV1_clear_path(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_SubscribeStoreChangesV1_get_path(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_SubscribeStoreChangesV1_set_path(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_SubscribeStoreChangesV1_size_path(lua_State *L);

// end of message SubscribeStoreChangesV1

// Message SubscribeEnvelopesV1
// registers the message type with Lua
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_SubscribeEnvelopesV1_open(lua_State *L);


// push a copy of the message to the Lua stack
// caller is free to use original message however she wants, but changes will not
// be reflected in Lua and vice-verse
LUA_PROTOBUF_EXPORT bool lua_protobuf_zippylog_protocol_request_SubscribeEnvelopesV1_pushcopy(lua_State *L, const ::zippylog::protocol::request::SubscribeEnvelopesV1 &msg);

// push a reference of the message to the Lua stack
// the 3rd and 4th arguments define a callback that can be invoked just before Lua
// garbage collects the message. If the 3rd argument is NULL, Lua will *NOT* free
// memory. If the second argument points to a function, that function is called when
// Lua garbage collects the object. The function is sent a pointer to the message being
// collected and the 4th argument to this function. If the function returns true,
// Lua will free the memory. If false (0), Lua will not free the memory.
LUA_PROTOBUF_EXPORT bool lua_protobuf_zippylog_protocol_request_SubscribeEnvelopesV1_pushreference(lua_State *L, ::zippylog::protocol::request::SubscribeEnvelopesV1 *msg, lua_protobuf_gc_callback callback, void *data);


// The following functions are called by Lua. Many people will not need them,
// but they are exported for those that do.


// constructor called from Lua
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_SubscribeEnvelopesV1_new(lua_State *L);

// obtain instance from a serialized string
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_SubscribeEnvelopesV1_parsefromstring(lua_State *L);

// garbage collects message instance in Lua
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_SubscribeEnvelopesV1_gc(lua_State *L);

// obtain serialized representation of instance
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_SubscribeEnvelopesV1_serialized(lua_State *L);

// clear all fields in the message
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_SubscribeEnvelopesV1_clear(lua_State *L);

// repeated string path = 1
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_SubscribeEnvelopesV1_clear_path(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_SubscribeEnvelopesV1_get_path(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_SubscribeEnvelopesV1_set_path(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_SubscribeEnvelopesV1_size_path(lua_State *L);

// optional string lua_code = 5
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_SubscribeEnvelopesV1_clear_lua_code(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_SubscribeEnvelopesV1_get_lua_code(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_SubscribeEnvelopesV1_set_lua_code(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_SubscribeEnvelopesV1_has_lua_code(lua_State *L);

// repeated uint32 filter_namespace = 2
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_SubscribeEnvelopesV1_clear_filter_namespace(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_SubscribeEnvelopesV1_get_filter_namespace(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_SubscribeEnvelopesV1_set_filter_namespace(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_SubscribeEnvelopesV1_size_filter_namespace(lua_State *L);

// repeated uint32 filter_enumeration_namespace = 3
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_SubscribeEnvelopesV1_clear_filter_enumeration_namespace(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_SubscribeEnvelopesV1_get_filter_enumeration_namespace(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_SubscribeEnvelopesV1_set_filter_enumeration_namespace(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_SubscribeEnvelopesV1_size_filter_enumeration_namespace(lua_State *L);

// repeated uint32 filter_enumeration_type = 4
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_SubscribeEnvelopesV1_clear_filter_enumeration_type(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_SubscribeEnvelopesV1_get_filter_enumeration_type(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_SubscribeEnvelopesV1_set_filter_enumeration_type(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_SubscribeEnvelopesV1_size_filter_enumeration_type(lua_State *L);

// end of message SubscribeEnvelopesV1

// Message SubscribeKeepaliveV1
// registers the message type with Lua
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_SubscribeKeepaliveV1_open(lua_State *L);


// push a copy of the message to the Lua stack
// caller is free to use original message however she wants, but changes will not
// be reflected in Lua and vice-verse
LUA_PROTOBUF_EXPORT bool lua_protobuf_zippylog_protocol_request_SubscribeKeepaliveV1_pushcopy(lua_State *L, const ::zippylog::protocol::request::SubscribeKeepaliveV1 &msg);

// push a reference of the message to the Lua stack
// the 3rd and 4th arguments define a callback that can be invoked just before Lua
// garbage collects the message. If the 3rd argument is NULL, Lua will *NOT* free
// memory. If the second argument points to a function, that function is called when
// Lua garbage collects the object. The function is sent a pointer to the message being
// collected and the 4th argument to this function. If the function returns true,
// Lua will free the memory. If false (0), Lua will not free the memory.
LUA_PROTOBUF_EXPORT bool lua_protobuf_zippylog_protocol_request_SubscribeKeepaliveV1_pushreference(lua_State *L, ::zippylog::protocol::request::SubscribeKeepaliveV1 *msg, lua_protobuf_gc_callback callback, void *data);


// The following functions are called by Lua. Many people will not need them,
// but they are exported for those that do.


// constructor called from Lua
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_SubscribeKeepaliveV1_new(lua_State *L);

// obtain instance from a serialized string
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_SubscribeKeepaliveV1_parsefromstring(lua_State *L);

// garbage collects message instance in Lua
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_SubscribeKeepaliveV1_gc(lua_State *L);

// obtain serialized representation of instance
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_SubscribeKeepaliveV1_serialized(lua_State *L);

// clear all fields in the message
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_SubscribeKeepaliveV1_clear(lua_State *L);

// required bytes id = 1
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_SubscribeKeepaliveV1_clear_id(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_SubscribeKeepaliveV1_get_id(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_SubscribeKeepaliveV1_set_id(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_SubscribeKeepaliveV1_has_id(lua_State *L);

// end of message SubscribeKeepaliveV1

// Message SubscribeCancelV1
// registers the message type with Lua
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_SubscribeCancelV1_open(lua_State *L);


// push a copy of the message to the Lua stack
// caller is free to use original message however she wants, but changes will not
// be reflected in Lua and vice-verse
LUA_PROTOBUF_EXPORT bool lua_protobuf_zippylog_protocol_request_SubscribeCancelV1_pushcopy(lua_State *L, const ::zippylog::protocol::request::SubscribeCancelV1 &msg);

// push a reference of the message to the Lua stack
// the 3rd and 4th arguments define a callback that can be invoked just before Lua
// garbage collects the message. If the 3rd argument is NULL, Lua will *NOT* free
// memory. If the second argument points to a function, that function is called when
// Lua garbage collects the object. The function is sent a pointer to the message being
// collected and the 4th argument to this function. If the function returns true,
// Lua will free the memory. If false (0), Lua will not free the memory.
LUA_PROTOBUF_EXPORT bool lua_protobuf_zippylog_protocol_request_SubscribeCancelV1_pushreference(lua_State *L, ::zippylog::protocol::request::SubscribeCancelV1 *msg, lua_protobuf_gc_callback callback, void *data);


// The following functions are called by Lua. Many people will not need them,
// but they are exported for those that do.


// constructor called from Lua
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_SubscribeCancelV1_new(lua_State *L);

// obtain instance from a serialized string
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_SubscribeCancelV1_parsefromstring(lua_State *L);

// garbage collects message instance in Lua
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_SubscribeCancelV1_gc(lua_State *L);

// obtain serialized representation of instance
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_SubscribeCancelV1_serialized(lua_State *L);

// clear all fields in the message
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_SubscribeCancelV1_clear(lua_State *L);

// required bytes id = 1
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_SubscribeCancelV1_clear_id(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_SubscribeCancelV1_get_id(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_SubscribeCancelV1_set_id(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_request_SubscribeCancelV1_has_id(lua_State *L);

// end of message SubscribeCancelV1

#ifdef __cplusplus
}
#endif

#endif