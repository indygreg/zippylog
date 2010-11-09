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

#ifndef ZIPPYLOG_ZIPPYLOGD_UTIL_HPP_
#define ZIPPYLOG_ZIPPYLOGD_UTIL_HPP_

#include <zippylog/zippylog.hpp>
#include <zippylog/lua.hpp>

#include <iostream>
#include <string>
#include <vector>

namespace zippylog {
namespace zippylogd {

class ZIPPYLOG_EXPORT ZippylogdStartParams {
public:
    ZippylogdStartParams() :
        mode_piped(false),
        mode_server(false),
        piped_lua_max_size(1024)
    { }

    bool mode_piped;
    bool mode_server;
    ::std::string server_config_file;
    ::std::string piped_lua_file;
    uint32 piped_lua_max_size;

};

/// An instance of zippylogd
///
/// zippylogd is a rather complex beast. it can have a number of modes of
/// operation. It might be best to read up on ZippylogdStartParams to see
/// how you can launch zippylogd instances.
class ZIPPYLOG_EXPORT Zippylogd {
public:
    /// Construct a new zippylogd instance from parameters
    ///
    /// Constructor just sets up zippylogd for execution. It doesn't
    /// actually start processing anything.
    Zippylogd(ZippylogdStartParams &params);

    ~Zippylogd();

    /// Runs the zippylogd instance
    ///
    /// Function will block until ...
    bool Run();

protected:
    bool run_piped;
    bool run_server;
    ::std::string server_config_file;
    ::std::istream *inpipe;

    // Lua interpreter for pipe processing
    ::zippylog::lua::LuaState pipeL;
    ::std::string piped_lua_file;
    uint32 piped_lua_max_size;

    bool piped_lua_have_line_processor;

    bool RunPipeListener();
private:
    Zippylogd(const Zippylogd &orig);
    Zippylogd & operator=(const Zippylogd &orig);
};

/// Parses zippylogd command arguments
///
/// The parsed parameters are stored in the passed StartParams class. If an
/// error was encountered, the function returns false and the passed string
/// contains an error message suitable for display to the invoker.
///
/// The passed list is likely modified during execution.
ZIPPYLOG_EXPORT bool ParseCommandArguments(::std::vector<::std::string> &args, ZippylogdStartParams &params, ::std::string & error);

}} // namespaces

#endif