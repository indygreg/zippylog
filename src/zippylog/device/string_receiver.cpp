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

#include <zippylog/device/string_receiver.hpp>

namespace zippylog {
namespace device {

using ::std::logic_error;
using ::std::string;
using ::std::vector;
using ::zippylog::lua::LineProcessorState;

StringReceiver::StringReceiver(StringReceiverStartParams &params) :
    inpipe(params.stream),
    L(),
    lua_file(params.lua_file),
    lua_max_size(params.lua_max_size),
    have_lua_line_processor(false)
{
    // if we have Lua, create an interpreter
    if (this->lua_file.size() > 0) {
        // first we increase the memory ceiling
        if (!this->L.SetMemoryCeiling(1024 * this->lua_max_size)) {
            throw Exception("error setting max memory ceiling on Lua interpreter");
        }

        // load libraries
        // we always enable string, since it is safe and convenient
        if (!this->L.LoadStringLibrary()) {
            throw Exception("could not load string library into Lua interpreter");
        }

        // try to load their code
        string error;
        if (!this->L.LoadFile(this->lua_file, error)) {
            throw Exception("error loading Lua file: " + error);
        }

        // define Lua capabilities
        this->have_lua_line_processor = this->L.HasLineProcessor();
    }
}

void StringReceiver::ReceiveLine(StringReceiverResult &result)
{
    if (!this->inpipe) {
        throw Exception("cannot receive line since no input pipe is defined on StringReceiver");
    }

    // istream is closed
    if (!*this->inpipe) {
        result.result = StringReceiverResult::NO_INPUT;
        return;
    }

    // TODO we should poll the istream to see if data is available
    // otherwise, we could wait a long, long time
    string line;
    getline(*this->inpipe, line);

    this->ProcessLine(line, result);
    return;
}

void StringReceiver::ProcessLine(const string &line, StringReceiverResult &result)
{
    LineProcessorState line_state;

    result.input_string = line;

    if (this->have_lua_line_processor) {
        line_state.string_in = line;

        if (!this->L.ProcessLine(line_state)) {
            result.result = StringReceiverResult::LUA_ERROR;
            return;
        }

        if (line_state.result == LineProcessorState::NO) {
            result.result = StringReceiverResult::LUA_FALSE;
            return;
        }

        if (line_state.bucket.length() > 0) {
            result.bucket = line_state.bucket;
        }
        if (line_state.stream_set.length() > 0) {
            result.stream_set = line_state.stream_set;
        }

        result.output_strings.push_back(line_state.string_out);

        if (line_state.result == LineProcessorState::STRING_MODIFIED) {
            result.result = StringReceiverResult::OK_STRING;
            return;
        }
        else if (line_state.result == LineProcessorState::YES) {
            result.result = StringReceiverResult::OK_STRING;
            return;
        }
        else if (line_state.result == LineProcessorState::NOTHING) {
            result.result = StringReceiverResult::OK_STRING;
            return;
        }

        throw logic_error("unhandled return code from ProcessLine()");
    }

    // else we don't have a Lua processor, so we treat string as-is
    result.output_strings.push_back(line);
    result.result = StringReceiverResult::OK_STRING;
    return;
}

}} // namespaces