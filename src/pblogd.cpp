//  Copyright 2010 Gregory Szorc
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

#pragma once

#include <pblog/pblog.h>
#include <pblog/broker.hpp>
#include <pblog/store.hpp>

#include <lua.h>
#include <lauxlib.h>

#include <iostream>
#include <string>

using ::pblog::Store;
using ::pblog::server::Broker;
using ::std::cout;
using ::std::endl;
using ::std::string;

int main(int argc, const char * const argv[])
{
    if (argc != 2) {
        cout << "Usage: pblogd /path/to/config/file.lua" << endl;
        return 1;
    }

    lua_State *L = luaL_newstate();
    if (luaL_dofile(L, argv[1])) {
        cout << "Error running config file" << endl;
        cout << lua_tostring(L, lua_gettop(L)) << endl;
        return 1;
    }

    lua_getglobal(L, "store_path");
    if (!lua_isstring(L, -1)) {
        cout << "store_path variable not defined or not a string" << endl;
        return 1;
    }
    string store_path = lua_tostring(L, -1);

    Store store = Store(store_path);
    lua_pop(L, 1);

    Broker broker = Broker(&store);

    lua_getglobal(L, "endpoints");
    if (!lua_istable(L, -1)) {
        cout << "endpoints variable not defined or not a table" << endl;
        return 1;
    }
    lua_pushnil(L);
    while (lua_next(L, 1) != 0) {
        if (!lua_isstring(L, -1)) {
            if (lua_isstring(L, -2)) {
                cout << "endpoint value at index '" << lua_tostring(L, -2) << "' is not a string" << endl;
                return 1;
            }
            else {
                cout << "non-string value seen in endpoints table. index not printable" << endl;
                return 1;
            }
        }
        // else
        const char *endpoint = lua_tostring(L, -1);
        broker.add_listen_endpoint(endpoint);
        lua_pop(L, 1);
    }

    broker.run();

    return 0;
}
