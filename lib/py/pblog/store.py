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
import os
import os.path
import time

from pblog.exception import PBException
from pblog.message import Message
from pblog.stream import Stream

class IStreamStore:
    def buckets(self):
        '''Returns a list of buckets in the store.'''
        raise PBException('not implemented')

    def stream_sets_in_bucket(self, bucket):
        '''Returns a list of stream sets in a named bucket.'''
        raise PBException('not implemented')

    def streams_in_set(self, bucket, stream_set):
        '''Returns a list of streams in a specific stream set.'''
        raise PBException('not implemented')

    def get_streamset(self, bucket, name):
        raise PBException('not implemented')


class FilesystemStreamStore(IStreamStore):
    '''Stream store backed by a filesystem.'''

    def __init__(self, root_path):
        '''Construct a stream store whose root is at the specified path.'''

        if not os.path.exists(root_path):
            raise PBException('root path does not exists: %s' % root_path)

        if not os.path.isdir(root_path):
            raise PBException('root path is not a directory: %s' % root_path)

        self.root_path = root_path

    def buckets(self):
        return [ p for p in os.listdir(self.root_path) 
            if os.path.isdir(os.path.join(self.root_path, p)) 
        ]

    def stream_sets_in_bucket(self, bucket):
        return [ p for p in os.listdir(os.path.join(self.root_path, bucket))
            if os.path.isdir(os.path.join(self.root_path, bucket, p))
        ]

    def streams_in_set(self, bucket, stream_set):
        set_dir = os.path.join(self.root_path, bucket, stream_set)
        return [ p for p in os.listdir(set_dir)
            if os.path.isfile(os.path.join(set_dir, p)) and p.endswith(('.pblog', '.pblog.z'))
        ]

    def get_stream_set(self, bucket, name, create=False):
        ss = StreamSet()
        
        base = os.path.join(self.root_path, bucket, name)

        if create and not os.path.exists(base):
            os.makedirs(base, 0775)

        ss.set_open_write_stream_callback(
            lambda f: self._get_stream_for_writing(base, f)
        )
        ss.set_open_read_stream_callback(
            lambda f: self._get_stream_for_reading(base, f)
        )

        return ss

    def _get_stream_for_writing(self, base, name):
        f = os.path.join(base, name + '.pblog')
        return Stream(
            open(f, 'ab', -1),
            is_empty=(os.path.getsize(f) == 0)
        )

    def _get_stream_for_reading(self, base, name):
        f = os.path.join(base, name + '.pblog')
        return Stream(open(f, 'rb', -1))

class StreamSet:
    '''A set of streams.

    This is the class that handles all the magic for managing multiple
    streams.'''

    # TODO support a bunch of arguments
    def __init__(self):
        self.streams = {}

        self.next_write_switch_time = 0
        self.active_stream_name = None
        self.seconds_per_file = 3600
        self.active_out_stream = None

    def write_message(self, message, t=None):
        if t is None:
            t = time.time()

        if t >= self.next_write_switch_time:
            self.active_stream_name, self.next_write_switch_time = stream_name_for_time(int(t), self.seconds_per_file)

            # TODO error checking
            self.active_out_stream = self.get_write_stream(self.active_stream_name)

        self.active_out_stream.write_messages(message)

    def set_open_write_stream_callback(self, callback):
        self.get_write_stream = callback

    def set_open_read_stream_callback(self, callback):
        self.get_read_stream = callback

def stream_name_for_time(t, seconds_per_file):
    '''Determine the filename to write to for the given time.

    Returns a typle consiting of a string filename component and the next
    integer corresponding to the time when a new stream name would be required.'''
    st = time.gmtime(t)

    # TODO compiled formatters

    # daily
    if seconds_per_file == 86400:
        filename = '%04d-%02d-%02d' % (st[0], st[1], st[2])

        tomorrow = t + 86400
        st = time.gmtime(tomorrow)

        return filename, timegm((st[0], st[1], st[2], 0, 0, 0))

    # hourly
    elif seconds_per_file == 3600:
        filename = '%04d-%02d-%02d-%02d' % (st[0], st[1], st[2], st[3])

        next_time = t + 3600
        st = time.gmtime(next_time)

        return filename, timegm((st[0], st[1], st[2], st[3], 0, 0))

    elif seconds_per_file > 3600 or seconds_per_file < 4:
        raise PBException('%d is not an allowed seconds_per_file value' % seconds_per_file)

    else:
        # TODO switch to a lookup table for efficiency?
        if 3600 % seconds_per_file > 0:
            raise PBException('%d does not divide into 3600 evenly' % seconds_per_file)

        series_max = 3600 / seconds_per_file
        seconds_since_hour = st[4] * 60 + st[5]

        current = seconds_since_hour / seconds_per_file + 1

        filename = '%04d-%02d-%02d-%02d-%03d-%03d' % ( st[0], st[1], st[2], st[3], current, series_max)

        t = timegm((st[0], st[1], st[2], st[3], 0, 0))

        return filename, t + current * seconds_per_file
