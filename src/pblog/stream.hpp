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
#include <pblog/envelope.hpp>

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <string>

namespace pblog {

using ::google::protobuf::int64;
using ::google::protobuf::io::FileInputStream;
using ::google::protobuf::io::CodedInputStream;
using ::std::string;

class PBLOG_EXPORT InputStream {
    public:
        InputStream();
        InputStream(string file, int64 seek_bytes=0);
        ~InputStream();

        bool OpenFile(string file, int64 start_offset = 0);
        bool ReadEnvelope(::pblog::Envelope &envelope);
        bool Seek(int64 offset);

    private:
        int _fd;
        FileInputStream *_is;
        CodedInputStream *_cis;
};

class PBLOG_EXPORT OutputStream {
    public:
        OutputStream();
        ~OutputStream();

        bool WriteEnvelope(::pblog::Envelope &envelope);
};

} // namespace

#endif