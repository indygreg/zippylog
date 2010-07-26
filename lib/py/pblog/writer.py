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

# TODO we probably should not be using an internal module
from google.protobuf.internal.encoder import _VarintEncoder
from pblog.event import LogEvent
from pblog.pblog_pb2 import WriterInfo
import socket
import time

class IWriter():
    '''Interface for all pblog writers'''

    def __init__(self, write_hostname=False):
        '''IWriter()

Default constructor. Sets some reasonable defaults'''

        self.sequence = 0
        self.hostname = socket.gethostname()
        self.host_id = None
        self.app_id = None
        self.write_hostname = write_hostname
        self.stream_version = 1

        self.varint_encoder = _VarintEncoder()

    def write(self, e):
        '''write(event)

Writes the passed event.

This must be defined in child classes.'''
        raise Exception('method must be defined in derived classes')

class FileObjectWriter(IWriter):
    '''A simple writer that writes out to an opened file object handle'''

    def __init__(self, handle, has_written=False, stream_version=1):
        '''FileObjectWriter(fh, [has_written=False])

        Initialize a new writer instance that associated with a File Object.

        The has_written parameter tells the writer if the output stream has been
        written to. This is used to properly encode the stream with the stream
        output version.

        stream_version is the pblog stream version encoding to use. Currently, only
        version 1 is supported. If writing to new streams, you almost always want to
        leave this as the default
        '''
        IWriter.__init__(self)
        self.handle = handle
        self.has_written = has_written

        if stream_version != 1:
            raise Exception('stream version %d not supported. 1 is only supported' % stream_version)

        self.stream_version = stream_version
        self.app_id = 'py_fileobjectwriter'
        
        self.writer_info = WriterInfo()
        
        if self.write_hostname:
            self.writer_info.hostname = self.hostname
   
        self.writer_info.app_id = self.app_id

    def flush(self):
        self.handle.flush()

    def write(self, e):
        '''Write an individual event to the writer.

        If the message is not a pblog.event.LogEvent() but is a
        google.protobuf.message, it will be wrapped in a LogEvent()
        automatically.
        '''

        # convert to wrapped message automagically
        if not isinstance(e, LogEvent):
            e = LogEvent(message=e)

        self.writer_info.write_time = int(time.time() * 1000000)
        self.writer_info.sequence_id = self.sequence
        self.sequence = self.sequence + 1

        e.add_writer_info(self.writer_info)
        binary = e.serialize()

        # the first byte of the stream is the stream version
        if not self.has_written:
            self.handle.write(chr(0x01))
            self.has_written = True

        self.varint_encoder(self.handle.write, len(binary))
        self.handle.write(binary)


