# Generated by the protocol buffer compiler.  DO NOT EDIT!

from google.protobuf import descriptor
from google.protobuf import message
from google.protobuf import reflection
from google.protobuf import descriptor_pb2
from zippylog.envelope import register_message
# @@protoc_insertion_point(imports)


DESCRIPTOR = descriptor.FileDescriptor(
  name='zippylog/protocol/request.proto',
  package='zippylog.protocol.request',
  serialized_pb='\n\x1fzippylog/protocol/request.proto\x12\x19zippylog.protocol.request\"\r\n\x0bGetFeatures\"\"\n\x0cGetStoreInfo\x12\x12\n\x07version\x18\x01 \x01(\r:\x01\x31\"3\n\rGetBucketInfo\x12\x12\n\x07version\x18\x01 \x01(\r:\x01\x31\x12\x0e\n\x06\x62ucket\x18\x02 \x01(\t\"4\n\x10GetStreamSetInfo\x12\x12\n\x07version\x18\x01 \x01(\r:\x01\x31\x12\x0c\n\x04path\x18\x02 \x01(\t\"1\n\rGetStreamInfo\x12\x12\n\x07version\x18\x01 \x01(\r:\x01\x31\x12\x0c\n\x04path\x18\x02 \x01(\t\"E\n\x03Get\x12\x0c\n\x04path\x18\x01 \x01(\t\x12\x14\n\x0cstart_offset\x18\x02 \x01(\x04\x12\x1a\n\x12max_response_bytes\x18\x03 \x01(\r\"C\n\rWriteEnvelope\x12\x12\n\x07version\x18\x01 \x01(\r:\x01\x31\x12\x0c\n\x04path\x18\x02 \x01(\t\x12\x10\n\x08\x65nvelope\x18\x03 \x03(\x0c\"s\n\x19WriteEnvelopeWithMessages\x12\x0c\n\x04path\x18\x01 \x01(\t\x12\x0f\n\x07message\x18\x02 \x03(\x0c\x12\x1d\n\x11message_namespace\x18\x03 \x03(\rB\x02\x10\x01\x12\x18\n\x0cmessage_type\x18\x04 \x03(\rB\x02\x10\x01\"%\n\x15SubscribeStoreChanges\x12\x0c\n\x04path\x18\x01 \x03(\t\"4\n\x12SubscribeEnvelopes\x12\x0c\n\x04path\x18\x01 \x03(\t\x12\x10\n\x08lua_code\x18\x02 \x01(\t\" \n\x12SubscribeKeepalive\x12\n\n\x02id\x18\x01 \x01(\x0c\"\x1d\n\x0fSubscribeCancel\x12\n\n\x02id\x18\x01 \x01(\x0c')




_GETFEATURES = descriptor.Descriptor(
  name='GetFeatures',
  full_name='zippylog.protocol.request.GetFeatures',
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
  serialized_start=62,
  serialized_end=75,
)


