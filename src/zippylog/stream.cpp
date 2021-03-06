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

#include <zippylog/stream.hpp>

#include <zippylog/platform.hpp>

using ::google::protobuf::io::CodedInputStream;
using ::google::protobuf::io::CodedOutputStream;
using ::google::protobuf::io::FileInputStream;
using ::google::protobuf::io::FileOutputStream;
using ::std::string;

namespace zippylog {

InputStream::InputStream() :
    cis(NULL),
    coded_input_stream_read_bytes(0),
    version(0),
    have_next_size(false),
    next_envelope_size(0),
    offset(-1)
{ }

InputStream::~InputStream() {
    if (this->cis) delete this->cis;
}

uint32 InputStream::NextEnvelopeSize()
{
    if (this->have_next_size) {
        return this->next_envelope_size;
    }

    if (!this->cis->ReadVarint32(&this->next_envelope_size)) {
        return 0;
    }

    this->have_next_size = true;

    return this->next_envelope_size;
}

bool InputStream::ReadVersion()
{
    // we read the first byte of the stream
    char v;
    if (!this->cis->ReadRaw(&v, sizeof(v))) {
        this->coded_input_stream_read_bytes++;
        return false;
    }

    this->version = v;
    this->offset = 1;

    if (v != 0x01) {
        return false;
    }

    return true;
}

bool InputStream::ReadEnvelope(::zippylog::Envelope &e, uint32 &bytes_read)
{
    uint32 size = this->NextEnvelopeSize();
    if (!size) {
        bytes_read = 0;
        return false;
    }

    // We rebuild the coded input stream if we have read more than half its
    // limit. This is more than generous, but recreation should be a problem
    // since we are only talking every ~1 billion bytes or so.
    if (this->coded_input_stream_read_bytes > (INT_MAX >> 1)) {
        this->RebuildCodedInputStream();
    }

    CodedInputStream::Limit limit = this->cis->PushLimit(size);
    if (!e.ParseFromCodedInputStream(*this->cis)) {
        this->cis->PopLimit(limit);
        this->have_next_size = false;
        bytes_read = 0;
        return false;
    }

    this->have_next_size = false;

    this->cis->PopLimit(limit);

    if (size <= 0x7f) {          // 127
        bytes_read = size + 1;
    }
    else if (size <= 0x3fff) {   // 16,383
        bytes_read = size + 2;
    }
    else if (size <= 0x1fffff) { // 2,097,151
        bytes_read = size + 3;
    }
    else {                       // 2,684,35,455 (well, actually a little less due to 32 bit limit
        bytes_read = size + 4;
    }

    this->offset += bytes_read;
    this->coded_input_stream_read_bytes += bytes_read;

    return true;
}

void InputStream::RebuildCodedInputStream()
{
    if (this->cis) {
        delete this->cis;
        this->cis = NULL;
    }

    CodedInputStream *s = this->ConstructCodedInputStream();
    assert(s);
    assert(!this->cis);

    s->SetTotalBytesLimit(INT_MAX, -1);
    this->cis = s;
    this->coded_input_stream_read_bytes = 0;
}


FileInputStream::FileInputStream(::std::string const &path,
                                 int64 start_offset) :
    InputStream(), fis(NULL)
{
    if (!this->file.Open(path, platform::File::READ | platform::File::BINARY)) {
        throw Exception("could not open file input stream");
    }

    this->fis = new ::google::protobuf::io::FileInputStream(this->file.FileDescriptor());
    this->RebuildCodedInputStream();

    if (!this->ReadVersion()) {
        throw Exception("could not read stream version or stream version not supported");
    }

    if (start_offset > 0) {
        if (!this->SetAbsoluteOffset(start_offset)) {
            throw Exception("could not set stream offset");
        }
    }
}

FileInputStream::~FileInputStream()
{
    // need to destroy coded stream to unassociate from file descriptor
    if (this->cis) {
        delete this->cis;
        this->cis = NULL;
    }

    if (this->fis) delete this->fis;

    this->file.Close();
}

bool FileInputStream::SetAbsoluteOffset(int64 offset)
{
    // if we change the underlying file descriptor, we need to rebuild the
    // protobuf pieces from the new offset in the descriptor b/c the
    // objects can buffer read data
    if (this->cis) {
        delete this->cis;
        this->cis = NULL;
    }

    if (this->fis) {
        delete this->fis;
        this->fis = NULL;
    }

    if (!this->file.Seek(offset)) {
        throw Exception("could not seek to requested stream offset");
    }

    this->fis = new ::google::protobuf::io::FileInputStream(this->file.FileDescriptor());
    this->cis = new CodedInputStream(this->fis);
    this->offset = offset;

    return true;
}

CodedInputStream * FileInputStream::ConstructCodedInputStream()
{
    return new CodedInputStream(this->fis);
}

OutputStream::OutputStream() : cos(NULL)
{ }

OutputStream::~OutputStream()
{
    if (this->cos) delete this->cos;
}

bool OutputStream::WriteEnvelope(::zippylog::Envelope &e)
{
    if (!this->cos) return false;

    int size = e.SerializedByteSize();

    this->cos->WriteVarint32(size);
    return e.Serialize(this->cos);
}

bool OutputStream::WriteEnvelope(const void *data, int length)
{
    if (!this->cos) return false;

    this->cos->WriteVarint32(length);
    this->cos->WriteRaw(data, length);

    return true;
}

bool OutputStream::WriteStreamHeader()
{
    if (!this->cos) return false;

    char version = 0x01;
    this->cos->WriteRaw(&version, sizeof(version));
    return this->Flush();
}

bool OutputStream::WriteData(const void *data, int length)
{
    if (!this->cos) return false;

    this->cos->WriteRaw(data, length);

    return true;
}

FileOutputStream::FileOutputStream(string const &path, bool write_lock) : OutputStream(), os(NULL)
{
    int flags = platform::File::CREATE | platform::File::APPEND | platform::File::WRITE | platform::File::BINARY;

    if (write_lock) flags |= platform::File::WRITE_LOCK;

    if (!this->file.Open(path, flags)) {
        throw Exception("could not open file");
    }

    platform::FileStat stat;
    if (!platform::stat(path, stat)) {
        throw Exception("could not stat file opened for writing. weird");
    }

    if (this->cos) delete this->cos;

    this->os = new ::google::protobuf::io::FileOutputStream(this->file.FileDescriptor());
    this->cos = new CodedOutputStream(this->os);

    // this is a new file, so we need to write out the version
    if (!stat.size) {
        if (!this->WriteStreamHeader()) {
            throw Exception("could not write stream header to new stream");
        }
    }
}

FileOutputStream::~FileOutputStream()
{
    // we must delete the coded output stream first, so it returns unused
    // buffer to output stream
    if (this->cos) {
        delete this->cos;

        // we set to NULL to prevent double free in parent dtor
        this->cos = NULL;
    }

    if (this->os) {
        // we close the output stream manually, which incurs a flush
        this->os->Close();

        delete this->os;
    }
}

bool FileOutputStream::Flush()
{
    // the coded output stream takes ownership of the buffer from the file
    // output stream, which screws with the later's Flush() API (it will
    // flush the whole buffer, even though only X bytes really contain data).
    // the solution is to nuke the coded output stream, which relinquishes
    // unused bytes back to file output stream. then, we flush the output
    // stream and create a new coded output stream
    // see http://code.google.com/p/protobuf/issues/detail?id=216

    if (this->cos) {
        delete this->cos;
    }

    bool result = this->os->Flush();
    this->cos = new CodedOutputStream(this->os);

    return this->file.Flush() && result;
}

} // namespace
