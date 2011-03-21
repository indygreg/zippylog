//  Copyright 2011 Gregory Szorc
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

#include <zippylog/device/string_loader.hpp>

namespace zippylog {
namespace device {

using ::std::invalid_argument;
using ::std::logic_error;
using ::std::string;
using ::std::vector;
using ::zippylog::lua::LoadStringResult;

StringLoader::StringLoader(StringLoaderStartParams &params) :
    instream(params.input_stream),
    outstream(params.output_stream),
    L(),
    have_lua_string_loader(false),
    default_bucket(params.default_bucket),
    default_set(params.default_set),
    store(NULL),
    active(params.active)
{
    if (!this->L.SetMemoryCeiling(1024 * params.lua_max_size)) {
        throw Exception("error setting max memory ceiling on Lua interpreter");
    }

    // we always enable the string library, since it is safe and convenient
    if (!this->L.LoadStringLibrary()) {
        throw Exception("could not load string library into Lua interpreter");
    }

    if (params.lua_file.length() > 0) {
        string error;
        if (!this->L.LoadFile(params.lua_file, error)) {
            throw Exception("error loading Lua file: " + error);
        }
    }

    if (params.lua_code.length() > 0) {
        string error;
        if (!this->L.LoadLuaCode(params.lua_code)) {
            throw Exception("error loading Lua code");
        }
    }

    this->have_lua_string_loader = this->L.HasLoadString();

    bool have_write_output = false;

    if (params.store_path.length() > 0) {
        have_write_output = true;
        this->store = Store::CreateStore(params.store_path);
    }

    if (have_write_output) {
        if (!this->default_bucket.length()) {
            throw invalid_argument("default_bucket not defined");
        }

        if (!this->default_set.length()) {
            throw invalid_argument("default_set not defined");
        }
    }
}

StringLoader::~StringLoader()
{
    if (this->store != NULL) delete this->store;
}

void StringLoader::ReadLineAndProcess(StringLoaderProcessingResult &result)
{
    if (!this->instream) {
        throw Exception("cannot receive line since no input stream is defined");
    }

    // istream is closed
    if (!*this->instream) {
        result.success = false;
        result.error = "input stream is closed";
        return;
    }

    // TODO we should poll the istream to see if data is available
    // otherwise, we could wait a long, long time
    string line;
    getline(*this->instream, line);

    this->ProcessString(line, result);
    return;
}

void StringLoader::ProcessString(string const& s, StringLoaderProcessingResult &result)
{
    result.success = false;

    LoadStringResult lsresult;

    if (this->have_lua_string_loader) {
        if (!this->L.ExecuteLoadString(s, lsresult)) {
            result.error = "Lua load string function did not execute";
            return;
        }

        if (!lsresult.execution_success) {
            result.error = "Lua error: " + lsresult.lua_error;
            return;
        }

        switch (lsresult.return_type) {
            case LoadStringResult::NIL:
            case LoadStringResult::BOOLTRUE:
                result.envelopes.push_back(new Envelope(s));
                if (lsresult.has_bucket) {
                    result.has_bucket = true;
                    result.bucket = lsresult.bucket;
                }
                if (lsresult.has_set) {
                    result.has_set = true;
                    result.set = lsresult.set;
                }
                result.success = true;
                break;

            case LoadStringResult::BOOLFALSE:
                result.success = true;
                break;

            case LoadStringResult::STRING:
                for (size_t i = 0; i < lsresult.strings.size(); i++) {
                    result.envelopes.push_back(new Envelope(lsresult.strings[i]));
                }

                if (lsresult.has_bucket) {
                    result.has_bucket = true;
                    result.bucket = lsresult.bucket;
                }

                if (lsresult.has_set) {
                    result.has_set = true;
                    result.set = lsresult.set;
                }

                result.success = true;

                break;

            case LoadStringResult::INVALID:
                result.error = "invalid signature from Lua function";
                break;

            case LoadStringResult::MESSAGE:
            case LoadStringResult::ENVELOPE:
                throw logic_error("message and envelope returns from Lua not currently implemented");
                break;

            default:
                throw logic_error("unhandled return type from LuaState.ExecuteLoadString()");
        }
    }
    // without a Lua processor, we just convert the input string to an
    // envelope
    else {
        result.envelopes.push_back(new Envelope(s));
        result.success = true;
    }

    return;
}

void StringLoader::SendResult(StringLoaderProcessingResult const& result)
{
    if (!result.success) {
        throw invalid_argument("cannot send unsuccessful results");
    }

    string bucket = result.has_bucket ? result.bucket : this->default_bucket;
    string set = result.has_set ? result.set : this->default_set;

    if (this->outstream) {
        for (size_t i = 0; i < result.envelopes.size(); i++) {
            *this->outstream << result.envelopes[i]->ToString() << ::std::endl;
        }
    }

    if (this->store) {
        for (size_t i = 0; i < result.envelopes.size(); i++) {
            this->store->WriteEnvelope(bucket, set, *(result.envelopes[i]));
        }
    }
}

void StringLoader::Run()
{
    if (!this->active) {
        throw new Exception("StringLoader cannot be executed because no active semaphore defined");
    }

    while (!this->active) {
        StringLoaderProcessingResult result;
        this->ReadLineAndProcess(result);
        if (result.success) {
            this->SendResult(result);
        }
    }
}

}} // namespaces