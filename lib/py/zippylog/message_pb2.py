# Generated by the protocol buffer compiler.  DO NOT EDIT!

from google.protobuf import descriptor
from google.protobuf import message
from google.protobuf import reflection
from google.protobuf import descriptor_pb2
# @@protoc_insertion_point(imports)



DESCRIPTOR = descriptor.FileDescriptor(
  name='zippylog/message.proto',
  package='zippylog.message',
  serialized_pb='\n\x16zippylog/message.proto\x12\x10zippylog.message\"\xe3\x01\n\x08\x45nvelope\x12\x12\n\x07version\x18\x01 \x01(\r:\x01\x31\x12\x0f\n\x07message\x18\x02 \x03(\x0c\x12\x1d\n\x11message_namespace\x18\x03 \x03(\rB\x02\x10\x01\x12\x18\n\x0cmessage_type\x18\x04 \x03(\rB\x02\x10\x01\x12\x13\n\x0b\x63reate_time\x18\x05 \x01(\x04\x12\x0b\n\x03tag\x18\x06 \x03(\x0c\x12*\n\x05\x61\x63tor\x18\x07 \x03(\x0b\x32\x1b.zippylog.message.ActorInfo\x12\x15\n\rnumeric_value\x18\x08 \x01(\x04\x12\x14\n\x0cstring_value\x18\t \x01(\t\"y\n\tActorInfo\x12\x12\n\ntouch_time\x18\x01 \x01(\x04\x12\x13\n\x0bsequence_id\x18\x02 \x01(\x04\x12\x10\n\x08hostname\x18\x03 \x01(\t\x12\x0f\n\x07host_id\x18\x04 \x01(\x0c\x12\x10\n\x08\x61\x63tor_id\x18\x05 \x01(\x0c\x12\x0e\n\x06\x61pp_id\x18\x06 \x01(\x0c')




_ENVELOPE = descriptor.Descriptor(
  name='Envelope',
  full_name='zippylog.message.Envelope',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    descriptor.FieldDescriptor(
      name='version', full_name='zippylog.message.Envelope.version', index=0,
      number=1, type=13, cpp_type=3, label=1,
      has_default_value=True, default_value=1,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    descriptor.FieldDescriptor(
      name='message', full_name='zippylog.message.Envelope.message', index=1,
      number=2, type=12, cpp_type=9, label=3,
      has_default_value=False, default_value=[],
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    descriptor.FieldDescriptor(
      name='message_namespace', full_name='zippylog.message.Envelope.message_namespace', index=2,
      number=3, type=13, cpp_type=3, label=3,
      has_default_value=False, default_value=[],
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=descriptor._ParseOptions(descriptor_pb2.FieldOptions(), '\020\001')),
    descriptor.FieldDescriptor(
      name='message_type', full_name='zippylog.message.Envelope.message_type', index=3,
      number=4, type=13, cpp_type=3, label=3,
      has_default_value=False, default_value=[],
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=descriptor._ParseOptions(descriptor_pb2.FieldOptions(), '\020\001')),
    descriptor.FieldDescriptor(
      name='create_time', full_name='zippylog.message.Envelope.create_time', index=4,
      number=5, type=4, cpp_type=4, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    descriptor.FieldDescriptor(
      name='tag', full_name='zippylog.message.Envelope.tag', index=5,
      number=6, type=12, cpp_type=9, label=3,
      has_default_value=False, default_value=[],
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    descriptor.FieldDescriptor(
      name='actor', full_name='zippylog.message.Envelope.actor', index=6,
      number=7, type=11, cpp_type=10, label=3,
      has_default_value=False, default_value=[],
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    descriptor.FieldDescriptor(
      name='numeric_value', full_name='zippylog.message.Envelope.numeric_value', index=7,
      number=8, type=4, cpp_type=4, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    descriptor.FieldDescriptor(
      name='string_value', full_name='zippylog.message.Envelope.string_value', index=8,
      number=9, type=9, cpp_type=9, label=1,
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
  serialized_start=45,
  serialized_end=272,
)


_ACTORINFO = descriptor.Descriptor(
  name='ActorInfo',
  full_name='zippylog.message.ActorInfo',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    descriptor.FieldDescriptor(
      name='touch_time', full_name='zippylog.message.ActorInfo.touch_time', index=0,
      number=1, type=4, cpp_type=4, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    descriptor.FieldDescriptor(
      name='sequence_id', full_name='zippylog.message.ActorInfo.sequence_id', index=1,
      number=2, type=4, cpp_type=4, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    descriptor.FieldDescriptor(
      name='hostname', full_name='zippylog.message.ActorInfo.hostname', index=2,
      number=3, type=9, cpp_type=9, label=1,
      has_default_value=False, default_value=unicode("", "utf-8"),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    descriptor.FieldDescriptor(
      name='host_id', full_name='zippylog.message.ActorInfo.host_id', index=3,
      number=4, type=12, cpp_type=9, label=1,
      has_default_value=False, default_value="",
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    descriptor.FieldDescriptor(
      name='actor_id', full_name='zippylog.message.ActorInfo.actor_id', index=4,
      number=5, type=12, cpp_type=9, label=1,
      has_default_value=False, default_value="",
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    descriptor.FieldDescriptor(
      name='app_id', full_name='zippylog.message.ActorInfo.app_id', index=5,
      number=6, type=12, cpp_type=9, label=1,
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
  serialized_start=274,
  serialized_end=395,
)

_ENVELOPE.fields_by_name['actor'].message_type = _ACTORINFO
DESCRIPTOR.message_types_by_name['Envelope'] = _ENVELOPE
DESCRIPTOR.message_types_by_name['ActorInfo'] = _ACTORINFO

class Envelope(message.Message):
  __metaclass__ = reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _ENVELOPE
  
  ZIPPYLOG_NAMESPACE = 1
  ZIPPYLOG_ENUMERATION = 30
  # @@protoc_insertion_point(class_scope:zippylog.message.Envelope)

class ActorInfo(message.Message):
  __metaclass__ = reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _ACTORINFO
  
  ZIPPYLOG_NAMESPACE = 1
  ZIPPYLOG_ENUMERATION = 31
  # @@protoc_insertion_point(class_scope:zippylog.message.ActorInfo)

# @@protoc_insertion_point(module_scope)
