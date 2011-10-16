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
  serialized_pb='\n\x1fzippylog/protocol/request.proto\x12\x19zippylog.protocol.request\"\x08\n\x06PingV1\"\x0f\n\rGetFeaturesV1\"\x10\n\x0eGetStoreInfoV1\"\x1f\n\x0fGetBucketInfoV1\x12\x0c\n\x04path\x18\x01 \x02(\t\"\"\n\x12GetStreamSetInfoV1\x12\x0c\n\x04path\x18\x01 \x02(\t\"\x1f\n\x0fGetStreamInfoV1\x12\x0c\n\x04path\x18\x01 \x02(\t\"t\n\x12GetStreamSegmentV1\x12\x0c\n\x04path\x18\x01 \x02(\t\x12\x14\n\x0cstart_offset\x18\x02 \x02(\x04\x12\x1a\n\x12max_response_bytes\x18\x03 \x01(\r\x12\x1e\n\x16max_response_envelopes\x18\x04 \x01(\r\"h\n\x0fWriteEnvelopeV1\x12\x0c\n\x04path\x18\x01 \x02(\t\x12\x10\n\x08\x65nvelope\x18\x02 \x03(\x0c\x12\x19\n\x0b\x61\x63knowledge\x18\x03 \x01(\x08:\x04true\x12\x1a\n\x0bsynchronous\x18\x04 \x01(\x08:\x05\x66\x61lse\"u\n\x1bWriteEnvelopeFromMessagesV1\x12\x0c\n\x04path\x18\x01 \x02(\t\x12\x0f\n\x07message\x18\x02 \x03(\x0c\x12\x1d\n\x11message_namespace\x18\x03 \x03(\rB\x02\x10\x01\x12\x18\n\x0cmessage_type\x18\x04 \x03(\rB\x02\x10\x01\"n\n\x19WriteEnvelopeFromStringV1\x12\x0c\n\x04path\x18\x01 \x02(\t\x12\x0c\n\x04\x64\x61ta\x18\x02 \x03(\t\x12\x19\n\x0b\x61\x63knowledge\x18\x03 \x01(\x08:\x04true\x12\x1a\n\x0bsynchronous\x18\x04 \x01(\x08:\x05\x66\x61lse\"\'\n\x17SubscribeStoreChangesV1\x12\x0c\n\x04path\x18\x01 \x03(\t\"\xa3\x01\n\x14SubscribeEnvelopesV1\x12\x0c\n\x04path\x18\x01 \x03(\t\x12\x10\n\x08lua_code\x18\x05 \x01(\t\x12\x1c\n\x10\x66ilter_namespace\x18\x02 \x03(\rB\x02\x10\x01\x12(\n\x1c\x66ilter_enumeration_namespace\x18\x03 \x03(\rB\x02\x10\x01\x12#\n\x17\x66ilter_enumeration_type\x18\x04 \x03(\rB\x02\x10\x01\"\"\n\x14SubscribeKeepaliveV1\x12\n\n\x02id\x18\x01 \x03(\x0c\"\x1f\n\x11SubscribeCancelV1\x12\n\n\x02id\x18\x01 \x02(\x0c\"2\n\x10RegisterPluginV1\x12\x0c\n\x04name\x18\x01 \x02(\t\x12\x10\n\x08lua_code\x18\x02 \x02(\t\"\"\n\x12UnregisterPluginV1\x12\x0c\n\x04name\x18\x01 \x02(\t\"!\n\x11GetPluginStatusV1\x12\x0c\n\x04name\x18\x01 \x03(\t')




_PINGV1 = descriptor.Descriptor(
  name='PingV1',
  full_name='zippylog.protocol.request.PingV1',
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
  serialized_end=70,
)


_GETFEATURESV1 = descriptor.Descriptor(
  name='GetFeaturesV1',
  full_name='zippylog.protocol.request.GetFeaturesV1',
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
  serialized_start=72,
  serialized_end=87,
)


_GETSTOREINFOV1 = descriptor.Descriptor(
  name='GetStoreInfoV1',
  full_name='zippylog.protocol.request.GetStoreInfoV1',
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
  serialized_start=89,
  serialized_end=105,
)


