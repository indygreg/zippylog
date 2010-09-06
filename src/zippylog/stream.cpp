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

#include <fcntl.h>
#include <io.h>

#ifdef WIN32
#define lseek64 _lseeki64
#define open _open
#endif

namespace zippylog {

InputStream::InputStream()
{
    this->_fd = -1;
    this->_cis = NULL;
    this->_is = NULL;
}

InputStream::InputStream(string file, int64 seek_bytes)
{
    if (!this->OpenFile(file, seek_bytes)) {
        // TODO handle error
    }
}

InputStream::~InputStream() {
    delete _cis, _is;
    _close(this->_fd);
}

bool InputStream::OpenFile(string file, int64 start_offset)
{
    if (this->_fd >= 0) {
        _close(this->_fd);
        this->_fd = -1;
    }

    if (this->_is) delete this->_is;
    if (this->_cis) delete this->_cis;
    this->_have_next_size = false;
    this->_next_envelope_size = 0;

    this->_fd = open(file.c_str(), O_RDONLY | O_BINARY);
    if (this->_fd < 0) {
        GOOGLE_LOG(FATAL) << "could not open file for reading: " << file;
        return false;
    }

    if (start_offset > 0) {
        lseek64(this->_fd, start_offset, SEEK_SET);
    }

    this->_is = new FileInputStream(this->_fd, -1);
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
    lseek(this->_fd, offset, SEEK_SET);

    return true;
}


OutputStream::OutputStream()
{

}

OutputStream::~OutputStream()
{

}

bool OutputStream::WriteEnvelope(::zippylog::Envelope &e)
{
    return false;
}


} // namespace

#ifdef WIN32
#undef lseek64
#endif