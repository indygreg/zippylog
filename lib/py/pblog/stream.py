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

# TODO probably don't want to use an internal interface
from google.protobuf.internal.decoder import _DecodeVarint
from google.protobuf.internal.encoder import _VarintEncoder

from pblog.exception import PBException
from pblog.message import Message

class Stream:
    def __init__(self, fh, version=1, is_empty=False, read_mode=None):
        if version != 1:
            raise PBException('only version 1 streams are supported')

        self.fh = fh
        self.is_empty = is_empty

        if read_mode == 'beginning':
            version = ord(self.fh.read(1))
            if version != 1:
                raise PBException('stream version %d not supported' % version)

        self.varint_encoder = _VarintEncoder()
        self.varint_decoder = _DecodeVarint

    def write_messages(self, *messages):
        '''Writes populated pblog.message.Message instances to the stream.

        Typically called by another module within pblog so messages can be
        "prepared" before being written.'''

        if self.is_empty:
            self.fh.write(chr(0x01))
            self.is_empty = False

        for m in messages:
            if not isinstance(m, Message):
                raise PBException('passed message not a pblog.message.Message: %s' % type(m))

            encoded = m.serialize()
            l = len(encoded)

            self.varint_encoder(self.fh.write, l)
            self.fh.write(encoded)

    def flush(self):
        '''Flush the underlying stream.'''
        return self.fh.flush()

    def read_message(self):
        buf = self.fh.read(4)

        if not len(buf):
            return None

        (size, pos) = self.varint_decoder(buf, 0)

        buf = buf[pos:] + self.fh.read(size - 4 + pos)

        return Message(serialized=buf)