_GETBUCKETINFOV1 = descriptor.Descriptor(
  name='GetBucketInfoV1',
  full_name='zippylog.protocol.request.GetBucketInfoV1',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    descriptor.FieldDescriptor(
      name='path', full_name='zippylog.protocol.request.GetBucketInfoV1.path', index=0,
      number=1, type=9, cpp_type=9, label=2,
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
  serialized_start=107,
  serialized_end=138,
)


_GETSTREAMSETINFOV1 = descriptor.Descriptor(
  name='GetStreamSetInfoV1',
  full_name='zippylog.protocol.request.GetStreamSetInfoV1',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    descriptor.FieldDescriptor(
      name='path', full_name='zippylog.protocol.request.GetStreamSetInfoV1.path', index=0,
      number=1, type=9, cpp_type=9, label=2,
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
  serialized_start=140,
  serialized_end=174,
)


_GETSTREAMINFOV1 = descriptor.Descriptor(
  name='GetStreamInfoV1',
  full_name='zippylog.protocol.request.GetStreamInfoV1',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    descriptor.FieldDescriptor(
      name='path', full_name='zippylog.protocol.request.GetStreamInfoV1.path', index=0,
      number=1, type=9, cpp_type=9, label=2,
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
  serialized_start=176,
  serialized_end=207,
)


_GETSTREAMSEGMENTV1 = descriptor.Descriptor(
  name='GetStreamSegmentV1',
  full_name='zippylog.protocol.request.GetStreamSegmentV1',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    descriptor.FieldDescriptor(
      name='path', full_name='zippylog.protocol.request.GetStreamSegmentV1.path', index=0,
      number=1, type=9, cpp_type=9, label=2,
      has_default_value=False, default_value=unicode("", "utf-8"),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    descriptor.FieldDescriptor(
      name='start_offset', full_name='zippylog.protocol.request.GetStreamSegmentV1.start_offset', index=1,
      number=2, type=4, cpp_type=4, label=2,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    descriptor.FieldDescriptor(
      name='max_response_bytes', full_name='zippylog.protocol.request.GetStreamSegmentV1.max_response_bytes', index=2,
      number=3, type=13, cpp_type=3, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    descriptor.FieldDescriptor(
      name='max_response_envelopes', full_name='zippylog.protocol.request.GetStreamSegmentV1.max_response_envelopes', index=3,
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
  serialized_start=209,
  serialized_end=325,
)


_WRITEENVELOPEV1 = descriptor.Descriptor(
  name='WriteEnvelopeV1',
  full_name='zippylog.protocol.request.WriteEnvelopeV1',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    descriptor.FieldDescriptor(
      name='path', full_name='zippylog.protocol.request.WriteEnvelopeV1.path', index=0,
      number=1, type=9, cpp_type=9, label=2,
      has_default_value=False, default_value=unicode("", "utf-8"),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    descriptor.FieldDescriptor(
      name='envelope', full_name='zippylog.protocol.request.WriteEnvelopeV1.envelope', index=1,
      number=2, type=12, cpp_type=9, label=3,
      has_default_value=False, default_value=[],
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    descriptor.FieldDescriptor(
      name='acknowledge', full_name='zippylog.protocol.request.WriteEnvelopeV1.acknowledge', index=2,
      number=3, type=8, cpp_type=7, label=1,
      has_default_value=True, default_value=True,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    descriptor.FieldDescriptor(
      name='synchronous', full_name='zippylog.protocol.request.WriteEnvelopeV1.synchronous', index=3,
      number=4, type=8, cpp_type=7, label=1,
      has_default_value=True, default_value=False,
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
  serialized_start=327,
  serialized_end=431,
)


_WRITEENVELOPEFROMMESSAGESV1 = descriptor.Descriptor(
  name='WriteEnvelopeFromMessagesV1',
  full_name='zippylog.protocol.request.WriteEnvelopeFromMessagesV1',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    descriptor.FieldDescriptor(
      name='path', full_name='zippylog.protocol.request.WriteEnvelopeFromMessagesV1.path', index=0,
      number=1, type=9, cpp_type=9, label=2,
      has_default_value=False, default_value=unicode("", "utf-8"),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    descriptor.FieldDescriptor(
      name='message', full_name='zippylog.protocol.request.WriteEnvelopeFromMessagesV1.message', index=1,
      number=2, type=12, cpp_type=9, label=3,
      has_default_value=False, default_value=[],
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    descriptor.FieldDescriptor(
      name='message_namespace', full_name='zippylog.protocol.request.WriteEnvelopeFromMessagesV1.message_namespace', index=2,
      number=3, type=13, cpp_type=3, label=3,
      has_default_value=False, default_value=[],
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=descriptor._ParseOptions(descriptor_pb2.FieldOptions(), '\020\001')),
    descriptor.FieldDescriptor(
      name='message_type', full_name='zippylog.protocol.request.WriteEnvelopeFromMessagesV1.message_type', index=3,
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
  serialized_start=433,
  serialized_end=550,
)


_WRITEENVELOPEFROMSTRINGV1 = descriptor.Descriptor(
  name='WriteEnvelopeFromStringV1',
  full_name='zippylog.protocol.request.WriteEnvelopeFromStringV1',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    descriptor.FieldDescriptor(
      name='path', full_name='zippylog.protocol.request.WriteEnvelopeFromStringV1.path', index=0,
      number=1, type=9, cpp_type=9, label=2,
      has_default_value=False, default_value=unicode("", "utf-8"),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    descriptor.FieldDescriptor(
      name='data', full_name='zippylog.protocol.request.WriteEnvelopeFromStringV1.data', index=1,
      number=2, type=9, cpp_type=9, label=3,
      has_default_value=False, default_value=[],
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    descriptor.FieldDescriptor(
      name='acknowledge', full_name='zippylog.protocol.request.WriteEnvelopeFromStringV1.acknowledge', index=2,
      number=3, type=8, cpp_type=7, label=1,
      has_default_value=True, default_value=True,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    descriptor.FieldDescriptor(
      name='synchronous', full_name='zippylog.protocol.request.WriteEnvelopeFromStringV1.synchronous', index=3,
      number=4, type=8, cpp_type=7, label=1,
      has_default_value=True, default_value=False,
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
  serialized_start=552,
  serialized_end=662,
)


_SUBSCRIBESTORECHANGESV1 = descriptor.Descriptor(
  name='SubscribeStoreChangesV1',
  full_name='zippylog.protocol.request.SubscribeStoreChangesV1',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    descriptor.FieldDescriptor(
      name='path', full_name='zippylog.protocol.request.SubscribeStoreChangesV1.path', index=0,
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
  serialized_start=664,
  serialized_end=703,
)


_SUBSCRIBEENVELOPESV1 = descriptor.Descriptor(
  name='SubscribeEnvelopesV1',
  full_name='zippylog.protocol.request.SubscribeEnvelopesV1',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    descriptor.FieldDescriptor(
      name='path', full_name='zippylog.protocol.request.SubscribeEnvelopesV1.path', index=0,
      number=1, type=9, cpp_type=9, label=3,
      has_default_value=False, default_value=[],
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    descriptor.FieldDescriptor(
      name='lua_code', full_name='zippylog.protocol.request.SubscribeEnvelopesV1.lua_code', index=1,
      number=5, type=9, cpp_type=9, label=1,
      has_default_value=False, default_value=unicode("", "utf-8"),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    descriptor.FieldDescriptor(
      name='filter_namespace', full_name='zippylog.protocol.request.SubscribeEnvelopesV1.filter_namespace', index=2,
      number=2, type=13, cpp_type=3, label=3,
      has_default_value=False, default_value=[],
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=descriptor._ParseOptions(descriptor_pb2.FieldOptions(), '\020\001')),
    descriptor.FieldDescriptor(
      name='filter_enumeration_namespace', full_name='zippylog.protocol.request.SubscribeEnvelopesV1.filter_enumeration_namespace', index=3,
      number=3, type=13, cpp_type=3, label=3,
      has_default_value=False, default_value=[],
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=descriptor._ParseOptions(descriptor_pb2.FieldOptions(), '\020\001')),
    descriptor.FieldDescriptor(
      name='filter_enumeration_type', full_name='zippylog.protocol.request.SubscribeEnvelopesV1.filter_enumeration_type', index=4,
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
  serialized_start=706,
  serialized_end=869,
)


_SUBSCRIBEKEEPALIVEV1 = descriptor.Descriptor(
  name='SubscribeKeepaliveV1',
  full_name='zippylog.protocol.request.SubscribeKeepaliveV1',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    descriptor.FieldDescriptor(
      name='id', full_name='zippylog.protocol.request.SubscribeKeepaliveV1.id', index=0,
      number=1, type=12, cpp_type=9, label=3,
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
  serialized_start=871,
  serialized_end=905,
)


_SUBSCRIBECANCELV1 = descriptor.Descriptor(
  name='SubscribeCancelV1',
  full_name='zippylog.protocol.request.SubscribeCancelV1',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    descriptor.FieldDescriptor(
      name='id', full_name='zippylog.protocol.request.SubscribeCancelV1.id', index=0,
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
  serialized_start=907,
  serialized_end=938,
)


_REGISTERPLUGINV1 = descriptor.Descriptor(
  name='RegisterPluginV1',
  full_name='zippylog.protocol.request.RegisterPluginV1',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    descriptor.FieldDescriptor(
      name='name', full_name='zippylog.protocol.request.RegisterPluginV1.name', index=0,
      number=1, type=9, cpp_type=9, label=2,
      has_default_value=False, default_value=unicode("", "utf-8"),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    descriptor.FieldDescriptor(
      name='lua_code', full_name='zippylog.protocol.request.RegisterPluginV1.lua_code', index=1,
      number=2, type=9, cpp_type=9, label=2,
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
  serialized_start=940,
  serialized_end=990,
)


_UNREGISTERPLUGINV1 = descriptor.Descriptor(
  name='UnregisterPluginV1',
  full_name='zippylog.protocol.request.UnregisterPluginV1',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    descriptor.FieldDescriptor(
      name='name', full_name='zippylog.protocol.request.UnregisterPluginV1.name', index=0,
      number=1, type=9, cpp_type=9, label=2,
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
  serialized_start=992,
  serialized_end=1026,
)


_GETPLUGINSTATUSV1 = descriptor.Descriptor(
  name='GetPluginStatusV1',
  full_name='zippylog.protocol.request.GetPluginStatusV1',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    descriptor.FieldDescriptor(
      name='name', full_name='zippylog.protocol.request.GetPluginStatusV1.name', index=0,
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
  serialized_start=1028,
  serialized_end=1061,
)

DESCRIPTOR.message_types_by_name['PingV1'] = _PINGV1
DESCRIPTOR.message_types_by_name['GetFeaturesV1'] = _GETFEATURESV1
DESCRIPTOR.message_types_by_name['GetStoreInfoV1'] = _GETSTOREINFOV1
DESCRIPTOR.message_types_by_name['GetBucketInfoV1'] = _GETBUCKETINFOV1
DESCRIPTOR.message_types_by_name['GetStreamSetInfoV1'] = _GETSTREAMSETINFOV1
DESCRIPTOR.message_types_by_name['GetStreamInfoV1'] = _GETSTREAMINFOV1
DESCRIPTOR.message_types_by_name['GetStreamSegmentV1'] = _GETSTREAMSEGMENTV1
DESCRIPTOR.message_types_by_name['WriteEnvelopeV1'] = _WRITEENVELOPEV1
DESCRIPTOR.message_types_by_name['WriteEnvelopeFromMessagesV1'] = _WRITEENVELOPEFROMMESSAGESV1
DESCRIPTOR.message_types_by_name['WriteEnvelopeFromStringV1'] = _WRITEENVELOPEFROMSTRINGV1
DESCRIPTOR.message_types_by_name['SubscribeStoreChangesV1'] = _SUBSCRIBESTORECHANGESV1
DESCRIPTOR.message_types_by_name['SubscribeEnvelopesV1'] = _SUBSCRIBEENVELOPESV1
DESCRIPTOR.message_types_by_name['SubscribeKeepaliveV1'] = _SUBSCRIBEKEEPALIVEV1
DESCRIPTOR.message_types_by_name['SubscribeCancelV1'] = _SUBSCRIBECANCELV1
DESCRIPTOR.message_types_by_name['RegisterPluginV1'] = _REGISTERPLUGINV1
DESCRIPTOR.message_types_by_name['UnregisterPluginV1'] = _UNREGISTERPLUGINV1
DESCRIPTOR.message_types_by_name['GetPluginStatusV1'] = _GETPLUGINSTATUSV1

class PingV1(message.Message):
  __metaclass__ = reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _PINGV1
  
  ZIPPYLOG_NAMESPACE = 1
  ZIPPYLOG_ENUMERATION = 91
  # @@protoc_insertion_point(class_scope:zippylog.protocol.request.PingV1)

class GetFeaturesV1(message.Message):
  __metaclass__ = reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _GETFEATURESV1
  
  ZIPPYLOG_NAMESPACE = 1
  ZIPPYLOG_ENUMERATION = 72
  # @@protoc_insertion_point(class_scope:zippylog.protocol.request.GetFeaturesV1)

class GetStoreInfoV1(message.Message):
  __metaclass__ = reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _GETSTOREINFOV1
  
  ZIPPYLOG_NAMESPACE = 1
  ZIPPYLOG_ENUMERATION = 73
  # @@protoc_insertion_point(class_scope:zippylog.protocol.request.GetStoreInfoV1)

class GetBucketInfoV1(message.Message):
  __metaclass__ = reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _GETBUCKETINFOV1
  
  ZIPPYLOG_NAMESPACE = 1
  ZIPPYLOG_ENUMERATION = 74
  # @@protoc_insertion_point(class_scope:zippylog.protocol.request.GetBucketInfoV1)

class GetStreamSetInfoV1(message.Message):
  __metaclass__ = reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _GETSTREAMSETINFOV1
  
  ZIPPYLOG_NAMESPACE = 1
  ZIPPYLOG_ENUMERATION = 75
  # @@protoc_insertion_point(class_scope:zippylog.protocol.request.GetStreamSetInfoV1)

class GetStreamInfoV1(message.Message):
  __metaclass__ = reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _GETSTREAMINFOV1
  
  ZIPPYLOG_NAMESPACE = 1
  ZIPPYLOG_ENUMERATION = 76
  # @@protoc_insertion_point(class_scope:zippylog.protocol.request.GetStreamInfoV1)

class GetStreamSegmentV1(message.Message):
  __metaclass__ = reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _GETSTREAMSEGMENTV1
  
  ZIPPYLOG_NAMESPACE = 1
  ZIPPYLOG_ENUMERATION = 92
  # @@protoc_insertion_point(class_scope:zippylog.protocol.request.GetStreamSegmentV1)

class WriteEnvelopeV1(message.Message):
  __metaclass__ = reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _WRITEENVELOPEV1
  
  ZIPPYLOG_NAMESPACE = 1
  ZIPPYLOG_ENUMERATION = 78
  # @@protoc_insertion_point(class_scope:zippylog.protocol.request.WriteEnvelopeV1)

class WriteEnvelopeFromMessagesV1(message.Message):
  __metaclass__ = reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _WRITEENVELOPEFROMMESSAGESV1
  
  ZIPPYLOG_NAMESPACE = 1
  ZIPPYLOG_ENUMERATION = 79
  # @@protoc_insertion_point(class_scope:zippylog.protocol.request.WriteEnvelopeFromMessagesV1)

class WriteEnvelopeFromStringV1(message.Message):
  __metaclass__ = reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _WRITEENVELOPEFROMSTRINGV1
  
  ZIPPYLOG_NAMESPACE = 1
  ZIPPYLOG_ENUMERATION = 131
  # @@protoc_insertion_point(class_scope:zippylog.protocol.request.WriteEnvelopeFromStringV1)

class SubscribeStoreChangesV1(message.Message):
  __metaclass__ = reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _SUBSCRIBESTORECHANGESV1
  
  ZIPPYLOG_NAMESPACE = 1
  ZIPPYLOG_ENUMERATION = 80
  # @@protoc_insertion_point(class_scope:zippylog.protocol.request.SubscribeStoreChangesV1)

class SubscribeEnvelopesV1(message.Message):
  __metaclass__ = reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _SUBSCRIBEENVELOPESV1
  
  ZIPPYLOG_NAMESPACE = 1
  ZIPPYLOG_ENUMERATION = 81
  # @@protoc_insertion_point(class_scope:zippylog.protocol.request.SubscribeEnvelopesV1)

class SubscribeKeepaliveV1(message.Message):
  __metaclass__ = reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _SUBSCRIBEKEEPALIVEV1
  
  ZIPPYLOG_NAMESPACE = 1
  ZIPPYLOG_ENUMERATION = 82
  # @@protoc_insertion_point(class_scope:zippylog.protocol.request.SubscribeKeepaliveV1)

class SubscribeCancelV1(message.Message):
  __metaclass__ = reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _SUBSCRIBECANCELV1
  
  ZIPPYLOG_NAMESPACE = 1
  ZIPPYLOG_ENUMERATION = 83
  # @@protoc_insertion_point(class_scope:zippylog.protocol.request.SubscribeCancelV1)

class RegisterPluginV1(message.Message):
  __metaclass__ = reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _REGISTERPLUGINV1
  
  ZIPPYLOG_NAMESPACE = 1
  ZIPPYLOG_ENUMERATION = 111
  # @@protoc_insertion_point(class_scope:zippylog.protocol.request.RegisterPluginV1)

class UnregisterPluginV1(message.Message):
  __metaclass__ = reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _UNREGISTERPLUGINV1
  
  ZIPPYLOG_NAMESPACE = 1
  ZIPPYLOG_ENUMERATION = 112
  # @@protoc_insertion_point(class_scope:zippylog.protocol.request.UnregisterPluginV1)

class GetPluginStatusV1(message.Message):
  __metaclass__ = reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _GETPLUGINSTATUSV1
  
  ZIPPYLOG_NAMESPACE = 1
  ZIPPYLOG_ENUMERATION = 116
  # @@protoc_insertion_point(class_scope:zippylog.protocol.request.GetPluginStatusV1)

register_message(1, 91, 'zippylog.protocol.request_pb2', 'PingV1')
register_message(1, 72, 'zippylog.protocol.request_pb2', 'GetFeaturesV1')
register_message(1, 73, 'zippylog.protocol.request_pb2', 'GetStoreInfoV1')
register_message(1, 74, 'zippylog.protocol.request_pb2', 'GetBucketInfoV1')
register_message(1, 75, 'zippylog.protocol.request_pb2', 'GetStreamSetInfoV1')
register_message(1, 76, 'zippylog.protocol.request_pb2', 'GetStreamInfoV1')
register_message(1, 92, 'zippylog.protocol.request_pb2', 'GetStreamSegmentV1')
register_message(1, 78, 'zippylog.protocol.request_pb2', 'WriteEnvelopeV1')
register_message(1, 79, 'zippylog.protocol.request_pb2', 'WriteEnvelopeFromMessagesV1')
register_message(1, 131, 'zippylog.protocol.request_pb2', 'WriteEnvelopeFromStringV1')
register_message(1, 80, 'zippylog.protocol.request_pb2', 'SubscribeStoreChangesV1')
register_message(1, 81, 'zippylog.protocol.request_pb2', 'SubscribeEnvelopesV1')
register_message(1, 82, 'zippylog.protocol.request_pb2', 'SubscribeKeepaliveV1')
register_message(1, 83, 'zippylog.protocol.request_pb2', 'SubscribeCancelV1')
register_message(1, 111, 'zippylog.protocol.request_pb2', 'RegisterPluginV1')
register_message(1, 112, 'zippylog.protocol.request_pb2', 'UnregisterPluginV1')
register_message(1, 116, 'zippylog.protocol.request_pb2', 'GetPluginStatusV1')
# @@protoc_insertion_point(module_scope)
