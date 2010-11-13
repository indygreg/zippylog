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

#ifndef ZIPPYLOG_DEVICE_PIPER_HPP_
#define ZIPPYLOG_DEVICE_PIPER_HPP_

#include <zippylog/zippylog.hpp>
#include <zippylog/lua.hpp>

#include <iostream>
#include <string>
#include <vector>

namespace zippylog {
namespace device {

class ZIPPYLOG_EXPORT PiperStartParams {
public:
    PiperStartParams() :
        lua_max_size(1024)
    { }

    // file that defines Lua code to be loaded
    ::std::string lua_file;

    // if running Lua code, the max memory usage the Lua interpreter is allowed
    // to grow to
    uint32 lua_max_size;

    // mask that says where to send output when in piped mode
    enum PipedDestination {
        // send to stdout
        STDOUT   = 0x00000001,

        // send to a configured store
        STORE    = 0x00000002,

        // send to a file opened from a path
        FILEPATH = 0x00000004,
    };

    // if outputting to a store, this is the path to the store
    ::std::string store_root_path;

    // if outputting to a store, the path within the store. e.g. "/bucket/set"
    ::std::string store_store_path;

    // if outputting to a file path, the path to open
    ::std::string output_path;

};

/// A piper instance
///
/// A piper receives data from an input pipe and processes it
class ZIPPYLOG_EXPORT Piper {
public:
    Piper(PiperStartParams &params);
    ~Piper();

    /// Runs the piper
    ///
    /// Function will block until ...
    bool Run();

protected:
    ::std::istream *inpipe;

    // Lua interpreter for pipe processing
    ::zippylog::lua::LuaState L;
    ::std::string lua_file;
    uint32 lua_max_size;

    bool have_lua_line_processor;

private:
    Piper(const Piper &orig);
    Piper & operator=(const Piper &orig);
};

}} // namespaces

#endif