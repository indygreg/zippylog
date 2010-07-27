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

from calendar import timegm

# TODO we probably should not be using an internal module
from google.protobuf.internal.encoder import _VarintEncoder
import os.path
from pblog.message import Message
from pblog.pblog_pb2 import AgentInfo
import socket
import time

class IWriter():
    '''Interface for all pblog writers'''

    def __init__(self,
                 write_hostname=False,
                 write_namespaces=False):
        '''IWriter()

        Default constructor. Sets some reasonable defaults.'''

        self.sequence = 0
        self.hostname = socket.gethostname()
        self.host_id = None
        self.app_id = None
        self.write_hostname = write_hostname
        self.stream_version = 1

        self.varint_encoder = _VarintEncoder()

    def write(self, m):
        '''write(message)

        Writes the passed event.

        This must be defined in child classes.'''
        raise Exception('method must be defined in derived classes')

    def _prepare_message(self, m):
        # convert to wrapped message automagically
        if not isinstance(m, Message):
            m = Message(message=m)

        return m

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
        leave this as the default.

        File objects should be opened in binary mode.
        '''
        IWriter.__init__(self)
        self.handle = handle
        self.has_written = has_written

        if stream_version != 1:
            raise Exception('stream version %d not supported. 1 is only supported' % stream_version)

        self.stream_version = stream_version
        self.app_id = 'py_fileobjectwriter'
        
        self.agent_info = AgentInfo()
        
        if self.write_hostname:
            self.agent_info.hostname = self.hostname
   
        self.agent_info.app_id = self.app_id

    def flush(self):
        self.handle.flush()

    def write(self, message):
        '''Write an individual message.

        If the message is not a pblog.event.Message() but is a
        google.protobuf.message, it will be wrapped in a pblog.event.Message
        automatically.
        '''

        message = self._prepare_message(message)
        self._write(message)

    def _write(self, e, t=None):

        if not t:
            t = time.time()

        self.agent_info.touch_time = int(t * 1000000)
        self.agent_info.sequence_id = self.sequence
        self.sequence += 1

        e.add_agent_info(self.agent_info)

        binary = e.serialize()

        # the first byte of the stream is the stream version
        if not self.has_written:
            self.handle.write(chr(0x01))
            self.has_written = True

        self.varint_encoder(self.handle.write, len(binary))
        self.handle.write(binary)

class FileSystemStreamStoreWriter(IWriter):
    '''Writes streams in a filesystem-backed stream store.'''

    def __init__(self, root_directory, default_bucket, default_streamset, seconds_per_file=3600):
        if not os.path.exists(root_directory):
            raise Exception('root directory does not exists: %s' % root_directory)

        self.root_directory = os.path.realpath(root_directory)
        
        bucket_dir = os.path.realpath(os.path.join(self.root_directory, default_bucket))

        if not os.path.exists(bucket_dir):
            raise Exception('default bucket directory does not exist: %s' % bucket_dir)

        # TODO verify bucket_dir is a subdir of root_directory

        self.default_bucket = default_bucket
        self.default_streamset = default_streamset

        self.writer = None
        self.active_file = None
        self.next_switch_time = 0
        self.seconds_per_file = seconds_per_file

    def write(self, e):
        t = time.time()

        if t >= self.next_switch_time:
            timepart, self.next_switch_time = file_for_time(int(t), self.seconds_per_file)
            self.active_file = os.path.join(self.root_directory, self.default_bucket, self.default_streamset, timepart)
       
            # TODO check if file exists so we flag has_written properly
            fh = open(self.active_file, 'wb')
            self.writer = FileObjectWriter(fh)

        self.writer._write(self._prepare_message(e), t)


    def flush(self):
        if self.writer:
            self.writer.flush()

def file_for_time(t, seconds_per_file):
    '''Determine the file to write to for the given time.

    Returns a typle consiting of a string filename and the next integer
    corresponding to a new time.'''
    st = time.gmtime(t)

    # TODO compiled formatters

    # daily
    if seconds_per_file == 86400:
        filename = '%04d-%02d-%02d.pblog' % (st[0], st[1], st[2])

        tomorrow = t + 86400
        st = time.gmtime(tomorrow)

        return filename, timegm((st[0], st[1], st[2], 0, 0, 0))

    # hourly
    elif seconds_per_file == 3600:
        filename = '%04d-%02d-%02d-%02d.pblog' % (st[0], st[1], st[2], st[3])

        next_time = t + 3600
        st = time.gmtime(next_time)

        return filename, timegm((st[0], st[1], st[2], st[3], 0, 0))

    elif seconds_per_file > 3600 or seconds_per_file < 4:
        raise Exception('%d is not an allowed seconds_per_file value' % seconds_per_file)

    else:
        # TODO switch to a lookup table for efficiency?
        if 3600 % seconds_per_file > 0:
            raise Exception('%d does not divide into 3600 evenly' % seconds_per_file)

        series_max = 3600 / seconds_per_file
        seconds_since_hour = st[4] * 60 + st[5]

        current = seconds_since_hour / seconds_per_file + 1

        filename = '%04d-%02d-%02d-%02d-%03d-%03d.pblog' % ( st[0], st[1], st[2], st[3], current, series_max)

        t = timegm((st[0], st[1], st[2], st[3], 0, 0))
            
        return filename, t + current * seconds_per_file
            
