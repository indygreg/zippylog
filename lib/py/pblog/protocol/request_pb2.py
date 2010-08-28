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
  serialized_pb='\n\x1cpblog/protocol/request.proto\x12\x16pblog.protocol.request\"\x0b\n\tStoreInfo\"E\n\x03Get\x12\x0c\n\x04path\x18\x01 \x01(\t\x12\x14\n\x0cstart_offset\x18\x02 \x01(\x04\x12\x1a\n\x12max_response_bytes\x18\x03 \x01(\r')




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


_GET = descriptor.Descriptor(
  name='Get',
  full_name='pblog.protocol.request.Get',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    descriptor.FieldDescriptor(
      name='path', full_name='pblog.protocol.request.Get.path', index=0,
      number=1, type=9, cpp_type=9, label=1,
      has_default_value=False, default_value=unicode("", "utf-8"),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    descriptor.FieldDescriptor(
      name='start_offset', full_name='pblog.protocol.request.Get.start_offset', index=1,
      number=2, type=4, cpp_type=4, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    descriptor.FieldDescriptor(
      name='max_response_bytes', full_name='pblog.protocol.request.Get.max_response_bytes', index=2,
      number=3, type=13, cpp_type=3, label=1,
      has_default_value=False, default_value=0,
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
  serialized_end=138,
)



class StoreInfo(message.Message):
  __metaclass__ = reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _STOREINFO
  
  PBLOG_NAMESPACE = 1
  PBLOG_ENUMERATION = 10
  # @@protoc_insertion_point(class_scope:pblog.protocol.request.StoreInfo)

class Get(message.Message):
  __metaclass__ = reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _GET
  
  PBLOG_NAMESPACE = 1
  PBLOG_ENUMERATION = 11
  # @@protoc_insertion_point(class_scope:pblog.protocol.request.Get)

register_message(1, 10, 'pblog.protocol.request_pb2', 'StoreInfo')
register_message(1, 11, 'pblog.protocol.request_pb2', 'Get')
# @@protoc_insertion_point(module_scope)
