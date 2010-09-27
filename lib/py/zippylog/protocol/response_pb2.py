# Generated by the protocol buffer compiler.  DO NOT EDIT!

from google.protobuf import descriptor
from google.protobuf import message
from google.protobuf import reflection
from google.protobuf import descriptor_pb2
from zippylog.envelope import register_message
# @@protoc_insertion_point(imports)


DESCRIPTOR = descriptor.FileDescriptor(
  name='zippylog/protocol/response.proto',
  package='zippylog.protocol.response',
  serialized_pb='\n zippylog/protocol/response.proto\x12\x1azippylog.protocol.response\"2\n\x12StreamSegmentStart\x12\x0c\n\x04path\x18\x01 \x01(\t\x12\x0e\n\x06offset\x18\x02 \x01(\x04\"\\\n\x10StreamSegmentEnd\x12\x0c\n\x04path\x18\x01 \x01(\t\x12\x0e\n\x06offset\x18\x02 \x01(\x04\x12\x12\n\nbytes_sent\x18\x03 \x01(\r\x12\x16\n\x0e\x65nvelopes_sent\x18\x04 \x01(\r\"\x1a\n\x0cSubscribeAck\x12\n\n\x02id\x18\x01 \x01(\t\"I\n\x05\x45rror\x12\x33\n\x04\x63ode\x18\x01 \x01(\x0e\x32%.zippylog.protocol.response.ErrorCode\x12\x0b\n\x03msg\x18\x02 \x01(\t*\xb0\x02\n\tErrorCode\x12\x1a\n\x16\x45NVELOPE_PARSE_FAILURE\x10\x01\x12\x1d\n\x19INVALID_MESSAGE_NAMESPACE\x10\x02\x12\x18\n\x14UNKNOWN_REQUEST_TYPE\x10\x03\x12\x12\n\x0e\x45MPTY_ENVELOPE\x10\x04\x12\x18\n\x14MISSING_ENUMERATIONS\x10\x05\x12\x1b\n\x17REQUEST_NOT_IMPLEMENTED\x10\x06\x12\x0f\n\x0b\x45MPTY_FIELD\x10\x07\x12\x1b\n\x17\x46IELD_LENGTHS_DIFFERENT\x10\x08\x12\x19\n\x15INVALID_STREAM_OFFSET\x10\t\x12\x12\n\x0eINVALID_OFFSET\x10\n\x12\x12\n\x0ePATH_NOT_FOUND\x10\x0b\x12\x12\n\x0eLIMIT_EXCEEDED\x10\x0c')

_ERRORCODE = descriptor.EnumDescriptor(
  name='ErrorCode',
  full_name='zippylog.protocol.response.ErrorCode',
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
  serialized_start=314,
  serialized_end=618,
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
  full_name='zippylog.protocol.response.StreamSegmentStart',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    descriptor.FieldDescriptor(
      name='path', full_name='zippylog.protocol.response.StreamSegmentStart.path', index=0,
      number=1, type=9, cpp_type=9, label=1,
      has_default_value=False, default_value=unicode("", "utf-8"),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    descriptor.FieldDescriptor(
      name='offset', full_name='zippylog.protocol.response.StreamSegmentStart.offset', index=1,
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
  serialized_start=64,
  serialized_end=114,
)


_STREAMSEGMENTEND = descriptor.Descriptor(
  name='StreamSegmentEnd',
  full_name='zippylog.protocol.response.StreamSegmentEnd',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    descriptor.FieldDescriptor(
      name='path', full_name='zippylog.protocol.response.StreamSegmentEnd.path', index=0,
      number=1, type=9, cpp_type=9, label=1,
      has_default_value=False, default_value=unicode("", "utf-8"),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    descriptor.FieldDescriptor(
      name='offset', full_name='zippylog.protocol.response.StreamSegmentEnd.offset', index=1,
      number=2, type=4, cpp_type=4, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    descriptor.FieldDescriptor(
      name='bytes_sent', full_name='zippylog.protocol.response.StreamSegmentEnd.bytes_sent', index=2,
      number=3, type=13, cpp_type=3, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    descriptor.FieldDescriptor(
      name='envelopes_sent', full_name='zippylog.protocol.response.StreamSegmentEnd.envelopes_sent', index=3,
      number=4, type=13, cpp_type=3, label=1,
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
  serialized_start=116,
  serialized_end=208,
)


_SUBSCRIBEACK = descriptor.Descriptor(
  name='SubscribeAck',
  full_name='zippylog.protocol.response.SubscribeAck',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    descriptor.FieldDescriptor(
      name='id', full_name='zippylog.protocol.response.SubscribeAck.id', index=0,
      number=1, type=9, cpp_type=9, label=1,
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
  serialized_start=210,
  serialized_end=236,
)


_ERROR = descriptor.Descriptor(
  name='Error',
  full_name='zippylog.protocol.response.Error',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    descriptor.FieldDescriptor(
      name='code', full_name='zippylog.protocol.response.Error.code', index=0,
      number=1, type=14, cpp_type=8, label=1,
      has_default_value=False, default_value=1,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    descriptor.FieldDescriptor(
      name='msg', full_name='zippylog.protocol.response.Error.msg', index=1,
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
  serialized_start=238,
  serialized_end=311,
)


_ERROR.fields_by_name['code'].enum_type = _ERRORCODE

class StreamSegmentStart(message.Message):
  __metaclass__ = reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _STREAMSEGMENTSTART
  
  ZIPPYLOG_NAMESPACE = 1
  ZIPPYLOG_ENUMERATION = 7
  # @@protoc_insertion_point(class_scope:zippylog.protocol.response.StreamSegmentStart)

class StreamSegmentEnd(message.Message):
  __metaclass__ = reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _STREAMSEGMENTEND
  
  ZIPPYLOG_NAMESPACE = 1
  ZIPPYLOG_ENUMERATION = 8
  # @@protoc_insertion_point(class_scope:zippylog.protocol.response.StreamSegmentEnd)

class SubscribeAck(message.Message):
  __metaclass__ = reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _SUBSCRIBEACK
  
  ZIPPYLOG_NAMESPACE = 1
  ZIPPYLOG_ENUMERATION = 24
  # @@protoc_insertion_point(class_scope:zippylog.protocol.response.SubscribeAck)

class Error(message.Message):
  __metaclass__ = reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _ERROR
  
  ZIPPYLOG_NAMESPACE = 1
  ZIPPYLOG_ENUMERATION = 9
  # @@protoc_insertion_point(class_scope:zippylog.protocol.response.Error)

register_message(1, 7, 'zippylog.protocol.response_pb2', 'StreamSegmentStart')
register_message(1, 8, 'zippylog.protocol.response_pb2', 'StreamSegmentEnd')
register_message(1, 24, 'zippylog.protocol.response_pb2', 'SubscribeAck')
register_message(1, 9, 'zippylog.protocol.response_pb2', 'Error')
# @@protoc_insertion_point(module_scope)
