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

import time

from pblog.exception import PBException
from pblog.message import Message
from pblog.pblog_pb2 import ActorInfo

class IActor:
    def __init__(self, write_namespaces=False, write_actor_info=True):
        self.in_streams = {}
        self.out_streams = {}
        self.write_sequence = 0
        self.hostname = None
        self.host_id = None
        self.actor_id = None
        self.app_id = None
        self.write_namespaces = bool(write_namespaces)
        self.write_actor_info = bool(write_actor_info)
        self.actor_info = ActorInfo()

    '''Interface for pblog actors.'''
    def write_messages(self, *messages):
        raise PBException('must be implemented in derived classes')

    def add_read_stream(self, name, stream):
        self.in_streams[name] = stream

    def add_write_stream(self, name, stream):
        self.out_streams[name] = stream

    def set_write_host_identifier(self, hostname=None, host_id=None):
        self.hostname = hostname

        if host is not None:
            self.actor_info.hostname = hostname
        else:
            self.actor_info.ClearField('hostname')

        self.host_id = host_id

        if host_id is not None:
            self.actor_info.host_id = host_id
        else:
            self.actor_info.ClearField('host_id')

    def set_actor_id(self, ident=None):
        self.actor_id = ident

        if ident is not None:
            self.actor_info.actor_id = ident
        else:
            self.actor_info.ClearField('actor_id')

    def set_application_id(self, ident=None):
        self.application_id = ident

        if ident is not None:
            self.actor_info.app_id = ident
        else:
            self.actor_info.ClearField('app_id')

    def set_write_namespaces(self, b):
        self.write_namespaces = bool(b)

    def write_message(self, message):
        if not isinstance(message, Message):
            m = Message(message=m)

        t = time.time()

        if self.write_actor_info:
            self.actor_info.touch_time = int(t * 1000000)
            self.actor_info.sequence_id = self.write_sequence

        # always increment, regardless if we are writing info
        self.write_sequence += 1

        m.add_actor_info(self.actor_info)

        # TODO namespace support

        for s in self.out_streams.itervalues():
            s.write_messages(message)
            
    def write_pblog_messages(self, *messages):
        '''Write multiple pblog.message.Message instances.'''
        for m in messages:
            if not isinstance(m, Message):
                raise PBException('object is not a pblog.message.Message type: %s' % type(m))

            self.write_message(m)

    def combine_and_write_messages(self, *messages):
        '''Write multiple protocol buffer messages to a pblog message.

        This takes multiple protocol buffer messages, embeds them in a pblog
        message container, and writes that single message.'''
        # TODO support additional parameters for common message container
        raise PBException('not yet implemented')

