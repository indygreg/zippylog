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
  serialized_pb='\n\x1cpblog/protocol/request.proto\x12\x16pblog.protocol.request\"\x0b\n\tStoreInfo\"\x17\n\x06Stream\x12\r\n\x05paths\x18\x01 \x03(\t\",\n\x03Get\x12\r\n\x05paths\x18\x01 \x03(\t\x12\x16\n\x0estream_offsets\x18\x02 \x03(\x04')




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
  serialized_start=56,
  serialized_end=67,
)


_STREAM = descriptor.Descriptor(
  name='Stream',
  full_name='pblog.protocol.request.Stream',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    descriptor.FieldDescriptor(
      name='paths', full_name='pblog.protocol.request.Stream.paths', index=0,
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
  serialized_start=69,
  serialized_end=92,
)


_GET = descriptor.Descriptor(
  name='Get',
  full_name='pblog.protocol.request.Get',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    descriptor.FieldDescriptor(
      name='paths', full_name='pblog.protocol.request.Get.paths', index=0,
      number=1, type=9, cpp_type=9, label=3,
      has_default_value=False, default_value=[],
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    descriptor.FieldDescriptor(
      name='stream_offsets', full_name='pblog.protocol.request.Get.stream_offsets', index=1,
      number=2, type=4, cpp_type=4, label=3,
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
  serialized_start=94,
  serialized_end=138,
)



class StoreInfo(message.Message):
  __metaclass__ = reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _STOREINFO
  
  PBLOG_NAMESPACE = 1
  PBLOG_ENUMERATION = 8
  # @@protoc_insertion_point(class_scope:pblog.protocol.request.StoreInfo)

class Stream(message.Message):
  __metaclass__ = reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _STREAM
  
  PBLOG_NAMESPACE = 1
  PBLOG_ENUMERATION = 9
  # @@protoc_insertion_point(class_scope:pblog.protocol.request.Stream)

class Get(message.Message):
  __metaclass__ = reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _GET
  
  PBLOG_NAMESPACE = 1
  PBLOG_ENUMERATION = 10
  # @@protoc_insertion_point(class_scope:pblog.protocol.request.Get)

register_message(1, 8, 'pblog.protocol.request_pb2', 'StoreInfo')
register_message(1, 9, 'pblog.protocol.request_pb2', 'Stream')
register_message(1, 10, 'pblog.protocol.request_pb2', 'Get')
# @@protoc_insertion_point(module_scope)
