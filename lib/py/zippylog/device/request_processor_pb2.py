# Generated by the protocol buffer compiler.  DO NOT EDIT!

from google.protobuf import descriptor
from google.protobuf import message
from google.protobuf import reflection
from google.protobuf import descriptor_pb2
from zippylog.envelope import register_message
# @@protoc_insertion_point(imports)


DESCRIPTOR = descriptor.FileDescriptor(
  name='zippylog/device/request_processor.proto',
  package='zippylog.device.request_processor',
  serialized_pb='\n\'zippylog/device/request_processor.proto\x12!zippylog.device.request_processor\"\x14\n\x06\x43reate\x12\n\n\x02id\x18\x01 \x01(\x0c\"\x15\n\x07\x44\x65stroy\x12\n\n\x02id\x18\x01 \x01(\x0c\"\x16\n\x08RunStart\x12\n\n\x02id\x18\x01 \x01(\x0c\"\x15\n\x07RunStop\x12\n\n\x02id\x18\x02 \x01(\x0c\" \n\x12\x46\x61ilReceiveMessage\x12\n\n\x02id\x18\x01 \x01(\x0c\"!\n\x13ReceiveEmptyMessage\x12\n\n\x02id\x18\x01 \x01(\x0c\"4\n\x15UnknownMessageVersion\x12\n\n\x02id\x18\x01 \x01(\x0c\x12\x0f\n\x07version\x18\x02 \x01(\r\"5\n\x12UnknownRequestType\x12\n\n\x02id\x18\x01 \x01(\x0c\x12\x13\n\x0b\x65numeration\x18\x02 \x01(\r\"0\n\x14\x45nvelopeParseFailure\x12\n\n\x02id\x18\x01 \x01(\x0c\x12\x0c\n\x04\x64\x61ta\x18\x02 \x01(\x0c\"\x1b\n\rEmptyEnvelope\x12\n\n\x02id\x18\x01 \x01(\x0c\"\'\n\x19InvalidMessageEnumeration\x12\n\n\x02id\x18\x01 \x01(\x0c\"#\n\x15\x42\x65ginProcessStoreInfo\x12\n\n\x02id\x18\x01 \x01(\x0c\"!\n\x13\x45ndProcessStoreInfo\x12\n\n\x02id\x18\x01 \x01(\x0c\"\x1f\n\x11ReceiveInvalidGet\x12\n\n\x02id\x18\x01 \x01(\x0c\"\x1e\n\x10GetInvalidStream\x12\n\n\x02id\x18\x01 \x01(\x0c\"\x1e\n\x10GetInvalidOffset\x12\n\n\x02id\x18\x01 \x01(\x0c\"\x1d\n\x0f\x42\x65ginProcessGet\x12\n\n\x02id\x18\x01 \x01(\x0c\"\x1b\n\rEndProcessGet\x12\n\n\x02id\x18\x01 \x01(\x0c\"0\n\x11SendErrorResponse\x12\n\n\x02id\x18\x01 \x01(\x0c\x12\x0f\n\x07message\x18\x02 \x01(\t\"=\n\x19\x46orwardSubscribeKeepalive\x12\n\n\x02id\x18\x01 \x01(\x0c\x12\x14\n\x0csubscription\x18\x02 \x01(\x0c')




