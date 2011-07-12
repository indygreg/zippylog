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
  serialized_pb='\n zippylog/protocol/response.proto\x12\x1azippylog.protocol.response\"\x08\n\x06PongV1\"\xc5\x01\n\x16\x46\x65\x61tureSpecificationV1\x12!\n\x19supported_message_formats\x18\x01 \x03(\r\x12\x1f\n\x17supported_request_types\x18\x02 \x03(\r\x12\x1f\n\x17supported_request_names\x18\x03 \x03(\t\x12 \n\x18max_stream_segment_bytes\x18\x04 \x01(\r\x12$\n\x1cmax_stream_segment_envelopes\x18\x05 \x01(\r\"4\n\x14StreamSegmentStartV1\x12\x0c\n\x04path\x18\x01 \x02(\t\x12\x0e\n\x06offset\x18\x02 \x02(\x04\"P\n\x12StreamSegmentEndV1\x12\x0e\n\x06offset\x18\x01 \x02(\x04\x12\x12\n\nbytes_sent\x18\x02 \x02(\r\x12\x16\n\x0e\x65nvelopes_sent\x18\x03 \x02(\r\")\n\x0eSubscribeAckV1\x12\n\n\x02id\x18\x01 \x02(\x0c\x12\x0b\n\x03ttl\x18\x02 \x02(\r\"!\n\x13SubscriptionStartV1\x12\n\n\x02id\x18\x01 \x02(\x0c\"\'\n\nWriteAckV1\x12\x19\n\x11\x65nvelopes_written\x18\x01 \x02(\r\"I\n\x05\x45rror\x12\x33\n\x04\x63ode\x18\x01 \x01(\x0e\x32%.zippylog.protocol.response.ErrorCode\x12\x0b\n\x03msg\x18\x02 \x01(\t*\x86\x04\n\tErrorCode\x12\x1a\n\x16\x45NVELOPE_PARSE_FAILURE\x10\x01\x12\x1d\n\x19INVALID_MESSAGE_NAMESPACE\x10\x02\x12\x18\n\x14UNKNOWN_REQUEST_TYPE\x10\x03\x12\x12\n\x0e\x45MPTY_ENVELOPE\x10\x04\x12\x18\n\x14MISSING_ENUMERATIONS\x10\x05\x12\x1b\n\x17REQUEST_NOT_IMPLEMENTED\x10\x06\x12\x0f\n\x0b\x45MPTY_FIELD\x10\x07\x12\x1b\n\x17\x46IELD_LENGTHS_DIFFERENT\x10\x08\x12\x19\n\x15INVALID_STREAM_OFFSET\x10\t\x12\x12\n\x0eINVALID_OFFSET\x10\n\x12\x12\n\x0ePATH_NOT_FOUND\x10\x0b\x12\x12\n\x0eLIMIT_EXCEEDED\x10\x0c\x12\"\n\x1eUNKNOWN_MESSAGE_FORMAT_VERSION\x10\r\x12\x10\n\x0cINVALID_PATH\x10\x0e\x12)\n%UNSUPPORTED_OPERATION_MESSAGE_VERSION\x10\x0f\x12\x11\n\rEMPTY_MESSAGE\x10\x10\x12\x18\n\x14PROTOCOL_NO_ENVELOPE\x10\x11\x12\x1c\n\x18GENERAL_ERROR_PROCESSING\x10\x12\x12\r\n\tLUA_ERROR\x10\x13\x12\x19\n\x15SUBSCRIPTION_REJECTED\x10\x14')

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
    descriptor.EnumValueDescriptor(
      name='UNKNOWN_MESSAGE_FORMAT_VERSION', index=12, number=13,
      options=None,
      type=None),
    descriptor.EnumValueDescriptor(
      name='INVALID_PATH', index=13, number=14,
      options=None,
      type=None),
    descriptor.EnumValueDescriptor(
      name='UNSUPPORTED_OPERATION_MESSAGE_VERSION', index=14, number=15,
      options=None,
      type=None),
    descriptor.EnumValueDescriptor(
      name='EMPTY_MESSAGE', index=15, number=16,
      options=None,
      type=None),
    descriptor.EnumValueDescriptor(
      name='PROTOCOL_NO_ENVELOPE', index=16, number=17,
      options=None,
      type=None),
    descriptor.EnumValueDescriptor(
      name='GENERAL_ERROR_PROCESSING', index=17, number=18,
      options=None,
      type=None),
    descriptor.EnumValueDescriptor(
      name='LUA_ERROR', index=18, number=19,
      options=None,
      type=None),
    descriptor.EnumValueDescriptor(
      name='SUBSCRIPTION_REJECTED', index=19, number=20,
      options=None,
      type=None),
  ],
  containing_type=None,
  options=None,
  serialized_start=605,
  serialized_end=1123,
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
UNKNOWN_MESSAGE_FORMAT_VERSION = 13
INVALID_PATH = 14
UNSUPPORTED_OPERATION_MESSAGE_VERSION = 15
EMPTY_MESSAGE = 16
PROTOCOL_NO_ENVELOPE = 17
GENERAL_ERROR_PROCESSING = 18
LUA_ERROR = 19
SUBSCRIPTION_REJECTED = 20



