# Generated by the protocol buffer compiler.  DO NOT EDIT!

from google.protobuf import descriptor
from google.protobuf import message
from google.protobuf import reflection
from google.protobuf import descriptor_pb2
from pblog.envelope import register_message
# @@protoc_insertion_point(imports)


DESCRIPTOR = descriptor.FileDescriptor(
  name='pblog/protocol/response.proto',
  package='pblog.protocol.response',
  serialized_pb='\n\x1dpblog/protocol/response.proto\x12\x17pblog.protocol.response\"?\n\x12StreamSegmentStart\x12\x0c\n\x04path\x18\x01 \x01(\t\x12\x1b\n\x13stream_start_offset\x18\x02 \x01(\x04\"<\n\x10StreamSegmentEnd\x12\x0c\n\x04path\x18\x01 \x01(\t\x12\x1a\n\x12stream_stop_offset\x18\x02 \x01(\x04\"F\n\x05\x45rror\x12\x30\n\x04\x63ode\x18\x01 \x01(\x0e\x32\".pblog.protocol.response.ErrorCode\x12\x0b\n\x03msg\x18\x02 \x01(\t*\xb0\x02\n\tErrorCode\x12\x1a\n\x16\x45NVELOPE_PARSE_FAILURE\x10\x01\x12\x1d\n\x19INVALID_MESSAGE_NAMESPACE\x10\x02\x12\x18\n\x14UNKNOWN_REQUEST_TYPE\x10\x03\x12\x12\n\x0e\x45MPTY_ENVELOPE\x10\x04\x12\x18\n\x14MISSING_ENUMERATIONS\x10\x05\x12\x1b\n\x17REQUEST_NOT_IMPLEMENTED\x10\x06\x12\x0f\n\x0b\x45MPTY_FIELD\x10\x07\x12\x1b\n\x17\x46IELD_LENGTHS_DIFFERENT\x10\x08\x12\x19\n\x15INVALID_STREAM_OFFSET\x10\t\x12\x12\n\x0eINVALID_OFFSET\x10\n\x12\x12\n\x0ePATH_NOT_FOUND\x10\x0b\x12\x12\n\x0eLIMIT_EXCEEDED\x10\x0c')

_ERRORCODE = descriptor.EnumDescriptor(
  name='ErrorCode',
  full_name='pblog.protocol.response.ErrorCode',
  filename=None,
  file=DESCRIPTOR,
  values=[
    descriptor.EnumValueDescriptor(
      name='ENVELOPE_PARSE_FAILURE', index=0, number=1,
      options=None,
      type=None),
    descriptor.EnumValueDescriptor(
      name='INVALID_MESSAGE_NAMESPACE', index=1, number=2,
      options=None,
      type=None),
    descriptor.EnumValueDescriptor(
      name='UNKNOWN_REQUEST_TYPE', index=2, number=3,
      options=None,
      type=None),
    descriptor.EnumValueDescriptor(
      name='EMPTY_ENVELOPE', index=3, number=4,
      options=None,
      type=None),
    descriptor.EnumValueDescriptor(
      name='MISSING_ENUMERATIONS', index=4, number=5,
      options=None,
      type=None),
    descriptor.EnumValueDescriptor(
      name='REQUEST_NOT_IMPLEMENTED', index=5, number=6,
      options=None,
      type=None),
    descriptor.EnumValueDescriptor(
      name='EMPTY_FIELD', index=6, number=7,
      options=None,
      type=None),
    descriptor.EnumValueDescriptor(
      name='FIELD_LENGTHS_DIFFERENT', index=7, number=8,
      options=None,
      type=None),
    descriptor.EnumValueDescriptor(
      name='INVALID_STREAM_OFFSET', index=8, number=9,
      options=None,
      type=None),
    descriptor.EnumValueDescriptor(
      name='INVALID_OFFSET', index=9, number=10,
      options=None,
      type=None),
    descriptor.EnumValueDescriptor(
      name='PATH_NOT_FOUND', index=10, number=11,
      options=None,
      type=None),
    descriptor.EnumValueDescriptor(
      name='LIMIT_EXCEEDED', index=11, number=12,
      options=None,
      type=None),
  ],
  containing_type=None,
  options=None,
  serialized_start=258,
  serialized_end=562,
)


