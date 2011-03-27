

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

int lua_protobuf_ismessage(lua_State *L, int index)
{
    void *ud = lua_touserdata(L, index);
    if (ud == NULL) return 0;
    if (luaL_getmetafield(L, index, "_isprotobuf") == 0) return 0;

    // if the field exists, we take that. no sense checking the value
    return 1;
}

::google::protobuf::Message * lua_protobuf_tomessage(lua_State *L, int index)
{
    void *ud = lua_touserdata(L, index);
    if (ud == NULL) return NULL;
    if (luaL_getmetafield(L, index, "_isprotobuf") == 0) return NULL;

    return ((lua_protobuf_udata_t *)ud)->msg;
}

