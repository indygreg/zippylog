// Generated by the lua-protobuf compiler.
// You shouldn't be editing this file manually
//
// source proto file: zippylog/protocol/response.proto

#ifndef LUA_PROTOBUF_zippylog_protocol_response_H
#define LUA_PROTOBUF_zippylog_protocol_response_H

#include "lua-protobuf.h"
#include <zippylog/protocol/response.pb.h>

#ifdef __cplusplus
extern "C" {
#endif

#include <lua.h>

// register all messages in this package to a Lua state
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_open(lua_State *L);

// Message PongV1
// registers the message type with Lua
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_PongV1_open(lua_State *L);


// push a copy of the message to the Lua stack
// caller is free to use original message however she wants, but changes will not
// be reflected in Lua and vice-verse
LUA_PROTOBUF_EXPORT bool lua_protobuf_zippylog_protocol_response_PongV1_pushcopy(lua_State *L, const ::zippylog::protocol::response::PongV1 &msg);

// push a reference of the message to the Lua stack
// the 3rd and 4th arguments define a callback that can be invoked just before Lua
// garbage collects the message. If the 3rd argument is NULL, Lua will *NOT* free
// memory. If the second argument points to a function, that function is called when
// Lua garbage collects the object. The function is sent a pointer to the message being
// collected and the 4th argument to this function. If the function returns true,
// Lua will free the memory. If false (0), Lua will not free the memory.
LUA_PROTOBUF_EXPORT bool lua_protobuf_zippylog_protocol_response_PongV1_pushreference(lua_State *L, ::zippylog::protocol::response::PongV1 *msg, lua_protobuf_gc_callback callback, void *data);


// The following functions are called by Lua. Many people will not need them,
// but they are exported for those that do.


// constructor called from Lua
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_PongV1_new(lua_State *L);

// obtain instance from a serialized string
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_PongV1_parsefromstring(lua_State *L);

// garbage collects message instance in Lua
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_PongV1_gc(lua_State *L);

// obtain serialized representation of instance
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_PongV1_serialized(lua_State *L);

// clear all fields in the message
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_PongV1_clear(lua_State *L);

// end of message PongV1

// Message FeatureSpecificationV1
// registers the message type with Lua
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_FeatureSpecificationV1_open(lua_State *L);


// push a copy of the message to the Lua stack
// caller is free to use original message however she wants, but changes will not
// be reflected in Lua and vice-verse
LUA_PROTOBUF_EXPORT bool lua_protobuf_zippylog_protocol_response_FeatureSpecificationV1_pushcopy(lua_State *L, const ::zippylog::protocol::response::FeatureSpecificationV1 &msg);

// push a reference of the message to the Lua stack
// the 3rd and 4th arguments define a callback that can be invoked just before Lua
// garbage collects the message. If the 3rd argument is NULL, Lua will *NOT* free
// memory. If the second argument points to a function, that function is called when
// Lua garbage collects the object. The function is sent a pointer to the message being
// collected and the 4th argument to this function. If the function returns true,
// Lua will free the memory. If false (0), Lua will not free the memory.
LUA_PROTOBUF_EXPORT bool lua_protobuf_zippylog_protocol_response_FeatureSpecificationV1_pushreference(lua_State *L, ::zippylog::protocol::response::FeatureSpecificationV1 *msg, lua_protobuf_gc_callback callback, void *data);


// The following functions are called by Lua. Many people will not need them,
// but they are exported for those that do.


// constructor called from Lua
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_FeatureSpecificationV1_new(lua_State *L);

// obtain instance from a serialized string
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_FeatureSpecificationV1_parsefromstring(lua_State *L);

// garbage collects message instance in Lua
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_FeatureSpecificationV1_gc(lua_State *L);

// obtain serialized representation of instance
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_FeatureSpecificationV1_serialized(lua_State *L);

// clear all fields in the message
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_FeatureSpecificationV1_clear(lua_State *L);

// repeated uint32 supported_message_formats = 1
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_FeatureSpecificationV1_clear_supported_message_formats(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_FeatureSpecificationV1_get_supported_message_formats(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_FeatureSpecificationV1_set_supported_message_formats(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_FeatureSpecificationV1_size_supported_message_formats(lua_State *L);

// repeated uint32 supported_request_types = 2
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_FeatureSpecificationV1_clear_supported_request_types(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_FeatureSpecificationV1_get_supported_request_types(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_FeatureSpecificationV1_set_supported_request_types(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_FeatureSpecificationV1_size_supported_request_types(lua_State *L);

// repeated string supported_request_names = 3
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_FeatureSpecificationV1_clear_supported_request_names(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_FeatureSpecificationV1_get_supported_request_names(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_FeatureSpecificationV1_set_supported_request_names(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_FeatureSpecificationV1_size_supported_request_names(lua_State *L);

// optional uint32 max_stream_segment_bytes = 4
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_FeatureSpecificationV1_clear_max_stream_segment_bytes(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_FeatureSpecificationV1_get_max_stream_segment_bytes(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_FeatureSpecificationV1_set_max_stream_segment_bytes(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_FeatureSpecificationV1_has_max_stream_segment_bytes(lua_State *L);

// optional uint32 max_stream_segment_envelopes = 5
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_FeatureSpecificationV1_clear_max_stream_segment_envelopes(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_FeatureSpecificationV1_get_max_stream_segment_envelopes(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_FeatureSpecificationV1_set_max_stream_segment_envelopes(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_FeatureSpecificationV1_has_max_stream_segment_envelopes(lua_State *L);

// end of message FeatureSpecificationV1

// Message StreamSegmentStartV1
// registers the message type with Lua
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_StreamSegmentStartV1_open(lua_State *L);


// push a copy of the message to the Lua stack
// caller is free to use original message however she wants, but changes will not
// be reflected in Lua and vice-verse
LUA_PROTOBUF_EXPORT bool lua_protobuf_zippylog_protocol_response_StreamSegmentStartV1_pushcopy(lua_State *L, const ::zippylog::protocol::response::StreamSegmentStartV1 &msg);

// push a reference of the message to the Lua stack
// the 3rd and 4th arguments define a callback that can be invoked just before Lua
// garbage collects the message. If the 3rd argument is NULL, Lua will *NOT* free
// memory. If the second argument points to a function, that function is called when
// Lua garbage collects the object. The function is sent a pointer to the message being
// collected and the 4th argument to this function. If the function returns true,
// Lua will free the memory. If false (0), Lua will not free the memory.
LUA_PROTOBUF_EXPORT bool lua_protobuf_zippylog_protocol_response_StreamSegmentStartV1_pushreference(lua_State *L, ::zippylog::protocol::response::StreamSegmentStartV1 *msg, lua_protobuf_gc_callback callback, void *data);


// The following functions are called by Lua. Many people will not need them,
// but they are exported for those that do.


// constructor called from Lua
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_StreamSegmentStartV1_new(lua_State *L);

// obtain instance from a serialized string
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_StreamSegmentStartV1_parsefromstring(lua_State *L);

// garbage collects message instance in Lua
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_StreamSegmentStartV1_gc(lua_State *L);

// obtain serialized representation of instance
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_StreamSegmentStartV1_serialized(lua_State *L);

// clear all fields in the message
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_StreamSegmentStartV1_clear(lua_State *L);

// required string path = 1
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_StreamSegmentStartV1_clear_path(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_StreamSegmentStartV1_get_path(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_StreamSegmentStartV1_set_path(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_StreamSegmentStartV1_has_path(lua_State *L);

// required uint64 offset = 2
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_StreamSegmentStartV1_clear_offset(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_StreamSegmentStartV1_get_offset(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_StreamSegmentStartV1_set_offset(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_StreamSegmentStartV1_has_offset(lua_State *L);

// end of message StreamSegmentStartV1

// Message StreamSegmentEndV1
// registers the message type with Lua
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_StreamSegmentEndV1_open(lua_State *L);


// push a copy of the message to the Lua stack
// caller is free to use original message however she wants, but changes will not
// be reflected in Lua and vice-verse
LUA_PROTOBUF_EXPORT bool lua_protobuf_zippylog_protocol_response_StreamSegmentEndV1_pushcopy(lua_State *L, const ::zippylog::protocol::response::StreamSegmentEndV1 &msg);

// push a reference of the message to the Lua stack
// the 3rd and 4th arguments define a callback that can be invoked just before Lua
// garbage collects the message. If the 3rd argument is NULL, Lua will *NOT* free
// memory. If the second argument points to a function, that function is called when
// Lua garbage collects the object. The function is sent a pointer to the message being
// collected and the 4th argument to this function. If the function returns true,
// Lua will free the memory. If false (0), Lua will not free the memory.
LUA_PROTOBUF_EXPORT bool lua_protobuf_zippylog_protocol_response_StreamSegmentEndV1_pushreference(lua_State *L, ::zippylog::protocol::response::StreamSegmentEndV1 *msg, lua_protobuf_gc_callback callback, void *data);


// The following functions are called by Lua. Many people will not need them,
// but they are exported for those that do.


// constructor called from Lua
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_StreamSegmentEndV1_new(lua_State *L);

// obtain instance from a serialized string
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_StreamSegmentEndV1_parsefromstring(lua_State *L);

// garbage collects message instance in Lua
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_StreamSegmentEndV1_gc(lua_State *L);

// obtain serialized representation of instance
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_StreamSegmentEndV1_serialized(lua_State *L);

// clear all fields in the message
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_StreamSegmentEndV1_clear(lua_State *L);

// required uint64 offset = 1
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_StreamSegmentEndV1_clear_offset(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_StreamSegmentEndV1_get_offset(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_StreamSegmentEndV1_set_offset(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_StreamSegmentEndV1_has_offset(lua_State *L);

// required uint32 bytes_sent = 2
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_StreamSegmentEndV1_clear_bytes_sent(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_StreamSegmentEndV1_get_bytes_sent(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_StreamSegmentEndV1_set_bytes_sent(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_StreamSegmentEndV1_has_bytes_sent(lua_State *L);

// required uint32 envelopes_sent = 3
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_StreamSegmentEndV1_clear_envelopes_sent(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_StreamSegmentEndV1_get_envelopes_sent(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_StreamSegmentEndV1_set_envelopes_sent(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_StreamSegmentEndV1_has_envelopes_sent(lua_State *L);

// end of message StreamSegmentEndV1

// Message SubscriptionAcceptAckV1
// registers the message type with Lua
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_SubscriptionAcceptAckV1_open(lua_State *L);


// push a copy of the message to the Lua stack
// caller is free to use original message however she wants, but changes will not
// be reflected in Lua and vice-verse
LUA_PROTOBUF_EXPORT bool lua_protobuf_zippylog_protocol_response_SubscriptionAcceptAckV1_pushcopy(lua_State *L, const ::zippylog::protocol::response::SubscriptionAcceptAckV1 &msg);

// push a reference of the message to the Lua stack
// the 3rd and 4th arguments define a callback that can be invoked just before Lua
// garbage collects the message. If the 3rd argument is NULL, Lua will *NOT* free
// memory. If the second argument points to a function, that function is called when
// Lua garbage collects the object. The function is sent a pointer to the message being
// collected and the 4th argument to this function. If the function returns true,
// Lua will free the memory. If false (0), Lua will not free the memory.
LUA_PROTOBUF_EXPORT bool lua_protobuf_zippylog_protocol_response_SubscriptionAcceptAckV1_pushreference(lua_State *L, ::zippylog::protocol::response::SubscriptionAcceptAckV1 *msg, lua_protobuf_gc_callback callback, void *data);


// The following functions are called by Lua. Many people will not need them,
// but they are exported for those that do.


// constructor called from Lua
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_SubscriptionAcceptAckV1_new(lua_State *L);

// obtain instance from a serialized string
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_SubscriptionAcceptAckV1_parsefromstring(lua_State *L);

// garbage collects message instance in Lua
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_SubscriptionAcceptAckV1_gc(lua_State *L);

// obtain serialized representation of instance
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_SubscriptionAcceptAckV1_serialized(lua_State *L);

// clear all fields in the message
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_SubscriptionAcceptAckV1_clear(lua_State *L);

// required bytes id = 1
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_SubscriptionAcceptAckV1_clear_id(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_SubscriptionAcceptAckV1_get_id(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_SubscriptionAcceptAckV1_set_id(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_SubscriptionAcceptAckV1_has_id(lua_State *L);

// required uint32 ttl = 2
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_SubscriptionAcceptAckV1_clear_ttl(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_SubscriptionAcceptAckV1_get_ttl(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_SubscriptionAcceptAckV1_set_ttl(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_SubscriptionAcceptAckV1_has_ttl(lua_State *L);

// end of message SubscriptionAcceptAckV1

// Message SubscriptionKeepaliveAckV1
// registers the message type with Lua
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_SubscriptionKeepaliveAckV1_open(lua_State *L);


// push a copy of the message to the Lua stack
// caller is free to use original message however she wants, but changes will not
// be reflected in Lua and vice-verse
LUA_PROTOBUF_EXPORT bool lua_protobuf_zippylog_protocol_response_SubscriptionKeepaliveAckV1_pushcopy(lua_State *L, const ::zippylog::protocol::response::SubscriptionKeepaliveAckV1 &msg);

// push a reference of the message to the Lua stack
// the 3rd and 4th arguments define a callback that can be invoked just before Lua
// garbage collects the message. If the 3rd argument is NULL, Lua will *NOT* free
// memory. If the second argument points to a function, that function is called when
// Lua garbage collects the object. The function is sent a pointer to the message being
// collected and the 4th argument to this function. If the function returns true,
// Lua will free the memory. If false (0), Lua will not free the memory.
LUA_PROTOBUF_EXPORT bool lua_protobuf_zippylog_protocol_response_SubscriptionKeepaliveAckV1_pushreference(lua_State *L, ::zippylog::protocol::response::SubscriptionKeepaliveAckV1 *msg, lua_protobuf_gc_callback callback, void *data);


// The following functions are called by Lua. Many people will not need them,
// but they are exported for those that do.


// constructor called from Lua
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_SubscriptionKeepaliveAckV1_new(lua_State *L);

// obtain instance from a serialized string
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_SubscriptionKeepaliveAckV1_parsefromstring(lua_State *L);

// garbage collects message instance in Lua
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_SubscriptionKeepaliveAckV1_gc(lua_State *L);

// obtain serialized representation of instance
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_SubscriptionKeepaliveAckV1_serialized(lua_State *L);

// clear all fields in the message
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_SubscriptionKeepaliveAckV1_clear(lua_State *L);

// required bytes id = 1
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_SubscriptionKeepaliveAckV1_clear_id(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_SubscriptionKeepaliveAckV1_get_id(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_SubscriptionKeepaliveAckV1_set_id(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_SubscriptionKeepaliveAckV1_has_id(lua_State *L);

// end of message SubscriptionKeepaliveAckV1

// Message SubscriptionStartV1
// registers the message type with Lua
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_SubscriptionStartV1_open(lua_State *L);


// push a copy of the message to the Lua stack
// caller is free to use original message however she wants, but changes will not
// be reflected in Lua and vice-verse
LUA_PROTOBUF_EXPORT bool lua_protobuf_zippylog_protocol_response_SubscriptionStartV1_pushcopy(lua_State *L, const ::zippylog::protocol::response::SubscriptionStartV1 &msg);

// push a reference of the message to the Lua stack
// the 3rd and 4th arguments define a callback that can be invoked just before Lua
// garbage collects the message. If the 3rd argument is NULL, Lua will *NOT* free
// memory. If the second argument points to a function, that function is called when
// Lua garbage collects the object. The function is sent a pointer to the message being
// collected and the 4th argument to this function. If the function returns true,
// Lua will free the memory. If false (0), Lua will not free the memory.
LUA_PROTOBUF_EXPORT bool lua_protobuf_zippylog_protocol_response_SubscriptionStartV1_pushreference(lua_State *L, ::zippylog::protocol::response::SubscriptionStartV1 *msg, lua_protobuf_gc_callback callback, void *data);


// The following functions are called by Lua. Many people will not need them,
// but they are exported for those that do.


// constructor called from Lua
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_SubscriptionStartV1_new(lua_State *L);

// obtain instance from a serialized string
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_SubscriptionStartV1_parsefromstring(lua_State *L);

// garbage collects message instance in Lua
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_SubscriptionStartV1_gc(lua_State *L);

// obtain serialized representation of instance
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_SubscriptionStartV1_serialized(lua_State *L);

// clear all fields in the message
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_SubscriptionStartV1_clear(lua_State *L);

// required bytes id = 1
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_SubscriptionStartV1_clear_id(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_SubscriptionStartV1_get_id(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_SubscriptionStartV1_set_id(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_SubscriptionStartV1_has_id(lua_State *L);

// end of message SubscriptionStartV1

// Message WriteAckV1
// registers the message type with Lua
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_WriteAckV1_open(lua_State *L);


// push a copy of the message to the Lua stack
// caller is free to use original message however she wants, but changes will not
// be reflected in Lua and vice-verse
LUA_PROTOBUF_EXPORT bool lua_protobuf_zippylog_protocol_response_WriteAckV1_pushcopy(lua_State *L, const ::zippylog::protocol::response::WriteAckV1 &msg);

// push a reference of the message to the Lua stack
// the 3rd and 4th arguments define a callback that can be invoked just before Lua
// garbage collects the message. If the 3rd argument is NULL, Lua will *NOT* free
// memory. If the second argument points to a function, that function is called when
// Lua garbage collects the object. The function is sent a pointer to the message being
// collected and the 4th argument to this function. If the function returns true,
// Lua will free the memory. If false (0), Lua will not free the memory.
LUA_PROTOBUF_EXPORT bool lua_protobuf_zippylog_protocol_response_WriteAckV1_pushreference(lua_State *L, ::zippylog::protocol::response::WriteAckV1 *msg, lua_protobuf_gc_callback callback, void *data);


// The following functions are called by Lua. Many people will not need them,
// but they are exported for those that do.


// constructor called from Lua
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_WriteAckV1_new(lua_State *L);

// obtain instance from a serialized string
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_WriteAckV1_parsefromstring(lua_State *L);

// garbage collects message instance in Lua
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_WriteAckV1_gc(lua_State *L);

// obtain serialized representation of instance
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_WriteAckV1_serialized(lua_State *L);

// clear all fields in the message
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_WriteAckV1_clear(lua_State *L);

// required uint32 envelopes_written = 1
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_WriteAckV1_clear_envelopes_written(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_WriteAckV1_get_envelopes_written(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_WriteAckV1_set_envelopes_written(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_WriteAckV1_has_envelopes_written(lua_State *L);

// end of message WriteAckV1

// Message PluginRegisterAckV1
// registers the message type with Lua
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_PluginRegisterAckV1_open(lua_State *L);


// push a copy of the message to the Lua stack
// caller is free to use original message however she wants, but changes will not
// be reflected in Lua and vice-verse
LUA_PROTOBUF_EXPORT bool lua_protobuf_zippylog_protocol_response_PluginRegisterAckV1_pushcopy(lua_State *L, const ::zippylog::protocol::response::PluginRegisterAckV1 &msg);

// push a reference of the message to the Lua stack
// the 3rd and 4th arguments define a callback that can be invoked just before Lua
// garbage collects the message. If the 3rd argument is NULL, Lua will *NOT* free
// memory. If the second argument points to a function, that function is called when
// Lua garbage collects the object. The function is sent a pointer to the message being
// collected and the 4th argument to this function. If the function returns true,
// Lua will free the memory. If false (0), Lua will not free the memory.
LUA_PROTOBUF_EXPORT bool lua_protobuf_zippylog_protocol_response_PluginRegisterAckV1_pushreference(lua_State *L, ::zippylog::protocol::response::PluginRegisterAckV1 *msg, lua_protobuf_gc_callback callback, void *data);


// The following functions are called by Lua. Many people will not need them,
// but they are exported for those that do.


// constructor called from Lua
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_PluginRegisterAckV1_new(lua_State *L);

// obtain instance from a serialized string
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_PluginRegisterAckV1_parsefromstring(lua_State *L);

// garbage collects message instance in Lua
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_PluginRegisterAckV1_gc(lua_State *L);

// obtain serialized representation of instance
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_PluginRegisterAckV1_serialized(lua_State *L);

// clear all fields in the message
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_PluginRegisterAckV1_clear(lua_State *L);

// required string name = 1
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_PluginRegisterAckV1_clear_name(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_PluginRegisterAckV1_get_name(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_PluginRegisterAckV1_set_name(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_PluginRegisterAckV1_has_name(lua_State *L);

// end of message PluginRegisterAckV1

// Message PluginUnregisterAckV1
// registers the message type with Lua
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_PluginUnregisterAckV1_open(lua_State *L);


// push a copy of the message to the Lua stack
// caller is free to use original message however she wants, but changes will not
// be reflected in Lua and vice-verse
LUA_PROTOBUF_EXPORT bool lua_protobuf_zippylog_protocol_response_PluginUnregisterAckV1_pushcopy(lua_State *L, const ::zippylog::protocol::response::PluginUnregisterAckV1 &msg);

// push a reference of the message to the Lua stack
// the 3rd and 4th arguments define a callback that can be invoked just before Lua
// garbage collects the message. If the 3rd argument is NULL, Lua will *NOT* free
// memory. If the second argument points to a function, that function is called when
// Lua garbage collects the object. The function is sent a pointer to the message being
// collected and the 4th argument to this function. If the function returns true,
// Lua will free the memory. If false (0), Lua will not free the memory.
LUA_PROTOBUF_EXPORT bool lua_protobuf_zippylog_protocol_response_PluginUnregisterAckV1_pushreference(lua_State *L, ::zippylog::protocol::response::PluginUnregisterAckV1 *msg, lua_protobuf_gc_callback callback, void *data);


// The following functions are called by Lua. Many people will not need them,
// but they are exported for those that do.


// constructor called from Lua
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_PluginUnregisterAckV1_new(lua_State *L);

// obtain instance from a serialized string
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_PluginUnregisterAckV1_parsefromstring(lua_State *L);

// garbage collects message instance in Lua
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_PluginUnregisterAckV1_gc(lua_State *L);

// obtain serialized representation of instance
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_PluginUnregisterAckV1_serialized(lua_State *L);

// clear all fields in the message
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_PluginUnregisterAckV1_clear(lua_State *L);

// required string name = 1
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_PluginUnregisterAckV1_clear_name(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_PluginUnregisterAckV1_get_name(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_PluginUnregisterAckV1_set_name(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_PluginUnregisterAckV1_has_name(lua_State *L);

// end of message PluginUnregisterAckV1

// Message PluginStatusV1
// registers the message type with Lua
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_PluginStatusV1_open(lua_State *L);


// push a copy of the message to the Lua stack
// caller is free to use original message however she wants, but changes will not
// be reflected in Lua and vice-verse
LUA_PROTOBUF_EXPORT bool lua_protobuf_zippylog_protocol_response_PluginStatusV1_pushcopy(lua_State *L, const ::zippylog::protocol::response::PluginStatusV1 &msg);

// push a reference of the message to the Lua stack
// the 3rd and 4th arguments define a callback that can be invoked just before Lua
// garbage collects the message. If the 3rd argument is NULL, Lua will *NOT* free
// memory. If the second argument points to a function, that function is called when
// Lua garbage collects the object. The function is sent a pointer to the message being
// collected and the 4th argument to this function. If the function returns true,
// Lua will free the memory. If false (0), Lua will not free the memory.
LUA_PROTOBUF_EXPORT bool lua_protobuf_zippylog_protocol_response_PluginStatusV1_pushreference(lua_State *L, ::zippylog::protocol::response::PluginStatusV1 *msg, lua_protobuf_gc_callback callback, void *data);


// The following functions are called by Lua. Many people will not need them,
// but they are exported for those that do.


// constructor called from Lua
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_PluginStatusV1_new(lua_State *L);

// obtain instance from a serialized string
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_PluginStatusV1_parsefromstring(lua_State *L);

// garbage collects message instance in Lua
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_PluginStatusV1_gc(lua_State *L);

// obtain serialized representation of instance
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_PluginStatusV1_serialized(lua_State *L);

// clear all fields in the message
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_PluginStatusV1_clear(lua_State *L);

// repeated message state = 1
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_PluginStatusV1_clear_state(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_PluginStatusV1_get_state(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_PluginStatusV1_set_state(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_PluginStatusV1_size_state(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_PluginStatusV1_add_state(lua_State *L);

// end of message PluginStatusV1

// Message Error
// registers the message type with Lua
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_Error_open(lua_State *L);


// push a copy of the message to the Lua stack
// caller is free to use original message however she wants, but changes will not
// be reflected in Lua and vice-verse
LUA_PROTOBUF_EXPORT bool lua_protobuf_zippylog_protocol_response_Error_pushcopy(lua_State *L, const ::zippylog::protocol::response::Error &msg);

// push a reference of the message to the Lua stack
// the 3rd and 4th arguments define a callback that can be invoked just before Lua
// garbage collects the message. If the 3rd argument is NULL, Lua will *NOT* free
// memory. If the second argument points to a function, that function is called when
// Lua garbage collects the object. The function is sent a pointer to the message being
// collected and the 4th argument to this function. If the function returns true,
// Lua will free the memory. If false (0), Lua will not free the memory.
LUA_PROTOBUF_EXPORT bool lua_protobuf_zippylog_protocol_response_Error_pushreference(lua_State *L, ::zippylog::protocol::response::Error *msg, lua_protobuf_gc_callback callback, void *data);


// The following functions are called by Lua. Many people will not need them,
// but they are exported for those that do.


// constructor called from Lua
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_Error_new(lua_State *L);

// obtain instance from a serialized string
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_Error_parsefromstring(lua_State *L);

// garbage collects message instance in Lua
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_Error_gc(lua_State *L);

// obtain serialized representation of instance
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_Error_serialized(lua_State *L);

// clear all fields in the message
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_Error_clear(lua_State *L);

// optional enum code = 1
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_Error_clear_code(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_Error_get_code(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_Error_set_code(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_Error_has_code(lua_State *L);

// optional string msg = 2
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_Error_clear_msg(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_Error_get_msg(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_Error_set_msg(lua_State *L);
LUA_PROTOBUF_EXPORT int lua_protobuf_zippylog_protocol_response_Error_has_msg(lua_State *L);

// end of message Error

#ifdef __cplusplus
}
#endif

#endif