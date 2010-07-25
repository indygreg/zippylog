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

from pblog.event import LogEvent as EventWrapper
from pblog.pblog_pb2 import LogEvent, WriterInfo
import struct

class IReader():
    '''Interface for all pblog readers'''

    def __init__(self):
        '''IReader()

Default constructor.'''

        # TODO use variants on wire
        self.unpack_struct = struct.Struct('>I')

    def read(self):
        '''read()

Read and return a single event.'''

        raise Exception('method must be defined in derived classes')

class FileObjectReader(IReader):
    '''A simple writer that reads from an opened file object handle'''

    def __init__(self, handle):
        '''FileObjectReader(fh)

Initialize a new reader instance that associated with a File Object'''
        IReader.__init__(self)
        self.handle = handle
        
    def read(self):
        '''read()

Read and return the next event on the stream.'''
        buf = self.handle.read(4)
        size = self.unpack_struct.unpack(buf)[0]

        buf = self.handle.read(size)

        e = EventWrapper(serialized=buf)
        return e
