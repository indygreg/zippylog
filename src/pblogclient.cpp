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

#include <pblog/client.hpp>

#include <google/protobuf/text_format.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <zmq.hpp>
#include <iostream>

using namespace pblog;
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

    while (true) {
        printf("%d\n", i++);
        protocol::StoreInfo *info = c.store_info();

        for (size_t i = 0; i < info->bucket_size(); i++) {
            protocol::BucketInfo binfo = info->bucket(i);
            string bucket = binfo.path();

            for (size_t j = 0; j < binfo.stream_set_size(); j++) {
                protocol::StreamSetInfo ssinfo = binfo.stream_set(j);
                string stream_set = ssinfo.path();

                for (size_t k = 0; k < ssinfo.stream_size(); k++) {
                    protocol::StreamInfo sinfo = ssinfo.stream(k);
                    string stream = sinfo.path();

                    cout << bucket << '/' << stream_set << '/' << stream << endl;

                    c.get_stream(bucket, stream_set, stream);

                    while (true) {
                        ::pblog::Envelope e;
                        if (c.read_envelope(e)) {
                            printer.Print(e.envelope, &os);
                            os.Flush();
                        }
                    }
                }
            }
        }
    }

    return 0;
}
