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

#include <zippylog/zippylogd/util.hpp>

#include <list>

namespace zippylog {
namespace zippylogd {

using ::std::string;
using ::std::vector;
using ::zippylog::lua::LineProcessorState;

Zippylogd::Zippylogd(ZippylogdStartParams &params) :
    run_piped(params.mode_piped),
    run_server(params.mode_server),
    server_config_file(params.server_config_file),
    pipeL(),
    piped_lua_file(params.piped_lua_file),
    piped_lua_max_size(params.piped_lua_max_size),
    piped_lua_have_line_processor(false)
{
    if (!this->run_piped && !this->run_server) {
        throw "must specify a mode for zippylogd to run in (piped or server)";
    }

    // TODO more validation

    if (this->run_piped) {
        // if they've provided a Lua file for us to use
        if (this->piped_lua_file.size() > 0) {
            // first we increase the memory ceiling
            if (!this->pipeL.SetMemoryCeiling(1024 * this->piped_lua_max_size)) {
                throw "error creating Lua interpreter for piped reading";
            }

            // next we load the libraries, as they will be needed by loaded code
            if (!this->pipeL.LoadStringLibrary()) {
                throw "could not load string library into piped Lua interpreter";
            }

            // now we try to load their use code
            string error;
            if (!this->pipeL.LoadFile(this->piped_lua_file, error)) {
                throw "error loading Lua file: " + error;
            }

            // grab the capabilities
            this->piped_lua_have_line_processor = this->pipeL.HasLineProcessor();
        }
    }
}

Zippylogd::~Zippylogd()
{

}

bool Zippylogd::Run()
{
    if (this->run_piped) {
        this->RunPipeListener();
    }

    return true;
}

bool Zippylogd::RunPipeListener()
{
    // we currently only support STDIN
    this->inpipe = &::std::cin;

    LineProcessorState line_state;

    // TODO need more work on logic
    while (*this->inpipe) {
        string line;
        getline(*this->inpipe, line);

        if (this->piped_lua_have_line_processor) {
            line_state.string_in = line;

            if (!this->pipeL.ProcessLine(line_state)) {
                throw "error processing line in Lua";
            }

            ::std::cout << line_state.string_out << ::std::endl;
        }
    }

    return true;
}

// convenience function to get an argument value and pop the current argument and the
// value from the list
//
// returns true if we found a value. false can be returned in the following scenarios:
//   - bad iterator
//   - no more strings after iterator
//   - next string begins with "-" or "--"
//
// in all these cases, no arguments are popped
bool GetArgumentValueAndPop(vector<string> &args, size_t &index, string &value)
{
    if (args.size() - index < 2) return false;

    if (args[index+1].at(0) == '-' || args[index+1].substr(0, 1) == "--") return false;

    value = args[++index];

    args.erase(args.begin() + index - 1);
    args.erase(args.begin() + index - 1);

    index--;

    return true;
}

bool ParseCommandArguments(vector<string> &args, ZippylogdStartParams &params, string &error)
{
    if (args.size() < 2) {
        error = "missing required argument --server or --piped";
        return false;
    }

    // get rid of first element, the program name
    args.erase(args.begin());

    // first, we ascertain the mode
    for (size_t i = 0; i < args.size(); i++) {
        if (args[i] == "--piped") {
            if (params.mode_piped) {
                error = "--piped specified multiple times";
                return false;
            }

            params.mode_piped = true;
            args.erase(args.begin() + i);
            break;
        }
        else if (args[i] == "--server") {
            if (params.mode_server) {
                error = "--server specified multiple times";
                return false;
            }

            string value;
            if (!GetArgumentValueAndPop(args, i, value)) {
                error = "--server requires an argument to a Lua config file";
                return false;
            }

            params.server_config_file = value;
        }
    }

    if (!params.mode_piped && !params.mode_server) {
        error = "--server or --piped not defined in arguments";
        return false;
    }

    // now look for options for --piped
    for (size_t i = 0; i < args.size(); i++) {
        if (args[i] == "--piped-lua-file") {
            string value;
            if (!GetArgumentValueAndPop(args, i, value)) {
                error = "--piped-lua-file requires an argument";
                return false;
            }
            params.piped_lua_file = value;
            continue;
        }
    }

    return true;
}

}} // namespaces