_PONGV1 = descriptor.Descriptor(
  name='PongV1',
  full_name='zippylog.protocol.response.PongV1',
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
  serialized_start=64,
  serialized_end=72,
)


_FEATURESPECIFICATIONV1 = descriptor.Descriptor(
  name='FeatureSpecificationV1',
  full_name='zippylog.protocol.response.FeatureSpecificationV1',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    descriptor.FieldDescriptor(
      name='supported_message_formats', full_name='zippylog.protocol.response.FeatureSpecificationV1.supported_message_formats', index=0,
      number=1, type=13, cpp_type=3, label=3,
      has_default_value=False, default_value=[],
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    descriptor.FieldDescriptor(
      name='supported_request_types', full_name='zippylog.protocol.response.FeatureSpecificationV1.supported_request_types', index=1,
      number=2, type=13, cpp_type=3, label=3,
      has_default_value=False, default_value=[],
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    descriptor.FieldDescriptor(
      name='supported_request_names', full_name='zippylog.protocol.response.FeatureSpecificationV1.supported_request_names', index=2,
      number=3, type=9, cpp_type=9, label=3,
      has_default_value=False, default_value=[],
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    descriptor.FieldDescriptor(
      name='max_stream_segment_bytes', full_name='zippylog.protocol.response.FeatureSpecificationV1.max_stream_segment_bytes', index=3,
      number=4, type=13, cpp_type=3, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    descriptor.FieldDescriptor(
      name='max_stream_segment_envelopes', full_name='zippylog.protocol.response.FeatureSpecificationV1.max_stream_segment_envelopes', index=4,
      number=5, type=13, cpp_type=3, label=1,
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
  serialized_start=75,
  serialized_end=272,
)


_STREAMSEGMENTSTARTV1 = descriptor.Descriptor(
  name='StreamSegmentStartV1',
  full_name='zippylog.protocol.response.StreamSegmentStartV1',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    descriptor.FieldDescriptor(
      name='path', full_name='zippylog.protocol.response.StreamSegmentStartV1.path', index=0,
      number=1, type=9, cpp_type=9, label=2,
      has_default_value=False, default_value=unicode("", "utf-8"),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    descriptor.FieldDescriptor(
      name='offset', full_name='zippylog.protocol.response.StreamSegmentStartV1.offset', index=1,
      number=2, type=4, cpp_type=4, label=2,
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
  serialized_start=274,
  serialized_end=326,
)


_STREAMSEGMENTENDV1 = descriptor.Descriptor(
  name='StreamSegmentEndV1',
  full_name='zippylog.protocol.response.StreamSegmentEndV1',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    descriptor.FieldDescriptor(
      name='offset', full_name='zippylog.protocol.response.StreamSegmentEndV1.offset', index=0,
      number=1, type=4, cpp_type=4, label=2,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    descriptor.FieldDescriptor(
      name='bytes_sent', full_name='zippylog.protocol.response.StreamSegmentEndV1.bytes_sent', index=1,
      number=2, type=13, cpp_type=3, label=2,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    descriptor.FieldDescriptor(
      name='envelopes_sent', full_name='zippylog.protocol.response.StreamSegmentEndV1.envelopes_sent', index=2,
      number=3, type=13, cpp_type=3, label=2,
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
  serialized_start=328,
  serialized_end=408,
)


_SUBSCRIBEACKV1 = descriptor.Descriptor(
  name='SubscribeAckV1',
  full_name='zippylog.protocol.response.SubscribeAckV1',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    descriptor.FieldDescriptor(
      name='id', full_name='zippylog.protocol.response.SubscribeAckV1.id', index=0,
      number=1, type=12, cpp_type=9, label=2,
      has_default_value=False, default_value="",
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    descriptor.FieldDescriptor(
      name='ttl', full_name='zippylog.protocol.response.SubscribeAckV1.ttl', index=1,
      number=2, type=13, cpp_type=3, label=2,
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
  serialized_start=410,
  serialized_end=451,
)


_SUBSCRIPTIONSTARTV1 = descriptor.Descriptor(
  name='SubscriptionStartV1',
  full_name='zippylog.protocol.response.SubscriptionStartV1',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    descriptor.FieldDescriptor(
      name='id', full_name='zippylog.protocol.response.SubscriptionStartV1.id', index=0,
      number=1, type=12, cpp_type=9, label=2,
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
  serialized_start=453,
  serialized_end=486,
)


_WRITEACKV1 = descriptor.Descriptor(
  name='WriteAckV1',
  full_name='zippylog.protocol.response.WriteAckV1',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    descriptor.FieldDescriptor(
      name='envelopes_written', full_name='zippylog.protocol.response.WriteAckV1.envelopes_written', index=0,
      number=1, type=13, cpp_type=3, label=2,
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
  serialized_start=488,
  serialized_end=527,
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
  serialized_start=529,
  serialized_end=602,
)

_ERROR.fields_by_name['code'].enum_type = _ERRORCODE
DESCRIPTOR.message_types_by_name['PongV1'] = _PONGV1
DESCRIPTOR.message_types_by_name['FeatureSpecificationV1'] = _FEATURESPECIFICATIONV1
DESCRIPTOR.message_types_by_name['StreamSegmentStartV1'] = _STREAMSEGMENTSTARTV1
DESCRIPTOR.message_types_by_name['StreamSegmentEndV1'] = _STREAMSEGMENTENDV1
DESCRIPTOR.message_types_by_name['SubscribeAckV1'] = _SUBSCRIBEACKV1
DESCRIPTOR.message_types_by_name['SubscriptionStartV1'] = _SUBSCRIPTIONSTARTV1
DESCRIPTOR.message_types_by_name['WriteAckV1'] = _WRITEACKV1
DESCRIPTOR.message_types_by_name['Error'] = _ERROR

class PongV1(message.Message):
  __metaclass__ = reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _PONGV1
  
  ZIPPYLOG_NAMESPACE = 1
  ZIPPYLOG_ENUMERATION = 70
  # @@protoc_insertion_point(class_scope:zippylog.protocol.response.PongV1)

class FeatureSpecificationV1(message.Message):
  __metaclass__ = reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _FEATURESPECIFICATIONV1
  
  ZIPPYLOG_NAMESPACE = 1
  ZIPPYLOG_ENUMERATION = 71
  # @@protoc_insertion_point(class_scope:zippylog.protocol.response.FeatureSpecificationV1)

class StreamSegmentStartV1(message.Message):
  __metaclass__ = reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _STREAMSEGMENTSTARTV1
  
  ZIPPYLOG_NAMESPACE = 1
  ZIPPYLOG_ENUMERATION = 104
  # @@protoc_insertion_point(class_scope:zippylog.protocol.response.StreamSegmentStartV1)

class StreamSegmentEndV1(message.Message):
  __metaclass__ = reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _STREAMSEGMENTENDV1
  
  ZIPPYLOG_NAMESPACE = 1
  ZIPPYLOG_ENUMERATION = 105
  # @@protoc_insertion_point(class_scope:zippylog.protocol.response.StreamSegmentEndV1)

class SubscribeAckV1(message.Message):
  __metaclass__ = reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _SUBSCRIBEACKV1
  
  ZIPPYLOG_NAMESPACE = 1
  ZIPPYLOG_ENUMERATION = 106
  # @@protoc_insertion_point(class_scope:zippylog.protocol.response.SubscribeAckV1)

class SubscriptionStartV1(message.Message):
  __metaclass__ = reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _SUBSCRIPTIONSTARTV1
  
  ZIPPYLOG_NAMESPACE = 1
  ZIPPYLOG_ENUMERATION = 107
  # @@protoc_insertion_point(class_scope:zippylog.protocol.response.SubscriptionStartV1)

class WriteAckV1(message.Message):
  __metaclass__ = reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _WRITEACKV1
  
  ZIPPYLOG_NAMESPACE = 1
  ZIPPYLOG_ENUMERATION = 108
  # @@protoc_insertion_point(class_scope:zippylog.protocol.response.WriteAckV1)

class Error(message.Message):
  __metaclass__ = reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _ERROR
  
  ZIPPYLOG_NAMESPACE = 1
  ZIPPYLOG_ENUMERATION = 69
  # @@protoc_insertion_point(class_scope:zippylog.protocol.response.Error)

register_message(1, 70, 'zippylog.protocol.response_pb2', 'PongV1')
register_message(1, 71, 'zippylog.protocol.response_pb2', 'FeatureSpecificationV1')
register_message(1, 104, 'zippylog.protocol.response_pb2', 'StreamSegmentStartV1')
register_message(1, 105, 'zippylog.protocol.response_pb2', 'StreamSegmentEndV1')
register_message(1, 106, 'zippylog.protocol.response_pb2', 'SubscribeAckV1')
register_message(1, 107, 'zippylog.protocol.response_pb2', 'SubscriptionStartV1')
register_message(1, 108, 'zippylog.protocol.response_pb2', 'WriteAckV1')
register_message(1, 69, 'zippylog.protocol.response_pb2', 'Error')
# @@protoc_insertion_point(module_scope)
