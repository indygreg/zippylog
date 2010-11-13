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
    piper(NULL),
    writer(NULL),
    ctx(params.zctx),
    own_context(false)
{
    if (!this->run_piped && !this->run_server) {
        throw "must specify a mode for zippylogd to run in (piped or server)";
    }

    // TODO more validation

    if (!this->ctx) {
        this->ctx = new ::zmq::context_t(2);
        this->own_context = true;
    }

    if (this->run_piped) {
        ::zippylog::device::PiperStartParams pparams;
        pparams.lua_file = params.piped_lua_file;
        pparams.lua_max_size = params.piped_lua_max_size;
        pparams.output_path = params.piped_output_path;
        pparams.zctx = this->ctx;

        //pparams.store_path = params.piped_store_root_path;
        //pparams.store_default_path = params.piped_store_store_path;

        this->piper = new ::zippylog::device::Piper(pparams);
    }
}

Zippylogd::~Zippylogd()
{
    if (this->piper) delete this->piper;
    if (this->writer) delete this->writer;
    if (this->ctx && this->own_context) delete this->ctx;
}

bool Zippylogd::Run()
{
    if (this->run_piped) {
        this->RunPiper();
    }

    return true;
}

bool Zippylogd::RunPiper()
{
    if (!this->piper) return false;

    return this->piper->Run();
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