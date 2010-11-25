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

#include "zippylog/stream.hpp"

#include <zippylog/platform.hpp>

using ::google::protobuf::io::CodedInputStream;
using ::google::protobuf::io::CodedOutputStream;
using ::google::protobuf::io::FileInputStream;
using ::google::protobuf::io::FileOutputStream;
using ::std::string;

namespace zippylog {

InputStream::InputStream() :
    cis(NULL),
    version(0),
    have_next_size(false),
    next_envelope_size(0)
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
        return false;
    }

    this->version = v;

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

    CodedInputStream::Limit limit = this->cis->PushLimit(size);
    if (!e.envelope.ParseFromCodedStream(this->cis) || !this->cis->ConsumedEntireMessage()) {
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

    return true;
}


FileInputStream::FileInputStream(const string &path, int64 offset) :
    fis(NULL)
{
    if (!this->file.Open(path, platform::File::READ | platform::File::BINARY)) {
        throw "could not open file input stream";
    }

    this->fis = new ::google::protobuf::io::FileInputStream(this->file.FileDescriptor());
    this->cis = new CodedInputStream(this->fis);

    if (!this->ReadVersion()) {
        throw "could not read stream version or stream version not supported";
    }

    if (offset > 0) {
        if (!this->SetAbsoluteOffset(offset)) {
            throw "could not set stream offset";
        }
    }
}

FileInputStream::~FileInputStream()
{
    if (this->fis) delete this->fis;

    this->file.Close();
}

bool FileInputStream::SetAbsoluteOffset(int64 offset)
{
    if (!this->file.Seek(offset)) {
        throw "could not seek to requested stream offset";
    }

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

    this->fis = new ::google::protobuf::io::FileInputStream(this->file.FileDescriptor());
    this->cis = new CodedInputStream(this->fis);

    return true;
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

    int size = e.envelope.ByteSize();

    this->cos->WriteVarint32(size);
    return e.envelope.SerializeToCodedStream(this->cos);
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

FileOutputStream::FileOutputStream(const string &path, bool write_lock) : os(NULL), OutputStream()
{
    int flags = platform::File::CREATE | platform::File::APPEND | platform::File::WRITE | platform::File::BINARY;

    if (write_lock) flags |= platform::File::WRITE_LOCK;

    if (!this->file.Open(path, flags)) {
        throw "could not open file";
    }

    platform::FileStat stat;
    if (!platform::stat(path, stat)) {
        throw "could not stat file opened for writing. weird";
    }

    if (this->cos) delete this->cos;

    this->os = new ::google::protobuf::io::FileOutputStream(this->file.FileDescriptor());
    this->cos = new CodedOutputStream(this->os);

    // this is a new file, so we need to write out the version
    if (!stat.size) {
        if (!this->WriteStreamHeader()) {
            throw "could not write stream header to new stream";
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
