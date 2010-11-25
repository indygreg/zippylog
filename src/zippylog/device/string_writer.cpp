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

#include <zippylog/device/string_writer.hpp>
#include <zippylog/envelope.hpp>

namespace zippylog {
namespace device {

using ::std::string;
using ::std::vector;

StringWriter::StringWriter(StringWriterStartParams &params) :
    ostream(params.output_stream),
    store(NULL),
    default_bucket(params.default_bucket),
    default_set(params.default_stream_set),
    receiver(params.receiver)
{
    if (!this->receiver) {
        throw "no string receiver defined";
    }

    if (params.store_path.length() > 0) {
        this->store = Store::CreateStore(params.store_path);
    }

}

StringWriter::~StringWriter()
{
    if (this->store) delete this->store;
}

void StringWriter::Run(bool *active)
{
    if (!active) {
        throw "argument to StringWriter::Run cannot be NULL";
    }

    while (*active) {
        StringReceiverResult result;
        this->receiver->ReceiveLine(result);

        // if input is done, stop
        if (result.result == StringReceiverResult::NO_INPUT) break;

        // if Lua says to ignore, don't do anything
        if (result.result == StringReceiverResult::LUA_FALSE) continue;

        // we have a string!
        if (result.result == StringReceiverResult::OK_STRING) {
            // if we have an output stream registered, send it there
            if (this->ostream) {
                vector<string>::iterator i = result.output_strings.begin();
                for (; i != result.output_strings.end(); i++) {
                    *this->ostream << *i << ::std::endl;
                }
            }

            // if we have a store registered, convert to an envelope and
            // write it there
            if (this->store) {
                string bucket = result.bucket.length() > 0 ? result.bucket : this->default_bucket;
                string set = result.stream_set.length() > 0 ? result.stream_set : this->default_set;

                vector<string>::iterator i = result.output_strings.begin();
                for (; i != result.output_strings.end(); i++) {
                    Envelope e(*i);

                    this->store->WriteEnvelope(bucket, set, e);
                }
            }
        }
    }

    return;
}

}} // namespaces