ENVELOPE_PARSE_FAILURE = 1
INVALID_MESSAGE_NAMESPACE = 2
UNKNOWN_REQUEST_TYPE = 3
EMPTY_ENVELOPE = 4
MISSING_ENUMERATIONS = 5
REQUEST_NOT_IMPLEMENTED = 6
EMPTY_FIELD = 7
FIELD_LENGTHS_DIFFERENT = 8
INVALID_STREAM_OFFSET = 9
INVALID_OFFSET = 10
PATH_NOT_FOUND = 11
LIMIT_EXCEEDED = 12



_STREAMSEGMENTSTART = descriptor.Descriptor(
  name='StreamSegmentStart',
  full_name='pblog.protocol.response.StreamSegmentStart',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    descriptor.FieldDescriptor(
      name='path', full_name='pblog.protocol.response.StreamSegmentStart.path', index=0,
      number=1, type=9, cpp_type=9, label=1,
      has_default_value=False, default_value=unicode("", "utf-8"),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    descriptor.FieldDescriptor(
      name='stream_start_offset', full_name='pblog.protocol.response.StreamSegmentStart.stream_start_offset', index=1,
      number=2, type=4, cpp_type=4, label=1,
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
  serialized_start=58,
  serialized_end=121,
)


_STREAMSEGMENTEND = descriptor.Descriptor(
  name='StreamSegmentEnd',
  full_name='pblog.protocol.response.StreamSegmentEnd',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    descriptor.FieldDescriptor(
      name='path', full_name='pblog.protocol.response.StreamSegmentEnd.path', index=0,
      number=1, type=9, cpp_type=9, label=1,
      has_default_value=False, default_value=unicode("", "utf-8"),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    descriptor.FieldDescriptor(
      name='stream_stop_offset', full_name='pblog.protocol.response.StreamSegmentEnd.stream_stop_offset', index=1,
      number=2, type=4, cpp_type=4, label=1,
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
  serialized_start=123,
  serialized_end=183,
)


_ERROR = descriptor.Descriptor(
  name='Error',
  full_name='pblog.protocol.response.Error',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    descriptor.FieldDescriptor(
      name='code', full_name='pblog.protocol.response.Error.code', index=0,
      number=1, type=14, cpp_type=8, label=1,
      has_default_value=False, default_value=1,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    descriptor.FieldDescriptor(
      name='msg', full_name='pblog.protocol.response.Error.msg', index=1,
      number=2, type=9, cpp_type=9, label=1,
      has_default_value=False, default_value=unicode("", "utf-8"),
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
  serialized_start=185,
  serialized_end=255,
)


_ERROR.fields_by_name['code'].enum_type = _ERRORCODE

class StreamSegmentStart(message.Message):
  __metaclass__ = reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _STREAMSEGMENTSTART
  
  PBLOG_NAMESPACE = 1
  PBLOG_ENUMERATION = 7
  # @@protoc_insertion_point(class_scope:pblog.protocol.response.StreamSegmentStart)

class StreamSegmentEnd(message.Message):
  __metaclass__ = reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _STREAMSEGMENTEND
  
  PBLOG_NAMESPACE = 1
  PBLOG_ENUMERATION = 8
  # @@protoc_insertion_point(class_scope:pblog.protocol.response.StreamSegmentEnd)

class Error(message.Message):
  __metaclass__ = reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _ERROR
  
  PBLOG_NAMESPACE = 1
  PBLOG_ENUMERATION = 9
  # @@protoc_insertion_point(class_scope:pblog.protocol.response.Error)

register_message(1, 7, 'pblog.protocol.response_pb2', 'StreamSegmentStart')
register_message(1, 8, 'pblog.protocol.response_pb2', 'StreamSegmentEnd')
register_message(1, 9, 'pblog.protocol.response_pb2', 'Error')
# @@protoc_insertion_point(module_scope)
