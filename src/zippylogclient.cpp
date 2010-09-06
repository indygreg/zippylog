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

#include <zippylog/client.hpp>

#include <google/protobuf/text_format.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <zmq.hpp>
#include <iostream>

using namespace zippylog;
using ::zmq::context_t;
using namespace ::std;

int main(int argc, const char * const argv[])
{
    context_t zctx(1);
    //client::Client c(&zctx, "tcp://192.168.1.69:52483");
    client::Client c(&zctx, "tcp://localhost:5000");

    int i = 0;

    ::google::protobuf::TextFormat::Printer printer = ::google::protobuf::TextFormat::Printer();
    ::google::protobuf::io::FileOutputStream os(1, -1);

    return 0;
}
