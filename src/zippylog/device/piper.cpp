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

#include <zippylog/device/piper.hpp>
#include <zippylog/envelope.hpp>

namespace zippylog {
namespace device {

using ::std::string;
using ::std::vector;
using ::zippylog::lua::LineProcessorState;

Piper::Piper(PiperStartParams &params) :
    inpipe(NULL),
    L(),
    lua_file(params.lua_file),
    lua_max_size(params.lua_max_size),
    have_lua_line_processor(false),
    default_bucket(params.default_bucket),
    default_set(params.default_stream_set),
    output_path(params.output_path),
    store_writer_envelope_pull_endpoint(params.store_writer_envelope_pull_endpoint),
    ctx(params.zctx),
    own_context(false),
    store_sender(NULL),
    write_stdout(params.write_targets & params.STDOUT),
    write_store(params.write_targets & params.STORE),
    write_file(params.write_targets & params.FILEPATH)
{
    if (params.write_targets == PiperStartParams::NONE) {
        throw "write_targets must have at least 1 flag set";
    }

    if (this->write_file && this->output_path.length() < 1) {
        throw "file write mode enabled but no output path defined";
    }

    if (this->write_store && this->store_writer_envelope_pull_endpoint.length() < 1) {
        throw "store writing enabled but no envelope pull endpoint defined";
    }

    // if we have Lua, create an interpreter
    if (this->lua_file.size() > 0) {
        // first we increase the memory ceiling
        if (!this->L.SetMemoryCeiling(1024 * this->lua_max_size)) {
            throw "error setting max memory ceiling on Lua interpreter";
        }

        // load libraries
        // we always enable string, since it is safe and convenient
        if (!this->L.LoadStringLibrary()) {
            throw "could not load string library into Lua interpreter";
        }

        // try to load their code
        string error;
        if (!this->L.LoadFile(this->lua_file, error)) {
            throw "error loading Lua file: " + error;
        }

        // define Lua capabilities
        this->have_lua_line_processor = this->L.HasLineProcessor();
    }

    if (!this->ctx) {
        this->ctx = new ::zmq::context_t(1);
        this->own_context = true;
    }

    if (this->store_writer_envelope_pull_endpoint.length() > 0) {
        ::zippylog::device::StoreWriterSenderStartParams swparams;
        swparams.ctx = this->ctx;
        swparams.envelope_pull_endpoint = this->store_writer_envelope_pull_endpoint;

        this->store_sender = new ::zippylog::device::StoreWriterSender(swparams);
    }
}

Piper::~Piper()
{
    if (this->store_sender) delete this->store_sender;
    if (this->own_context && this->ctx) delete this->ctx;
}

bool Piper::Run()
{
    // we currently only support STDIN
    this->inpipe = &::std::cin;

    LineProcessorState line_state;

    // TODO need more work on logic
    while (*this->inpipe) {
        string line;
        getline(*this->inpipe, line);

        if (this->have_lua_line_processor) {
            line_state.string_in = line;

            if (!this->L.ProcessLine(line_state)) {
                throw "error processing line in Lua";
            }

            // if told not to do anything, don't
            if (line_state.result == line_state.NO) {
                continue;
            }

            if (this->write_stdout) {
                ::std::cout << line_state.string_out << ::std::endl;
            }

            if (this->write_store) {
                ::zippylog::Envelope e(line_state.string_out);
            }


        }
    }

    return true;
}

}} // namespaces