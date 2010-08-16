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

#ifndef PBLOG_STREAM_HPP_
#define PBLOG_STREAM_HPP_

#include <pblog/pblog.h>

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>

#include <pblog/message.pb.h>

namespace pblog {

using namespace ::google::protobuf;
using namespace ::google::protobuf::io;
using namespace message;

class PBLOG_EXPORT InputStream {
    public:
        InputStream(const char *file, int64 seek_bytes=0);
        ~InputStream();

        bool ReadEnvelope(Envelope *);

    private:
        int _fd;
        FileInputStream *_is;
        CodedInputStream *_cis;
};

} // namespace

#endif