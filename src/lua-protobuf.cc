

#include "lua-protobuf.h"

#ifdef __cplusplus
extern "C" {
#endif

#include <lauxlib.h>

#ifdef __cplusplus
}
#endif

int lua_protobuf_enum_index(lua_State *L)
{
    return luaL_error(L, "attempting to access undefined enumeration value: %s", lua_tostring(L, 2));
}

int lua_protobuf_enum_newindex(lua_State *L)
{
    return luaL_error(L, "cannot modify enumeration tables");
}

int lua_protobuf_gc_always_free(::google::protobuf::Message *, void *)
{
    return 1;
}