_GETSTOREINFO = descriptor.Descriptor(
  name='GetStoreInfo',
  full_name='zippylog.protocol.request.GetStoreInfo',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    descriptor.FieldDescriptor(
      name='version', full_name='zippylog.protocol.request.GetStoreInfo.version', index=0,
      number=1, type=13, cpp_type=3, label=1,
      has_default_value=True, default_value=1,
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
  serialized_start=77,
  serialized_end=111,
)


_GETBUCKETINFO = descriptor.Descriptor(
  name='GetBucketInfo',
  full_name='zippylog.protocol.request.GetBucketInfo',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    descriptor.FieldDescriptor(
      name='version', full_name='zippylog.protocol.request.GetBucketInfo.version', index=0,
      number=1, type=13, cpp_type=3, label=1,
      has_default_value=True, default_value=1,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    descriptor.FieldDescriptor(
      name='bucket', full_name='zippylog.protocol.request.GetBucketInfo.bucket', index=1,
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
  serialized_start=113,
  serialized_end=164,
)


_GETSTREAMSETINFO = descriptor.Descriptor(
  name='GetStreamSetInfo',
  full_name='zippylog.protocol.request.GetStreamSetInfo',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    descriptor.FieldDescriptor(
      name='version', full_name='zippylog.protocol.request.GetStreamSetInfo.version', index=0,
      number=1, type=13, cpp_type=3, label=1,
      has_default_value=True, default_value=1,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    descriptor.FieldDescriptor(
      name='path', full_name='zippylog.protocol.request.GetStreamSetInfo.path', index=1,
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
  serialized_start=166,
  serialized_end=218,
)


_GETSTREAMINFO = descriptor.Descriptor(
  name='GetStreamInfo',
  full_name='zippylog.protocol.request.GetStreamInfo',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    descriptor.FieldDescriptor(
      name='version', full_name='zippylog.protocol.request.GetStreamInfo.version', index=0,
      number=1, type=13, cpp_type=3, label=1,
      has_default_value=True, default_value=1,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    descriptor.FieldDescriptor(
      name='path', full_name='zippylog.protocol.request.GetStreamInfo.path', index=1,
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
  serialized_start=220,
  serialized_end=269,
)


_GET = descriptor.Descriptor(
  name='Get',
  full_name='zippylog.protocol.request.Get',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    descriptor.FieldDescriptor(
      name='path', full_name='zippylog.protocol.request.Get.path', index=0,
      number=1, type=9, cpp_type=9, label=1,
      has_default_value=False, default_value=unicode("", "utf-8"),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    descriptor.FieldDescriptor(
      name='start_offset', full_name='zippylog.protocol.request.Get.start_offset', index=1,
      number=2, type=4, cpp_type=4, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    descriptor.FieldDescriptor(
      name='max_response_bytes', full_name='zippylog.protocol.request.Get.max_response_bytes', index=2,
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
  serialized_start=271,
  serialized_end=340,
)


_WRITEENVELOPE = descriptor.Descriptor(
  name='WriteEnvelope',
  full_name='zippylog.protocol.request.WriteEnvelope',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    descriptor.FieldDescriptor(
      name='version', full_name='zippylog.protocol.request.WriteEnvelope.version', index=0,
      number=1, type=13, cpp_type=3, label=1,
      has_default_value=True, default_value=1,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    descriptor.FieldDescriptor(
      name='path', full_name='zippylog.protocol.request.WriteEnvelope.path', index=1,
      number=2, type=9, cpp_type=9, label=1,
      has_default_value=False, default_value=unicode("", "utf-8"),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    descriptor.FieldDescriptor(
      name='envelope', full_name='zippylog.protocol.request.WriteEnvelope.envelope', index=2,
      number=3, type=12, cpp_type=9, label=3,
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
  serialized_start=342,
  serialized_end=409,
)


_WRITEENVELOPEWITHMESSAGES = descriptor.Descriptor(
  name='WriteEnvelopeWithMessages',
  full_name='zippylog.protocol.request.WriteEnvelopeWithMessages',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    descriptor.FieldDescriptor(
      name='path', full_name='zippylog.protocol.request.WriteEnvelopeWithMessages.path', index=0,
      number=1, type=9, cpp_type=9, label=1,
      has_default_value=False, default_value=unicode("", "utf-8"),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    descriptor.FieldDescriptor(
      name='message', full_name='zippylog.protocol.request.WriteEnvelopeWithMessages.message', index=1,
      number=2, type=12, cpp_type=9, label=3,
      has_default_value=False, default_value=[],
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    descriptor.FieldDescriptor(
      name='message_namespace', full_name='zippylog.protocol.request.WriteEnvelopeWithMessages.message_namespace', index=2,
      number=3, type=13, cpp_type=3, label=3,
      has_default_value=False, default_value=[],
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=descriptor._ParseOptions(descriptor_pb2.FieldOptions(), '\020\001')),
    descriptor.FieldDescriptor(
      name='message_type', full_name='zippylog.protocol.request.WriteEnvelopeWithMessages.message_type', index=3,
      number=4, type=13, cpp_type=3, label=3,
      has_default_value=False, default_value=[],
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=descriptor._ParseOptions(descriptor_pb2.FieldOptions(), '\020\001')),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  options=None,
  is_extendable=False,
  extension_ranges=[],
  serialized_start=411,
  serialized_end=526,
)


_SUBSCRIBESTORECHANGES = descriptor.Descriptor(
  name='SubscribeStoreChanges',
  full_name='zippylog.protocol.request.SubscribeStoreChanges',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    descriptor.FieldDescriptor(
      name='path', full_name='zippylog.protocol.request.SubscribeStoreChanges.path', index=0,
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
  serialized_start=528,
  serialized_end=565,
)


_SUBSCRIBEENVELOPES = descriptor.Descriptor(
  name='SubscribeEnvelopes',
  full_name='zippylog.protocol.request.SubscribeEnvelopes',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    descriptor.FieldDescriptor(
      name='path', full_name='zippylog.protocol.request.SubscribeEnvelopes.path', index=0,
      number=1, type=9, cpp_type=9, label=3,
      has_default_value=False, default_value=[],
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    descriptor.FieldDescriptor(
      name='lua_code', full_name='zippylog.protocol.request.SubscribeEnvelopes.lua_code', index=1,
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
  serialized_start=567,
  serialized_end=619,
)


_SUBSCRIBEKEEPALIVE = descriptor.Descriptor(
  name='SubscribeKeepalive',
  full_name='zippylog.protocol.request.SubscribeKeepalive',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    descriptor.FieldDescriptor(
      name='id', full_name='zippylog.protocol.request.SubscribeKeepalive.id', index=0,
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
  serialized_start=621,
  serialized_end=653,
)


_SUBSCRIBECANCEL = descriptor.Descriptor(
  name='SubscribeCancel',
  full_name='zippylog.protocol.request.SubscribeCancel',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    descriptor.FieldDescriptor(
      name='id', full_name='zippylog.protocol.request.SubscribeCancel.id', index=0,
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
  serialized_start=655,
  serialized_end=684,
)



class GetFeatures(message.Message):
  __metaclass__ = reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _GETFEATURES
  
  ZIPPYLOG_NAMESPACE = 1
  ZIPPYLOG_ENUMERATION = 56
  # @@protoc_insertion_point(class_scope:zippylog.protocol.request.GetFeatures)

class GetStoreInfo(message.Message):
  __metaclass__ = reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _GETSTOREINFO
  
  ZIPPYLOG_NAMESPACE = 1
  ZIPPYLOG_ENUMERATION = 57
  # @@protoc_insertion_point(class_scope:zippylog.protocol.request.GetStoreInfo)

class GetBucketInfo(message.Message):
  __metaclass__ = reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _GETBUCKETINFO
  
  ZIPPYLOG_NAMESPACE = 1
  ZIPPYLOG_ENUMERATION = 100
  # @@protoc_insertion_point(class_scope:zippylog.protocol.request.GetBucketInfo)

class GetStreamSetInfo(message.Message):
  __metaclass__ = reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _GETSTREAMSETINFO
  
  ZIPPYLOG_NAMESPACE = 1
  ZIPPYLOG_ENUMERATION = 101
  # @@protoc_insertion_point(class_scope:zippylog.protocol.request.GetStreamSetInfo)

class GetStreamInfo(message.Message):
  __metaclass__ = reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _GETSTREAMINFO
  
  ZIPPYLOG_NAMESPACE = 1
  ZIPPYLOG_ENUMERATION = 102
  # @@protoc_insertion_point(class_scope:zippylog.protocol.request.GetStreamInfo)

class Get(message.Message):
  __metaclass__ = reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _GET
  
  ZIPPYLOG_NAMESPACE = 1
  ZIPPYLOG_ENUMERATION = 11
  # @@protoc_insertion_point(class_scope:zippylog.protocol.request.Get)

class WriteEnvelope(message.Message):
  __metaclass__ = reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _WRITEENVELOPE
  
  ZIPPYLOG_NAMESPACE = 1
  ZIPPYLOG_ENUMERATION = 58
  # @@protoc_insertion_point(class_scope:zippylog.protocol.request.WriteEnvelope)

class WriteEnvelopeWithMessages(message.Message):
  __metaclass__ = reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _WRITEENVELOPEWITHMESSAGES
  
  ZIPPYLOG_NAMESPACE = 1
  ZIPPYLOG_ENUMERATION = 59
  # @@protoc_insertion_point(class_scope:zippylog.protocol.request.WriteEnvelopeWithMessages)

class SubscribeStoreChanges(message.Message):
  __metaclass__ = reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _SUBSCRIBESTORECHANGES
  
  ZIPPYLOG_NAMESPACE = 1
  ZIPPYLOG_ENUMERATION = 22
  # @@protoc_insertion_point(class_scope:zippylog.protocol.request.SubscribeStoreChanges)

class SubscribeEnvelopes(message.Message):
  __metaclass__ = reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _SUBSCRIBEENVELOPES
  
  ZIPPYLOG_NAMESPACE = 1
  ZIPPYLOG_ENUMERATION = 54
  # @@protoc_insertion_point(class_scope:zippylog.protocol.request.SubscribeEnvelopes)

class SubscribeKeepalive(message.Message):
  __metaclass__ = reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _SUBSCRIBEKEEPALIVE
  
  ZIPPYLOG_NAMESPACE = 1
  ZIPPYLOG_ENUMERATION = 23
  # @@protoc_insertion_point(class_scope:zippylog.protocol.request.SubscribeKeepalive)

class SubscribeCancel(message.Message):
  __metaclass__ = reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _SUBSCRIBECANCEL
  
  ZIPPYLOG_NAMESPACE = 1
  ZIPPYLOG_ENUMERATION = 25
  # @@protoc_insertion_point(class_scope:zippylog.protocol.request.SubscribeCancel)

register_message(1, 56, 'zippylog.protocol.request_pb2', 'GetFeatures')
register_message(1, 57, 'zippylog.protocol.request_pb2', 'GetStoreInfo')
register_message(1, 100, 'zippylog.protocol.request_pb2', 'GetBucketInfo')
register_message(1, 101, 'zippylog.protocol.request_pb2', 'GetStreamSetInfo')
register_message(1, 102, 'zippylog.protocol.request_pb2', 'GetStreamInfo')
register_message(1, 11, 'zippylog.protocol.request_pb2', 'Get')
register_message(1, 58, 'zippylog.protocol.request_pb2', 'WriteEnvelope')
register_message(1, 59, 'zippylog.protocol.request_pb2', 'WriteEnvelopeWithMessages')
register_message(1, 22, 'zippylog.protocol.request_pb2', 'SubscribeStoreChanges')
register_message(1, 54, 'zippylog.protocol.request_pb2', 'SubscribeEnvelopes')
register_message(1, 23, 'zippylog.protocol.request_pb2', 'SubscribeKeepalive')
register_message(1, 25, 'zippylog.protocol.request_pb2', 'SubscribeCancel')
# @@protoc_insertion_point(module_scope)
