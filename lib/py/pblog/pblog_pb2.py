# Generated by the protocol buffer compiler.  DO NOT EDIT!

from google.protobuf import descriptor
from google.protobuf import message
from google.protobuf import reflection
from google.protobuf import descriptor_pb2
# @@protoc_insertion_point(imports)


DESCRIPTOR = descriptor.FileDescriptor(
  name='pblog/pblog.proto',
  package='pblog',
  serialized_pb='\n\x11pblog/pblog.proto\x12\x05pblog\x1a\x1dpblog/site/MessageTypes.proto\"\xc7\x01\n\x08LogEvent\x12\x13\n\x0b\x63reate_time\x18\x01 \x01(\x04\x12\x13\n\x0bprimary_key\x18\x02 \x01(\x0c\x12\x16\n\x0esecondary_keys\x18\x03 \x03(\x0c\x12\"\n\x07writers\x18\x04 \x03(\x0b\x32\x11.pblog.WriterInfo\x12\r\n\x05level\x18\x05 \x01(\r\x12,\n\x0b\x65vent_types\x18\x06 \x03(\x0e\x32\x17.pblog.site.MessageType\x12\x0e\n\x06\x65vents\x18\x07 \x03(\x0c*\x08\x08\x64\x10\x80\x80\x80\x80\x02\"h\n\nWriterInfo\x12\x12\n\nwrite_time\x18\x01 \x01(\x04\x12\x13\n\x0bsequence_id\x18\x02 \x01(\x04\x12\x10\n\x08hostname\x18\x03 \x01(\t\x12\x0f\n\x07host_id\x18\x04 \x01(\x0c\x12\x0e\n\x06\x61pp_id\x18\x05 \x01(\x0c')




_LOGEVENT = descriptor.Descriptor(
  name='LogEvent',
  full_name='pblog.LogEvent',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    descriptor.FieldDescriptor(
      name='create_time', full_name='pblog.LogEvent.create_time', index=0,
      number=1, type=4, cpp_type=4, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    descriptor.FieldDescriptor(
      name='primary_key', full_name='pblog.LogEvent.primary_key', index=1,
      number=2, type=12, cpp_type=9, label=1,
      has_default_value=False, default_value="",
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    descriptor.FieldDescriptor(
      name='secondary_keys', full_name='pblog.LogEvent.secondary_keys', index=2,
      number=3, type=12, cpp_type=9, label=3,
      has_default_value=False, default_value=[],
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    descriptor.FieldDescriptor(
      name='writers', full_name='pblog.LogEvent.writers', index=3,
      number=4, type=11, cpp_type=10, label=3,
      has_default_value=False, default_value=[],
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    descriptor.FieldDescriptor(
      name='level', full_name='pblog.LogEvent.level', index=4,
      number=5, type=13, cpp_type=3, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    descriptor.FieldDescriptor(
      name='event_types', full_name='pblog.LogEvent.event_types', index=5,
      number=6, type=14, cpp_type=8, label=3,
      has_default_value=False, default_value=[],
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    descriptor.FieldDescriptor(
      name='events', full_name='pblog.LogEvent.events', index=6,
      number=7, type=12, cpp_type=9, label=3,
      has_default_value=False, default_value=[],
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  options=None,
  is_extendable=True,
  extension_ranges=[(100, 536870912), ],
  serialized_start=60,
  serialized_end=259,
)


_WRITERINFO = descriptor.Descriptor(
  name='WriterInfo',
  full_name='pblog.WriterInfo',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    descriptor.FieldDescriptor(
      name='write_time', full_name='pblog.WriterInfo.write_time', index=0,
      number=1, type=4, cpp_type=4, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    descriptor.FieldDescriptor(
      name='sequence_id', full_name='pblog.WriterInfo.sequence_id', index=1,
      number=2, type=4, cpp_type=4, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    descriptor.FieldDescriptor(
      name='hostname', full_name='pblog.WriterInfo.hostname', index=2,
      number=3, type=9, cpp_type=9, label=1,
      has_default_value=False, default_value=unicode("", "utf-8"),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    descriptor.FieldDescriptor(
      name='host_id', full_name='pblog.WriterInfo.host_id', index=3,
      number=4, type=12, cpp_type=9, label=1,
      has_default_value=False, default_value="",
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    descriptor.FieldDescriptor(
      name='app_id', full_name='pblog.WriterInfo.app_id', index=4,
      number=5, type=12, cpp_type=9, label=1,
      has_default_value=False, default_value="",
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  options=None,
  is_extendable=False,
  extension_ranges=[],
  serialized_start=261,
  serialized_end=365,
)

import pblog.site.MessageTypes_pb2

_LOGEVENT.fields_by_name['writers'].message_type = _WRITERINFO
_LOGEVENT.fields_by_name['event_types'].enum_type = pblog.site.MessageTypes_pb2._MESSAGETYPE

class LogEvent(message.Message):
  __metaclass__ = reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _LOGEVENT
  
  # @@protoc_insertion_point(class_scope:pblog.LogEvent)

class WriterInfo(message.Message):
  __metaclass__ = reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _WRITERINFO
  
  # @@protoc_insertion_point(class_scope:pblog.WriterInfo)

# @@protoc_insertion_point(module_scope)
