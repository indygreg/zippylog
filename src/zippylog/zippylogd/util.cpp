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
#include <zippylog/util.hpp>

#include <list>

namespace zippylog {
namespace zippylogd {

using ::std::string;
using ::std::vector;
using ::zippylog::util::GetArgumentValueAndPop;

Zippylogd::Zippylogd(ZippylogdStartParams &params) :
    run_server(params.mode_server),
    server_config_file(params.server_config_file),
    writer(NULL),
    ctx(params.zctx),
    own_context(false),
    store_path(params.store_path)
{
    // TODO more validation

    if (!this->ctx) {
        this->ctx = new ::zmq::context_t(2);
        this->own_context = true;
    }

    if (this->store_path.length() > 0) {
        ::zippylog::device::StoreWriterStartParams swparams;
        swparams.store_path = this->store_path;
        swparams.envelope_pull_endpoint = params.store_writer_envelope_pull_endpoint;
        swparams.ctx = this->ctx;

        this->writer = new ::zippylog::device::StoreWriter(swparams);
    }
}

Zippylogd::~Zippylogd()
{
    if (this->writer) delete this->writer;
    if (this->ctx && this->own_context) delete this->ctx;
}

bool Zippylogd::Run()
{

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

    // now look for global options
    for (size_t i = 0; i < args.size(); i++) {
        if (args[i] == "--store") {
            string value;
            if (!GetArgumentValueAndPop(args, i, value)) {
                error = "--store requires an argument";
                return false;
            }
            params.store_path = value;
            continue;
        }
    }

    return true;
}

}} // namespaces