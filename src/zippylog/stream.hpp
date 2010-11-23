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

#ifndef ZIPPYLOG_STREAM_HPP_
#define ZIPPYLOG_STREAM_HPP_

#include <zippylog/zippylog.hpp>
#include <zippylog/envelope.hpp>
#include <zippylog/platform.hpp>

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <string>

namespace zippylog {

using ::google::protobuf::io::FileInputStream;
using ::google::protobuf::io::FileOutputStream;
using ::google::protobuf::io::CodedInputStream;
using ::google::protobuf::io::CodedOutputStream;

// since streams are only ever appended to, it doesn't make sense to model a
// read/write stream, so we have a separate input and output stream

class ZIPPYLOG_EXPORT InputStream {
    public:
        InputStream();
        InputStream(::std::string file, int64 seek_bytes=0);
        ~InputStream();

        bool OpenFile(::std::string file, int64 start_offset = 0);

        // Size (in bytes) of the next envelope in the stream
        // does NOT include size of envelope size encoding
        uint32 NextEnvelopeSize();

        bool ReadEnvelope(::zippylog::Envelope &envelope, uint32 &bytes_read);
        bool Seek(int64 offset);

    private:
        // disable copy constructor and assignment operator
        InputStream(const InputStream &orig);
        InputStream & operator=(const InputStream &orig);

        platform::File file;

        FileInputStream *_is;
        CodedInputStream *_cis;
        bool _have_next_size;
        uint32 _next_envelope_size;

};

// A stream used for writing data
// Currently, OutputStream's are not thread safe. In fact, multiple instances
// pointing to the same file are not safe. The reason is buffering.
// Currently, the protocol buffers output streams buffer data before it is
// written to an underlying file descriptor. Flushes don't necessarily occur
// at boundaries we desire. If multiple output streams are writing to the same
// file, for example, a partial envelope could be flushed by one and another
// could write out a full envelope shortly thereafter, corrupting the stream.
//
// In a nutshell, only allow one OutputStream per output file globally
// TODO establish better sanity around output stream usage
class ZIPPYLOG_EXPORT OutputStream {
    public:
        // opens an output stream (always appends)
        OutputStream(const ::std::string file);
        ~OutputStream();

        // writes an envelope to the stream
        // NOT thread safe. if multiple threads call this, it may write a corrupt stream
        bool WriteEnvelope(::zippylog::Envelope &envelope);

        // writes an envelope from a buffer
        // unlike WriteData(), this will add the varint length to the stream,
        // as is necessary from the stream specification
        bool WriteEnvelope(const void *data, int length);

        // writes data with specified length to stream
        bool WriteData(const void *data, int length);

        // writes data inside a string to stream
        bool WriteString(const ::std::string &s);

        bool Flush();

private:
    platform::File file;
    FileOutputStream *os;
    CodedOutputStream *cos;
};

} // namespace

#endif