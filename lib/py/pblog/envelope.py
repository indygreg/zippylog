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

import sys
import time

from pblog.exception import PBException
from pblog.message_pb2 import Envelope as PBEnvelope

_messages = {}

def register_message(namespace, value, module, name):
    '''Registers a message type.

    For every message type, this function is called so the pblog system knows
    how to handle a specific message when it is encountered in a stream.

    The name argument is the message name, like 'MyEvent'. This corresponds
    to the 'message' type in a .proto file.

    The value argument is the numeric value pblog assigns to the message
    (via pblog_compile).

    The namespace argument defines the numeric message namespace to which the
    message belongs. A namespace of 0 (the default value) is the global
    namespace. When a message without an encoded namespace is encountered, the
    global namespace will be searched.

    Attempts to overwrite a previously-defined message will raise an
    exception.'''

    if value < 1:
        raise PBException('message value for %s is < 1' % name)

    if namespace < 0:
        raise PBException('namespace %d for message %s can not be negative' % ( namespace, name ))

    if namespace not in _messages:
        _messages[namespace] = {}

    if value in _messages[namespace]:
        raise PBException('namespace %d already contains message with value %d' % ( namespace, value ) )

    _messages[namespace][value] = ( module, name )

def register_all_messages(state):
    '''Registers all known messages so everything can be read.

    The passed object should be the loaded pblog state file contents.

    This should eventually grab the file from a package resource.'''
    for namespace, i in state['namespaces'].iteritems():
        for full, enum in state['messages'][namespace].iteritems():
            module, name = full.rsplit('.', 1)
            module += '_pb2'
            register_message(i, enum, module, name)

class Envelope():
    '''An individual pblog envelope

    This class provides a convenient wrapper around the
    pblog.message.pblog_pb2.Envelope class. It allows you to quickly fill in fields
    without having to worry too much about implementation details.'''

    def __init__(self,
                 serialized=None,
                 create_time=-1,
                 tags=None,
                 numeric=None,
                 message=None,
                 write_namespaces=False):
        '''Envelope(create_time=None,primary_key=None,secondary_keys=None,level=None,message=None)

        serialized contains a byte array of protocol buffer serialized data,
        which can be used to recreate a message.

        create_time defaults to -1, which means to use the current time.
        if create time is None, no time will be associated with the message.

        tags is a list of scalar types to associate with the message.
        
        numeric is an integer value to associate with the message.

        if message is defined and is a protocol buffer message, it will be
        added to the message.
        '''
        self.envelope = PBEnvelope()
        if serialized:
            self.envelope.MergeFromString(serialized)
            return

        if create_time >= 0:
            self.envelope.create_time = create_time
        elif create_time:
            self.envelope.create_time = int(time.time() * 1000000)

        self.tags = []

        if tags:
            self.tags = tags

        if numeric:
            self.numeric = numeric

        if message:
            self.add_message(message)

        self.write_namespaces = write_namespaces

    def get_envelope(self):
        return self.envelope

    def add_message(self, m):
        '''add_message(m)

        Adds a message to this one. The parameter should be protocol buffer
        class instance and should be registered with pblog.'''

        # we simply add the binary data directly to the messages list and recorded
        # the enumerated type of the message
        name = m.DESCRIPTOR.full_name

        self.envelope.message.append(m.SerializeToString())
        self.envelope.message_type.append(m.PBLOG_ENUMERATION)
        self.envelope.message_namespace.append(m.PBLOG_NAMESPACE)

    def get_message(self, index=0, default_namespace=None):
        '''get_message(i)

        Get the message at specified index.'''

        if index > len(self.envelope.message_type) - 1:
            raise PBException('message not available at index %d' % index)

        b = self.envelope.message[index]
        type_enumeration = self.envelope.message_type[index]
        namespace = None
        
        if len(self.envelope.message_namespace):
            namespace = self.envelope.message_namespace[index]
        else:
            namespace = default_namespace

        if not namespace:
            raise PBException('unknown namespace')

        if namespace not in _messages:
            raise PBException('seen namespace not registered with pblog: %d' % namespace )

        if type_enumeration not in _messages[namespace]:
            raise PBException(
                'type enumeration %d not registered for namespace %d'
                % ( type_enumeration, namespace )
            )

        module, name = _messages[namespace][type_enumeration]

        if module not in sys.modules:
            raise PBException('module not loaded: %s' % module)

        mod = sys.modules[module]

        e = eval('mod.%s()' % name)
        e.MergeFromString(b)
        return e

    def add_agent_info(self, i):
        '''add_agent_info(info)

        Add a pblog.message.pblog_pb2.AgentInfo instance to the envelope
        '''
        new_info = self.envelope.agent.add()
       
        for field in i.ListFields():
            new_info.__setattr__(field[0].name, field[1]) 

    def serialize(self):
        '''serialize()

        Return the binary serialization of the message instance'''
        return self.envelope.SerializeToString()
