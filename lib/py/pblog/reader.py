#  Copyright 2010 Gregory Szorc
#
#  Licensed under the Apache License, Version 2.0 (the "License");
#  you may not use this file except in compliance with the License.
#  You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License.

from google.protobuf.internal.decoder import _DecodeVarint
from pblog.message import Message

class IReader():
    '''Interface for all pblog readers'''

    def __init__(self):
        '''IReader()

        Default constructor.'''

        self.varint_decoder = _DecodeVarint

    def read(self):
        '''read()

        Read and return a single message.'''

        raise Exception('method must be defined in derived classes')

class FileObjectReader(IReader):
    '''A simple writer that reads from an opened file object handle'''

    def __init__(self, handle, has_read=False, stream_version=None):
        '''FileObjectReader(fh)

        Initialize a new reader instance that associated with a File Object'''
        IReader.__init__(self)
        self.handle = handle
        
        # if we haven't read yet, do that now to verify we can understand the
        # stream
        if not has_read:
            version = ord(self.handle.read(1))

            if version != 1:
                raise Exception('stream version %d not supported' % version)

            self.stream_version = version

        elif has_read and not stream_version:
            raise Exception('stream version must be defined if has_read is True')
        elif stream_version != 1:
            raise Exception('stream version %d not supported' % stream_version)
        else:
            self.stream_version = stream_version

    def read(self):
        '''read()

        Read and return the next message on the stream.'''

        # the Google varint decoder works on a buffer. So, we grab some data
        # and let it loose. We later grab more data as we need it
        # TODO '4' will surely lead to a bug somewhere
        buf = self.handle.read(4)

        (size, pos) = self.varint_decoder(buf, 0)
        buf = buf[pos:] + self.handle.read(size)

        e = Message(serialized=buf)
        return e
