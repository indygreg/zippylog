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

using ::std::string;

namespace zippylog {

InputStream::InputStream() :
    _cis(NULL),
    _is(NULL)
{ }

InputStream::InputStream(const InputStream &orig)
{
    throw "copy constructor not available for InputStream";
}

InputStream::InputStream(string file, int64 seek_bytes) :
    _cis(NULL),
    _is(NULL)
{
    if (!this->OpenFile(file, seek_bytes)) {
        throw "could not open stream for reading";
    }
}

InputStream & InputStream::operator=(const InputStream &orig)
{
    throw "assignment operator not available for InputStream";
}

InputStream::~InputStream() {
    delete this->_cis;
    delete this->_is;

    this->file.Close();
}

bool InputStream::OpenFile(string file, int64 start_offset)
{
    this->file.Close();

    if (this->_cis) delete this->_cis;
    if (this->_is) delete this->_is;
    this->_have_next_size = false;
    this->_next_envelope_size = 0;

    if (!this->file.Open(file, platform::File::READ | platform::File::BINARY)) {
        return false;
    }

    if (start_offset > 0) {
        if (!this->file.Seek(start_offset)) return false;
    }

    this->_is = new FileInputStream(this->file.FileDescriptor());
    this->_cis = new CodedInputStream(this->_is);

    if (start_offset == 0) {
        char v;
        if (!_cis->ReadRaw(&v, sizeof(v))) {
            GOOGLE_LOG(FATAL) << "could not read version byte from stream";
            return false;
        }
        else if (v != 1) {
            GOOGLE_LOG(FATAL) << "stream is not version 1";
            return false;
        }
    }

    return true;
}

uint32 InputStream::NextEnvelopeSize()
{
    if (this->_have_next_size) {
        return this->_next_envelope_size;
    }

    if (!this->_cis->ReadVarint32(&this->_next_envelope_size)) {
        return 0;
    }

    this->_have_next_size = true;

    return this->_next_envelope_size;
}

bool InputStream::ReadEnvelope(::zippylog::Envelope &e, uint32 &bytes_read)
{
    uint32 size = this->NextEnvelopeSize();
    if (!size) {
        bytes_read = 0;
        return false;
    }

    CodedInputStream::Limit limit = _cis->PushLimit(size);
    if (!e.envelope.ParseFromCodedStream(_cis) || !_cis->ConsumedEntireMessage()) {
        this->_have_next_size = false;
        bytes_read = 0;
        return false;
    }

    this->_have_next_size = false;

    _cis->PopLimit(limit);

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

bool InputStream::Seek(int64 offset)
{
    if (!this->file.Seek(offset)) return false;

    delete this->_cis;
    delete this->_is;

    this->_is = new FileInputStream(this->file.FileDescriptor());
    this->_cis = new CodedInputStream(this->_is);

    return true;
}



OutputStream::OutputStream(const string file)
{
    if (!this->file.Open(file, platform::File::CREATE | platform::File::APPEND | platform::File::WRITE)) {
        throw "could not open file";
    }

    platform::FileStat stat;
    if (!platform::stat(file, stat)) {
        throw "could not stat file opened for writing. weird";
    }

    // this is a new file, so we need to write out the version
    if (!stat.size) {
        char version = 0x01;
        if (!this->file.Write(&version, 1)) {
            throw "could not write version byte to stream";
        }
    }

    this->os = new FileOutputStream(this->file.FileDescriptor());
    this->cos = new CodedOutputStream(this->os);
}

OutputStream::~OutputStream()
{
    // we must delete the coded output stream first, so it returns unused
    // buffer to output stream
    delete this->cos;

    // we close the output stream manually, which incurs a flush
    this->os->Close();

    delete this->os;
}

bool OutputStream::WriteEnvelope(::zippylog::Envelope &e)
{
    int size = e.envelope.ByteSize();

    this->cos->WriteVarint32(size);
    return e.envelope.SerializeToCodedStream(this->cos);
}

bool OutputStream::WriteEnvelope(const void *data, int length)
{
    this->cos->WriteVarint32(length);
    this->cos->WriteRaw(data, length);

    return true;
}

bool OutputStream::WriteData(const void *data, int length)
{
    this->cos->WriteRaw(data, length);

    return true;
}

bool OutputStream::WriteString(const string &s)
{
    this->cos->WriteRaw(s.data(), s.size());

    return true;
}

bool OutputStream::Flush()
{
    // the coded output stream takes ownership of the buffer from the file
    // output stream, which screws with the later's Flush() API (it will
    // flush the whole buffer, even though only X bytes really contain data).
    // the solution is to nuke the coded output stream, which relinquishes
    // unused bytes back to file output stream. then, we flush the output
    // stream and create a new coded output stream
    // see http://code.google.com/p/protobuf/issues/detail?id=216

    delete this->cos;
    bool result = this->os->Flush();
    this->cos = new CodedOutputStream(this->os);

    // TODO consider using unbuffered I/O for the created file instead
    // of forcing a file descriptor flush
    return this->file.Flush() && result;
}

} // namespace
