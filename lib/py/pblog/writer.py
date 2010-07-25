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

from pblog.pblog_pb2 import LogEvent, WriterInfo
import socket
import struct
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

        # TODO encode using variant
        self.pack_struct = struct.Struct('>I')

    def write(self, e):
        '''write(event)

Writes the passed event.

This must be defined in child classes.'''
        raise Exception('method must be defined in derived classes')

class FileObjectWriter(IWriter):
    '''A simple writer that writes out to an opened file object handle'''

    def __init__(self, handle):
        '''FileObjectWriter(fh)

Initialize a new writer instance that associated with a File Object'''
        IWriter.__init__(self)
        self.handle = handle
        self.app_id = 'py_fileobjectwriter'
        
        self.writer_info = WriterInfo()
        
        if self.write_hostname:
            self.writer_info.hostname = self.hostname
   
        self.writer_info.app_id = self.app_id

    def flush(self):
        self.handle.flush()

    def write(self, e):
        '''write(event)

Write an individual event to the writer.

Returns the number of bytes written.'''
        self.writer_info.write_time = int(time.time() * 1000000)
        self.writer_info.sequence_id = self.sequence
        self.sequence = self.sequence + 1

        e.add_writer_info(self.writer_info)
        binary = e.serialize()
        self.handle.write(self.pack_struct.pack(len(binary)))
        self.handle.write(binary)

        return len(binary) + 4

