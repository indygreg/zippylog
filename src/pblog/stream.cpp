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

#include "pblog/stream.hpp"

#include <fcntl.h>
#include <io.h>

#ifdef WIN32
#define lseek64 _lseeki64
#define open _open
#endif

using namespace ::google::protobuf;
using namespace ::google::protobuf::io;

namespace pblog {

InputStream::InputStream(const char *file, int64 seek_bytes)
{
    this->_fd = open(file, O_RDONLY | O_BINARY);
    if (this->_fd < 0) {
        GOOGLE_LOG(FATAL) << "could not open file for reading: " << file;
    }

    if (seek_bytes > 0) {
        lseek64(this->_fd, seek_bytes, SEEK_SET);
    }

    this->_is = new FileInputStream(this->_fd, -1);
    this->_cis = new CodedInputStream(this->_is);

    if (seek_bytes == 0) {
        char v;
        if (!_cis->ReadRaw(&v, sizeof(v))) {
            GOOGLE_LOG(FATAL) << "could not read version byte from stream";
        }
        else if (v != 1) {
            GOOGLE_LOG(FATAL) << "stream is not version 1";
        }
    }
}

InputStream::~InputStream() {
    delete _cis, _is;
    _close(this->_fd);
}

bool InputStream::ReadEnvelope(Envelope *e)
{
    uint32 size;
    if (!_cis->ReadVarint32(&size)) return false;
    CodedInputStream::Limit limit = _cis->PushLimit(size);
    if (!e->ParseFromCodedStream(_cis)) return false;
    if (!_cis->ConsumedEntireMessage()) return false;
    _cis->PopLimit(limit);

    return true;
}

} // namespace

#ifdef WIN32
#undef lseek64
#endif