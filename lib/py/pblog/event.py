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

from pblog.pblog_pb2 import LogEvent as PBEvent
import sys
import time

_types_numeric = {}
_types_fullname = {}

def register_types(d):
    '''Registers a dictionary of types with the event system.

    The passed argument should be the TYPES_BY_INDEX module variable from
    automatically generated module built by pblog.'''

    for k,v in d.iteritems():
        _types_numeric[k] = v
        _types_fullname[v[2]] = k

class LogEvent():
    '''An individual Protocol Buffer Log Event

This class provides a convenient wrapper around the
pblog.pblog_pbs.LogEvent class. It allows you to quickly fill in fields
without having to worry too much about implementation details.'''

    def __init__(self, serialized=None, create_time=None, primary_key=None, secondary_keys=None, level=None, message=None):
        '''LogEvent(create_time=None,primary_key=None,secondary_keys=None,level=None,message=None)

Construct an empty log event.

If not defined, the event's time will be set to the current time.
All other options have no defaults.'''
        self.event = PBEvent()
        if serialized:
            self.event.MergeFromString(serialized)
            return

        if create_time:
            self.event.create_time = create_time
        else:
            self.event.create_time = int(time.time() * 1000000)

        if primary_key:
            self.event.primary_key = primary_key

        if secondary_keys:
            self.event.secondary_keys = secondary_keys

        if level:
            self.event.level = level

        if message:
            self.add_message(message)

    def get_event(self):
        return self.event

    def add_message(self, m):
        '''add_message(m)

        Adds a message to this event. The parameter should be protocol buffer
        class instance and should be registered with the pblog.site package.'''

        # we simply add the binary data directly to the messages list and recorded
        # the enumerated type of the message
        name = m.DESCRIPTOR.full_name
        if name not in _types_fullname:
            raise Exception('%s type not registered with pblog')

        self.event.events.append(m.SerializeToString())
        self.event.event_types.append(_types_fullname[name])

    def get_message(self, index=0):
        '''get_message(i)

Get the message at specified index.'''

        if index > len(self.event.event_types) - 1:
            raise Exception('message not available at index %d' % index)

        b = self.event.events[index]

        type_constant = self.event.event_types[index]
        if type_constant not in _types_numeric:
            raise Exception('type constant not registered: %d' % type_constant)

        t = _types_numeric[type_constant]

        mod_name = t[0]
        if mod_name not in sys.modules:
            raise Exception('type constant is imported but the module is not present, weird')

        mod = sys.modules[mod_name]
        cl = t[1]
        e = eval('mod.%s()' % cl)
        e.MergeFromString(b)

        return e

    def add_writer_info(self, i):
        '''add_writer_info(info)

Add a pblog.pblog_pb2.WriterInfo instance to the list of writers
associated with the event.

This is likely only called by a writer implementation at write time.'''
        new_info = self.event.writers.add()
       
        for field in i.ListFields():
            new_info.__setattr__(field[0].name, field[1]) 

    def serialize(self):
        '''serialize()

Return the binary serialization of the message instance'''
        return self.event.SerializeToString()