_CREATE = descriptor.Descriptor(
  name='Create',
  full_name='zippylog.device.request_processor.Create',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    descriptor.FieldDescriptor(
      name='id', full_name='zippylog.device.request_processor.Create.id', index=0,
      number=1, type=12, cpp_type=9, label=1,
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
  serialized_start=78,
  serialized_end=98,
)


_DESTROY = descriptor.Descriptor(
  name='Destroy',
  full_name='zippylog.device.request_processor.Destroy',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    descriptor.FieldDescriptor(
      name='id', full_name='zippylog.device.request_processor.Destroy.id', index=0,
      number=1, type=12, cpp_type=9, label=1,
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
  serialized_start=100,
  serialized_end=121,
)


_RUNSTART = descriptor.Descriptor(
  name='RunStart',
  full_name='zippylog.device.request_processor.RunStart',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    descriptor.FieldDescriptor(
      name='id', full_name='zippylog.device.request_processor.RunStart.id', index=0,
      number=1, type=12, cpp_type=9, label=1,
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
  serialized_start=123,
  serialized_end=145,
)


_RUNSTOP = descriptor.Descriptor(
  name='RunStop',
  full_name='zippylog.device.request_processor.RunStop',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    descriptor.FieldDescriptor(
      name='id', full_name='zippylog.device.request_processor.RunStop.id', index=0,
      number=2, type=12, cpp_type=9, label=1,
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
  serialized_start=147,
  serialized_end=168,
)


_FAILRECEIVEMESSAGE = descriptor.Descriptor(
  name='FailReceiveMessage',
  full_name='zippylog.device.request_processor.FailReceiveMessage',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    descriptor.FieldDescriptor(
      name='id', full_name='zippylog.device.request_processor.FailReceiveMessage.id', index=0,
      number=1, type=12, cpp_type=9, label=1,
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
  serialized_start=170,
  serialized_end=202,
)


_RECEIVEEMPTYMESSAGE = descriptor.Descriptor(
  name='ReceiveEmptyMessage',
  full_name='zippylog.device.request_processor.ReceiveEmptyMessage',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    descriptor.FieldDescriptor(
      name='id', full_name='zippylog.device.request_processor.ReceiveEmptyMessage.id', index=0,
      number=1, type=12, cpp_type=9, label=1,
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
  serialized_start=204,
  serialized_end=237,
)


_UNKNOWNMESSAGEVERSION = descriptor.Descriptor(
  name='UnknownMessageVersion',
  full_name='zippylog.device.request_processor.UnknownMessageVersion',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    descriptor.FieldDescriptor(
      name='id', full_name='zippylog.device.request_processor.UnknownMessageVersion.id', index=0,
      number=1, type=12, cpp_type=9, label=1,
      has_default_value=False, default_value="",
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    descriptor.FieldDescriptor(
      name='version', full_name='zippylog.device.request_processor.UnknownMessageVersion.version', index=1,
      number=2, type=13, cpp_type=3, label=1,
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
  serialized_start=239,
  serialized_end=291,
)


_UNKNOWNREQUESTTYPE = descriptor.Descriptor(
  name='UnknownRequestType',
  full_name='zippylog.device.request_processor.UnknownRequestType',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    descriptor.FieldDescriptor(
      name='id', full_name='zippylog.device.request_processor.UnknownRequestType.id', index=0,
      number=1, type=12, cpp_type=9, label=1,
      has_default_value=False, default_value="",
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    descriptor.FieldDescriptor(
      name='enumeration', full_name='zippylog.device.request_processor.UnknownRequestType.enumeration', index=1,
      number=2, type=13, cpp_type=3, label=1,
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
  serialized_start=293,
  serialized_end=346,
)


_ENVELOPEPARSEFAILURE = descriptor.Descriptor(
  name='EnvelopeParseFailure',
  full_name='zippylog.device.request_processor.EnvelopeParseFailure',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    descriptor.FieldDescriptor(
      name='id', full_name='zippylog.device.request_processor.EnvelopeParseFailure.id', index=0,
      number=1, type=12, cpp_type=9, label=1,
      has_default_value=False, default_value="",
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    descriptor.FieldDescriptor(
      name='data', full_name='zippylog.device.request_processor.EnvelopeParseFailure.data', index=1,
      number=2, type=12, cpp_type=9, label=1,
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
  serialized_start=348,
  serialized_end=396,
)


_EMPTYENVELOPE = descriptor.Descriptor(
  name='EmptyEnvelope',
  full_name='zippylog.device.request_processor.EmptyEnvelope',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    descriptor.FieldDescriptor(
      name='id', full_name='zippylog.device.request_processor.EmptyEnvelope.id', index=0,
      number=1, type=12, cpp_type=9, label=1,
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
  serialized_start=398,
  serialized_end=425,
)


_INVALIDMESSAGEENUMERATION = descriptor.Descriptor(
  name='InvalidMessageEnumeration',
  full_name='zippylog.device.request_processor.InvalidMessageEnumeration',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    descriptor.FieldDescriptor(
      name='id', full_name='zippylog.device.request_processor.InvalidMessageEnumeration.id', index=0,
      number=1, type=12, cpp_type=9, label=1,
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
  serialized_start=427,
  serialized_end=466,
)


_BEGINPROCESSSTOREINFO = descriptor.Descriptor(
  name='BeginProcessStoreInfo',
  full_name='zippylog.device.request_processor.BeginProcessStoreInfo',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    descriptor.FieldDescriptor(
      name='id', full_name='zippylog.device.request_processor.BeginProcessStoreInfo.id', index=0,
      number=1, type=12, cpp_type=9, label=1,
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
  serialized_start=468,
  serialized_end=503,
)


_ENDPROCESSSTOREINFO = descriptor.Descriptor(
  name='EndProcessStoreInfo',
  full_name='zippylog.device.request_processor.EndProcessStoreInfo',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    descriptor.FieldDescriptor(
      name='id', full_name='zippylog.device.request_processor.EndProcessStoreInfo.id', index=0,
      number=1, type=12, cpp_type=9, label=1,
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
  serialized_start=505,
  serialized_end=538,
)


_RECEIVEINVALIDGET = descriptor.Descriptor(
  name='ReceiveInvalidGet',
  full_name='zippylog.device.request_processor.ReceiveInvalidGet',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    descriptor.FieldDescriptor(
      name='id', full_name='zippylog.device.request_processor.ReceiveInvalidGet.id', index=0,
      number=1, type=12, cpp_type=9, label=1,
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
  serialized_start=540,
  serialized_end=571,
)


_GETINVALIDSTREAM = descriptor.Descriptor(
  name='GetInvalidStream',
  full_name='zippylog.device.request_processor.GetInvalidStream',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    descriptor.FieldDescriptor(
      name='id', full_name='zippylog.device.request_processor.GetInvalidStream.id', index=0,
      number=1, type=12, cpp_type=9, label=1,
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
  serialized_start=573,
  serialized_end=603,
)


_GETINVALIDOFFSET = descriptor.Descriptor(
  name='GetInvalidOffset',
  full_name='zippylog.device.request_processor.GetInvalidOffset',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    descriptor.FieldDescriptor(
      name='id', full_name='zippylog.device.request_processor.GetInvalidOffset.id', index=0,
      number=1, type=12, cpp_type=9, label=1,
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
  serialized_start=605,
  serialized_end=635,
)


_BEGINPROCESSGET = descriptor.Descriptor(
  name='BeginProcessGet',
  full_name='zippylog.device.request_processor.BeginProcessGet',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    descriptor.FieldDescriptor(
      name='id', full_name='zippylog.device.request_processor.BeginProcessGet.id', index=0,
      number=1, type=12, cpp_type=9, label=1,
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
  serialized_start=637,
  serialized_end=666,
)


_ENDPROCESSGET = descriptor.Descriptor(
  name='EndProcessGet',
  full_name='zippylog.device.request_processor.EndProcessGet',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    descriptor.FieldDescriptor(
      name='id', full_name='zippylog.device.request_processor.EndProcessGet.id', index=0,
      number=1, type=12, cpp_type=9, label=1,
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
  serialized_start=668,
  serialized_end=695,
)


_SENDERRORRESPONSE = descriptor.Descriptor(
  name='SendErrorResponse',
  full_name='zippylog.device.request_processor.SendErrorResponse',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    descriptor.FieldDescriptor(
      name='id', full_name='zippylog.device.request_processor.SendErrorResponse.id', index=0,
      number=1, type=12, cpp_type=9, label=1,
      has_default_value=False, default_value="",
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    descriptor.FieldDescriptor(
      name='message', full_name='zippylog.device.request_processor.SendErrorResponse.message', index=1,
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
  serialized_start=697,
  serialized_end=745,
)


_FORWARDSUBSCRIBEKEEPALIVE = descriptor.Descriptor(
  name='ForwardSubscribeKeepalive',
  full_name='zippylog.device.request_processor.ForwardSubscribeKeepalive',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    descriptor.FieldDescriptor(
      name='id', full_name='zippylog.device.request_processor.ForwardSubscribeKeepalive.id', index=0,
      number=1, type=12, cpp_type=9, label=1,
      has_default_value=False, default_value="",
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    descriptor.FieldDescriptor(
      name='subscription', full_name='zippylog.device.request_processor.ForwardSubscribeKeepalive.subscription', index=1,
      number=2, type=12, cpp_type=9, label=1,
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
  serialized_start=747,
  serialized_end=808,
)



class Create(message.Message):
  __metaclass__ = reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _CREATE
  
  ZIPPYLOG_NAMESPACE = 1
  ZIPPYLOG_ENUMERATION = 60
  # @@protoc_insertion_point(class_scope:zippylog.device.request_processor.Create)

class Destroy(message.Message):
  __metaclass__ = reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _DESTROY
  
  ZIPPYLOG_NAMESPACE = 1
  ZIPPYLOG_ENUMERATION = 61
  # @@protoc_insertion_point(class_scope:zippylog.device.request_processor.Destroy)

class RunStart(message.Message):
  __metaclass__ = reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _RUNSTART
  
  ZIPPYLOG_NAMESPACE = 1
  ZIPPYLOG_ENUMERATION = 62
  # @@protoc_insertion_point(class_scope:zippylog.device.request_processor.RunStart)

class RunStop(message.Message):
  __metaclass__ = reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _RUNSTOP
  
  ZIPPYLOG_NAMESPACE = 1
  ZIPPYLOG_ENUMERATION = 63
  # @@protoc_insertion_point(class_scope:zippylog.device.request_processor.RunStop)

class FailReceiveMessage(message.Message):
  __metaclass__ = reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _FAILRECEIVEMESSAGE
  
  ZIPPYLOG_NAMESPACE = 1
  ZIPPYLOG_ENUMERATION = 64
  # @@protoc_insertion_point(class_scope:zippylog.device.request_processor.FailReceiveMessage)

class ReceiveEmptyMessage(message.Message):
  __metaclass__ = reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _RECEIVEEMPTYMESSAGE
  
  ZIPPYLOG_NAMESPACE = 1
  ZIPPYLOG_ENUMERATION = 65
  # @@protoc_insertion_point(class_scope:zippylog.device.request_processor.ReceiveEmptyMessage)

class UnknownMessageVersion(message.Message):
  __metaclass__ = reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _UNKNOWNMESSAGEVERSION
  
  ZIPPYLOG_NAMESPACE = 1
  ZIPPYLOG_ENUMERATION = 66
  # @@protoc_insertion_point(class_scope:zippylog.device.request_processor.UnknownMessageVersion)

class UnknownRequestType(message.Message):
  __metaclass__ = reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _UNKNOWNREQUESTTYPE
  
  ZIPPYLOG_NAMESPACE = 1
  ZIPPYLOG_ENUMERATION = 67
  # @@protoc_insertion_point(class_scope:zippylog.device.request_processor.UnknownRequestType)

class EnvelopeParseFailure(message.Message):
  __metaclass__ = reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _ENVELOPEPARSEFAILURE
  
  ZIPPYLOG_NAMESPACE = 1
  ZIPPYLOG_ENUMERATION = 68
  # @@protoc_insertion_point(class_scope:zippylog.device.request_processor.EnvelopeParseFailure)

class EmptyEnvelope(message.Message):
  __metaclass__ = reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _EMPTYENVELOPE
  
  ZIPPYLOG_NAMESPACE = 1
  ZIPPYLOG_ENUMERATION = 69
  # @@protoc_insertion_point(class_scope:zippylog.device.request_processor.EmptyEnvelope)

class InvalidMessageEnumeration(message.Message):
  __metaclass__ = reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _INVALIDMESSAGEENUMERATION
  
  ZIPPYLOG_NAMESPACE = 1
  ZIPPYLOG_ENUMERATION = 70
  # @@protoc_insertion_point(class_scope:zippylog.device.request_processor.InvalidMessageEnumeration)

class BeginProcessStoreInfo(message.Message):
  __metaclass__ = reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _BEGINPROCESSSTOREINFO
  
  ZIPPYLOG_NAMESPACE = 1
  ZIPPYLOG_ENUMERATION = 71
  # @@protoc_insertion_point(class_scope:zippylog.device.request_processor.BeginProcessStoreInfo)

class EndProcessStoreInfo(message.Message):
  __metaclass__ = reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _ENDPROCESSSTOREINFO
  
  ZIPPYLOG_NAMESPACE = 1
  ZIPPYLOG_ENUMERATION = 72
  # @@protoc_insertion_point(class_scope:zippylog.device.request_processor.EndProcessStoreInfo)

class ReceiveInvalidGet(message.Message):
  __metaclass__ = reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _RECEIVEINVALIDGET
  
  ZIPPYLOG_NAMESPACE = 1
  ZIPPYLOG_ENUMERATION = 73
  # @@protoc_insertion_point(class_scope:zippylog.device.request_processor.ReceiveInvalidGet)

class GetInvalidStream(message.Message):
  __metaclass__ = reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _GETINVALIDSTREAM
  
  ZIPPYLOG_NAMESPACE = 1
  ZIPPYLOG_ENUMERATION = 74
  # @@protoc_insertion_point(class_scope:zippylog.device.request_processor.GetInvalidStream)

class GetInvalidOffset(message.Message):
  __metaclass__ = reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _GETINVALIDOFFSET
  
  ZIPPYLOG_NAMESPACE = 1
  ZIPPYLOG_ENUMERATION = 75
  # @@protoc_insertion_point(class_scope:zippylog.device.request_processor.GetInvalidOffset)

class BeginProcessGet(message.Message):
  __metaclass__ = reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _BEGINPROCESSGET
  
  ZIPPYLOG_NAMESPACE = 1
  ZIPPYLOG_ENUMERATION = 76
  # @@protoc_insertion_point(class_scope:zippylog.device.request_processor.BeginProcessGet)

class EndProcessGet(message.Message):
  __metaclass__ = reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _ENDPROCESSGET
  
  ZIPPYLOG_NAMESPACE = 1
  ZIPPYLOG_ENUMERATION = 77
  # @@protoc_insertion_point(class_scope:zippylog.device.request_processor.EndProcessGet)

class SendErrorResponse(message.Message):
  __metaclass__ = reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _SENDERRORRESPONSE
  
  ZIPPYLOG_NAMESPACE = 1
  ZIPPYLOG_ENUMERATION = 78
  # @@protoc_insertion_point(class_scope:zippylog.device.request_processor.SendErrorResponse)

class ForwardSubscribeKeepalive(message.Message):
  __metaclass__ = reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _FORWARDSUBSCRIBEKEEPALIVE
  
  ZIPPYLOG_NAMESPACE = 1
  ZIPPYLOG_ENUMERATION = 79
  # @@protoc_insertion_point(class_scope:zippylog.device.request_processor.ForwardSubscribeKeepalive)

register_message(1, 60, 'zippylog.device.request_processor_pb2', 'Create')
register_message(1, 61, 'zippylog.device.request_processor_pb2', 'Destroy')
register_message(1, 62, 'zippylog.device.request_processor_pb2', 'RunStart')
register_message(1, 63, 'zippylog.device.request_processor_pb2', 'RunStop')
register_message(1, 64, 'zippylog.device.request_processor_pb2', 'FailReceiveMessage')
register_message(1, 65, 'zippylog.device.request_processor_pb2', 'ReceiveEmptyMessage')
register_message(1, 66, 'zippylog.device.request_processor_pb2', 'UnknownMessageVersion')
register_message(1, 67, 'zippylog.device.request_processor_pb2', 'UnknownRequestType')
register_message(1, 68, 'zippylog.device.request_processor_pb2', 'EnvelopeParseFailure')
register_message(1, 69, 'zippylog.device.request_processor_pb2', 'EmptyEnvelope')
register_message(1, 70, 'zippylog.device.request_processor_pb2', 'InvalidMessageEnumeration')
register_message(1, 71, 'zippylog.device.request_processor_pb2', 'BeginProcessStoreInfo')
register_message(1, 72, 'zippylog.device.request_processor_pb2', 'EndProcessStoreInfo')
register_message(1, 73, 'zippylog.device.request_processor_pb2', 'ReceiveInvalidGet')
register_message(1, 74, 'zippylog.device.request_processor_pb2', 'GetInvalidStream')
register_message(1, 75, 'zippylog.device.request_processor_pb2', 'GetInvalidOffset')
register_message(1, 76, 'zippylog.device.request_processor_pb2', 'BeginProcessGet')
register_message(1, 77, 'zippylog.device.request_processor_pb2', 'EndProcessGet')
register_message(1, 78, 'zippylog.device.request_processor_pb2', 'SendErrorResponse')
register_message(1, 79, 'zippylog.device.request_processor_pb2', 'ForwardSubscribeKeepalive')
# @@protoc_insertion_point(module_scope)
