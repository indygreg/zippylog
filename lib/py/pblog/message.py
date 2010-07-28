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

from pblog.pblog_pb2 import Message as PBMessage
import sys
import time

_types_numeric = {}
_types_fullname = {}

def register_types(d):
    '''Registers a dictionary of types with message system.

    The passed argument should be the TYPES_BY_INDEX module variable from
    automatically generated module built by pblog.'''

    for k,v in d.iteritems():
        _types_numeric[k] = v
        _types_fullname[v[2]] = k

class Message():
    '''An individual pblog message

    This class provides a convenient wrapper around the
    pblog.pblog_pb2.Message class. It allows you to quickly fill in fields
    without having to worry too much about implementation details.'''

    def __init__(self,
                 serialized=None,
                 create_time=-1,
                 tags=None,
                 numeric=None,
                 message=None,
                 write_namespaces=False):
        '''Message(create_time=None,primary_key=None,secondary_keys=None,level=None,message=None)

        serialized contains a byte array of protocol buffer serialized data,
        which can be used to recreate a message.

        create_time defaults to -1, which means to use the current time.
        if create time is None, no time will be associated with the message.

        tags is a list of scalar types to associate with the message.
        
        numeric is an integer value to associate with the message.

        if message is defined and is a protocol buffer message, it will be
        added to the message.
        '''
        self.message = PBMessage()
        if serialized:
            self.message.MergeFromString(serialized)
            return

        if create_time >= 0:
            self.message.create_time = create_time
        elif create_time:
            self.message.create_time = int(time.time() * 1000000)

        self.tags = []

        if tags:
            self.tags = tags

        if numeric:
            self.numeric = numeric

        if message:
            self.add_message(message)

        self.write_namespaces = write_namespaces

    def get_message(self):
        return self.message

    def add_message(self, m):
        '''add_message(m)

        Adds a message to this one. The parameter should be protocol buffer
        class instance and should be registered with pblog.'''

        # we simply add the binary data directly to the messages list and recorded
        # the enumerated type of the message
        name = m.DESCRIPTOR.full_name
        if name not in _types_fullname:
            raise Exception('%s type not registered with pblog')

        self.message.messages.append(m.SerializeToString())
        self.message.message_types.append(_types_fullname[name])

        # TODO implement namespaces

    def get_message(self, index=0):
        '''get_message(i)

        Get the message at specified index.'''

        if index > len(self.message.message_types) - 1:
            raise Exception('message not available at index %d' % index)

        b = self.message.messages[index]

        type_constant = self.message.message_types[index]
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

    def add_agent_info(self, i):
        '''add_agent_info(info)

        Add a pblog.pblog_pb2.AgentInfo instance to the message
        '''
        new_info = self.message.agents.add()
       
        for field in i.ListFields():
            new_info.__setattr__(field[0].name, field[1]) 

    def serialize(self):
        '''serialize()

        Return the binary serialization of the message instance'''
        return self.message.SerializeToString()
