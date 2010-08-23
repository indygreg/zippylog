# Generated by the protocol buffer compiler.  DO NOT EDIT!

from google.protobuf import descriptor
from google.protobuf import message
from google.protobuf import reflection
from google.protobuf import descriptor_pb2
from pblog.envelope import register_message
# @@protoc_insertion_point(imports)


DESCRIPTOR = descriptor.FileDescriptor(
  name='pblog/protocol/request.proto',
  package='pblog.protocol.request',
  serialized_pb='\n\x1cpblog/protocol/request.proto\x12\x16pblog.protocol.request\x1a\x14pblog/protocol.proto\"\x0b\n\tStoreInfo\"\x16\n\x06Stream\x12\x0c\n\x04path\x18\x01 \x03(\t\"^\n\x03Get\x12\x34\n\x06stream\x18\x01 \x03(\x0b\x32$.pblog.protocol.GetStreamDescription\x12!\n\x12\x62ytes_per_response\x18\x02 \x01(\r:\x05\x36\x35\x35\x33\x35')




_STOREINFO = descriptor.Descriptor(
  name='StoreInfo',
  full_name='pblog.protocol.request.StoreInfo',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  options=None,
  is_extendable=False,
  extension_ranges=[],
  serialized_start=78,
  serialized_end=89,
)


_STREAM = descriptor.Descriptor(
  name='Stream',
  full_name='pblog.protocol.request.Stream',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    descriptor.FieldDescriptor(
      name='path', full_name='pblog.protocol.request.Stream.path', index=0,
      number=1, type=9, cpp_type=9, label=3,
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
  is_extendable=False,
  extension_ranges=[],
  serialized_start=91,
  serialized_end=113,
)


_GET = descriptor.Descriptor(
  name='Get',
  full_name='pblog.protocol.request.Get',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    descriptor.FieldDescriptor(
      name='stream', full_name='pblog.protocol.request.Get.stream', index=0,
      number=1, type=11, cpp_type=10, label=3,
      has_default_value=False, default_value=[],
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    descriptor.FieldDescriptor(
      name='bytes_per_response', full_name='pblog.protocol.request.Get.bytes_per_response', index=1,
      number=2, type=13, cpp_type=3, label=1,
      has_default_value=True, default_value=65535,
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
  serialized_start=115,
  serialized_end=209,
)

import pblog.protocol_pb2

_GET.fields_by_name['stream'].message_type = pblog.protocol_pb2._GETSTREAMDESCRIPTION

class StoreInfo(message.Message):
  __metaclass__ = reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _STOREINFO
  
  PBLOG_NAMESPACE = 1
  PBLOG_ENUMERATION = 11
  # @@protoc_insertion_point(class_scope:pblog.protocol.request.StoreInfo)

class Stream(message.Message):
  __metaclass__ = reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _STREAM
  
  PBLOG_NAMESPACE = 1
  PBLOG_ENUMERATION = 12
  # @@protoc_insertion_point(class_scope:pblog.protocol.request.Stream)

class Get(message.Message):
  __metaclass__ = reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _GET
  
  PBLOG_NAMESPACE = 1
  PBLOG_ENUMERATION = 13
  # @@protoc_insertion_point(class_scope:pblog.protocol.request.Get)

register_message(1, 11, 'pblog.protocol.request_pb2', 'StoreInfo')
register_message(1, 12, 'pblog.protocol.request_pb2', 'Stream')
register_message(1, 13, 'pblog.protocol.request_pb2', 'Get')
# @@protoc_insertion_point(module_scope)
