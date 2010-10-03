# Generated by the protocol buffer compiler.  DO NOT EDIT!

from google.protobuf import descriptor
from google.protobuf import message
from google.protobuf import reflection
from google.protobuf import descriptor_pb2
from zippylog.envelope import register_message
# @@protoc_insertion_point(imports)


DESCRIPTOR = descriptor.FileDescriptor(
  name='zippylog/zippylogd.proto',
  package='zippylog.zippylogd',
  serialized_pb='\n\x18zippylog/zippylogd.proto\x12\x12zippylog.zippylogd\"=\n\rBrokerStartup\x12\x12\n\nstore_path\x18\x01 \x01(\t\x12\x18\n\x10listen_endpoints\x18\x02 \x03(\t\"\x10\n\x0e\x42rokerShutdown\"\x1c\n\x1a\x42rokerReceiveClientMessage\"\x1a\n\x18WorkerFailReceiveMessage\"\x1b\n\x19WorkerReceiveEmptyMessage\"\x1b\n\x19WorkerRequestParseFailure\"\x1c\n\x1aWorkerRequestEmptyEnvelope\"!\n\x1fWorkerInvalidMessageEnumeration\"\x1d\n\x1bWorkerBeginProcessStoreInfo\"\x1b\n\x19WorkerEndProcessStoreInfo\"\x19\n\x17WorkerReceiveInvalidGet\"\x18\n\x16WorkerGetInvalidStream\"\x18\n\x16WorkerGetInvalidOffset\"\x17\n\x15WorkerBeginProcessGet\"\x15\n\x13WorkerEndProcessGet\"*\n\x17WorkerSendErrorResponse\x12\x0f\n\x07message\x18\x01 \x01(\t\")\n\x1bStreamerSubscriptionExpired\x12\n\n\x02id\x18\x01 \x01(\x0c\"&\n\x18StreamerReceiveKeepalive\x12\n\n\x02id\x18\x01 \x01(\x0c\"6\n(StreamerSubscriptionRenewedFromKeepalive\x12\n\n\x02id\x18\x01 \x01(\x0c\"8\n*StreamerRejectKeepaliveUnknownSubscription\x12\n\n\x02id\x18\x01 \x01(\x0c')




_BROKERSTARTUP = descriptor.Descriptor(
  name='BrokerStartup',
  full_name='zippylog.zippylogd.BrokerStartup',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    descriptor.FieldDescriptor(
      name='store_path', full_name='zippylog.zippylogd.BrokerStartup.store_path', index=0,
      number=1, type=9, cpp_type=9, label=1,
      has_default_value=False, default_value=unicode("", "utf-8"),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    descriptor.FieldDescriptor(
      name='listen_endpoints', full_name='zippylog.zippylogd.BrokerStartup.listen_endpoints', index=1,
      number=2, type=9, cpp_type=9, label=3,
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
  serialized_start=48,
  serialized_end=109,
)


_BROKERSHUTDOWN = descriptor.Descriptor(
  name='BrokerShutdown',
  full_name='zippylog.zippylogd.BrokerShutdown',
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
  serialized_start=111,
  serialized_end=127,
)


_BROKERRECEIVECLIENTMESSAGE = descriptor.Descriptor(
  name='BrokerReceiveClientMessage',
  full_name='zippylog.zippylogd.BrokerReceiveClientMessage',
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
  serialized_start=129,
  serialized_end=157,
)


_WORKERFAILRECEIVEMESSAGE = descriptor.Descriptor(
  name='WorkerFailReceiveMessage',
  full_name='zippylog.zippylogd.WorkerFailReceiveMessage',
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
  serialized_start=159,
  serialized_end=185,
)


_WORKERRECEIVEEMPTYMESSAGE = descriptor.Descriptor(
  name='WorkerReceiveEmptyMessage',
  full_name='zippylog.zippylogd.WorkerReceiveEmptyMessage',
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
  serialized_start=187,
  serialized_end=214,
)


_WORKERREQUESTPARSEFAILURE = descriptor.Descriptor(
  name='WorkerRequestParseFailure',
  full_name='zippylog.zippylogd.WorkerRequestParseFailure',
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
  serialized_start=216,
  serialized_end=243,
)


_WORKERREQUESTEMPTYENVELOPE = descriptor.Descriptor(
  name='WorkerRequestEmptyEnvelope',
  full_name='zippylog.zippylogd.WorkerRequestEmptyEnvelope',
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
  serialized_start=245,
  serialized_end=273,
)


_WORKERINVALIDMESSAGEENUMERATION = descriptor.Descriptor(
  name='WorkerInvalidMessageEnumeration',
  full_name='zippylog.zippylogd.WorkerInvalidMessageEnumeration',
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
  serialized_start=275,
  serialized_end=308,
)


_WORKERBEGINPROCESSSTOREINFO = descriptor.Descriptor(
  name='WorkerBeginProcessStoreInfo',
  full_name='zippylog.zippylogd.WorkerBeginProcessStoreInfo',
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
  serialized_start=310,
  serialized_end=339,
)


_WORKERENDPROCESSSTOREINFO = descriptor.Descriptor(
  name='WorkerEndProcessStoreInfo',
  full_name='zippylog.zippylogd.WorkerEndProcessStoreInfo',
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
  serialized_start=341,
  serialized_end=368,
)


_WORKERRECEIVEINVALIDGET = descriptor.Descriptor(
  name='WorkerReceiveInvalidGet',
  full_name='zippylog.zippylogd.WorkerReceiveInvalidGet',
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
  serialized_start=370,
  serialized_end=395,
)


_WORKERGETINVALIDSTREAM = descriptor.Descriptor(
  name='WorkerGetInvalidStream',
  full_name='zippylog.zippylogd.WorkerGetInvalidStream',
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
  serialized_start=397,
  serialized_end=421,
)


_WORKERGETINVALIDOFFSET = descriptor.Descriptor(
  name='WorkerGetInvalidOffset',
  full_name='zippylog.zippylogd.WorkerGetInvalidOffset',
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
  serialized_start=423,
  serialized_end=447,
)


_WORKERBEGINPROCESSGET = descriptor.Descriptor(
  name='WorkerBeginProcessGet',
  full_name='zippylog.zippylogd.WorkerBeginProcessGet',
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
  serialized_start=449,
  serialized_end=472,
)


_WORKERENDPROCESSGET = descriptor.Descriptor(
  name='WorkerEndProcessGet',
  full_name='zippylog.zippylogd.WorkerEndProcessGet',
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
  serialized_start=474,
  serialized_end=495,
)


_WORKERSENDERRORRESPONSE = descriptor.Descriptor(
  name='WorkerSendErrorResponse',
  full_name='zippylog.zippylogd.WorkerSendErrorResponse',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    descriptor.FieldDescriptor(
      name='message', full_name='zippylog.zippylogd.WorkerSendErrorResponse.message', index=0,
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
  serialized_start=497,
  serialized_end=539,
)


_STREAMERSUBSCRIPTIONEXPIRED = descriptor.Descriptor(
  name='StreamerSubscriptionExpired',
  full_name='zippylog.zippylogd.StreamerSubscriptionExpired',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    descriptor.FieldDescriptor(
      name='id', full_name='zippylog.zippylogd.StreamerSubscriptionExpired.id', index=0,
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
  serialized_start=541,
  serialized_end=582,
)


_STREAMERRECEIVEKEEPALIVE = descriptor.Descriptor(
  name='StreamerReceiveKeepalive',
  full_name='zippylog.zippylogd.StreamerReceiveKeepalive',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    descriptor.FieldDescriptor(
      name='id', full_name='zippylog.zippylogd.StreamerReceiveKeepalive.id', index=0,
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
  serialized_start=584,
  serialized_end=622,
)


_STREAMERSUBSCRIPTIONRENEWEDFROMKEEPALIVE = descriptor.Descriptor(
  name='StreamerSubscriptionRenewedFromKeepalive',
  full_name='zippylog.zippylogd.StreamerSubscriptionRenewedFromKeepalive',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    descriptor.FieldDescriptor(
      name='id', full_name='zippylog.zippylogd.StreamerSubscriptionRenewedFromKeepalive.id', index=0,
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
  serialized_start=624,
  serialized_end=678,
)


_STREAMERREJECTKEEPALIVEUNKNOWNSUBSCRIPTION = descriptor.Descriptor(
  name='StreamerRejectKeepaliveUnknownSubscription',
  full_name='zippylog.zippylogd.StreamerRejectKeepaliveUnknownSubscription',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    descriptor.FieldDescriptor(
      name='id', full_name='zippylog.zippylogd.StreamerRejectKeepaliveUnknownSubscription.id', index=0,
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
  serialized_start=680,
  serialized_end=736,
)



class BrokerStartup(message.Message):
  __metaclass__ = reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _BROKERSTARTUP
  
  ZIPPYLOG_NAMESPACE = 1
  ZIPPYLOG_ENUMERATION = 12
  # @@protoc_insertion_point(class_scope:zippylog.zippylogd.BrokerStartup)

class BrokerShutdown(message.Message):
  __metaclass__ = reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _BROKERSHUTDOWN
  
  ZIPPYLOG_NAMESPACE = 1
  ZIPPYLOG_ENUMERATION = 13
  # @@protoc_insertion_point(class_scope:zippylog.zippylogd.BrokerShutdown)

class BrokerReceiveClientMessage(message.Message):
  __metaclass__ = reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _BROKERRECEIVECLIENTMESSAGE
  
  ZIPPYLOG_NAMESPACE = 1
  ZIPPYLOG_ENUMERATION = 14
  # @@protoc_insertion_point(class_scope:zippylog.zippylogd.BrokerReceiveClientMessage)

class WorkerFailReceiveMessage(message.Message):
  __metaclass__ = reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _WORKERFAILRECEIVEMESSAGE
  
  ZIPPYLOG_NAMESPACE = 1
  ZIPPYLOG_ENUMERATION = 27
  # @@protoc_insertion_point(class_scope:zippylog.zippylogd.WorkerFailReceiveMessage)

class WorkerReceiveEmptyMessage(message.Message):
  __metaclass__ = reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _WORKERRECEIVEEMPTYMESSAGE
  
  ZIPPYLOG_NAMESPACE = 1
  ZIPPYLOG_ENUMERATION = 28
  # @@protoc_insertion_point(class_scope:zippylog.zippylogd.WorkerReceiveEmptyMessage)

class WorkerRequestParseFailure(message.Message):
  __metaclass__ = reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _WORKERREQUESTPARSEFAILURE
  
  ZIPPYLOG_NAMESPACE = 1
  ZIPPYLOG_ENUMERATION = 29
  # @@protoc_insertion_point(class_scope:zippylog.zippylogd.WorkerRequestParseFailure)

class WorkerRequestEmptyEnvelope(message.Message):
  __metaclass__ = reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _WORKERREQUESTEMPTYENVELOPE
  
  ZIPPYLOG_NAMESPACE = 1
  ZIPPYLOG_ENUMERATION = 30
  # @@protoc_insertion_point(class_scope:zippylog.zippylogd.WorkerRequestEmptyEnvelope)

class WorkerInvalidMessageEnumeration(message.Message):
  __metaclass__ = reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _WORKERINVALIDMESSAGEENUMERATION
  
  ZIPPYLOG_NAMESPACE = 1
  ZIPPYLOG_ENUMERATION = 31
  # @@protoc_insertion_point(class_scope:zippylog.zippylogd.WorkerInvalidMessageEnumeration)

class WorkerBeginProcessStoreInfo(message.Message):
  __metaclass__ = reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _WORKERBEGINPROCESSSTOREINFO
  
  ZIPPYLOG_NAMESPACE = 1
  ZIPPYLOG_ENUMERATION = 32
  # @@protoc_insertion_point(class_scope:zippylog.zippylogd.WorkerBeginProcessStoreInfo)

class WorkerEndProcessStoreInfo(message.Message):
  __metaclass__ = reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _WORKERENDPROCESSSTOREINFO
  
  ZIPPYLOG_NAMESPACE = 1
  ZIPPYLOG_ENUMERATION = 33
  # @@protoc_insertion_point(class_scope:zippylog.zippylogd.WorkerEndProcessStoreInfo)

class WorkerReceiveInvalidGet(message.Message):
  __metaclass__ = reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _WORKERRECEIVEINVALIDGET
  
  ZIPPYLOG_NAMESPACE = 1
  ZIPPYLOG_ENUMERATION = 34
  # @@protoc_insertion_point(class_scope:zippylog.zippylogd.WorkerReceiveInvalidGet)

class WorkerGetInvalidStream(message.Message):
  __metaclass__ = reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _WORKERGETINVALIDSTREAM
  
  ZIPPYLOG_NAMESPACE = 1
  ZIPPYLOG_ENUMERATION = 35
  # @@protoc_insertion_point(class_scope:zippylog.zippylogd.WorkerGetInvalidStream)

class WorkerGetInvalidOffset(message.Message):
  __metaclass__ = reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _WORKERGETINVALIDOFFSET
  
  ZIPPYLOG_NAMESPACE = 1
  ZIPPYLOG_ENUMERATION = 36
  # @@protoc_insertion_point(class_scope:zippylog.zippylogd.WorkerGetInvalidOffset)

class WorkerBeginProcessGet(message.Message):
  __metaclass__ = reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _WORKERBEGINPROCESSGET
  
  ZIPPYLOG_NAMESPACE = 1
  ZIPPYLOG_ENUMERATION = 37
  # @@protoc_insertion_point(class_scope:zippylog.zippylogd.WorkerBeginProcessGet)

class WorkerEndProcessGet(message.Message):
  __metaclass__ = reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _WORKERENDPROCESSGET
  
  ZIPPYLOG_NAMESPACE = 1
  ZIPPYLOG_ENUMERATION = 38
  # @@protoc_insertion_point(class_scope:zippylog.zippylogd.WorkerEndProcessGet)

class WorkerSendErrorResponse(message.Message):
  __metaclass__ = reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _WORKERSENDERRORRESPONSE
  
  ZIPPYLOG_NAMESPACE = 1
  ZIPPYLOG_ENUMERATION = 39
  # @@protoc_insertion_point(class_scope:zippylog.zippylogd.WorkerSendErrorResponse)

class StreamerSubscriptionExpired(message.Message):
  __metaclass__ = reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _STREAMERSUBSCRIPTIONEXPIRED
  
  ZIPPYLOG_NAMESPACE = 1
  ZIPPYLOG_ENUMERATION = 40
  # @@protoc_insertion_point(class_scope:zippylog.zippylogd.StreamerSubscriptionExpired)

class StreamerReceiveKeepalive(message.Message):
  __metaclass__ = reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _STREAMERRECEIVEKEEPALIVE
  
  ZIPPYLOG_NAMESPACE = 1
  ZIPPYLOG_ENUMERATION = 41
  # @@protoc_insertion_point(class_scope:zippylog.zippylogd.StreamerReceiveKeepalive)

class StreamerSubscriptionRenewedFromKeepalive(message.Message):
  __metaclass__ = reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _STREAMERSUBSCRIPTIONRENEWEDFROMKEEPALIVE
  
  ZIPPYLOG_NAMESPACE = 1
  ZIPPYLOG_ENUMERATION = 42
  # @@protoc_insertion_point(class_scope:zippylog.zippylogd.StreamerSubscriptionRenewedFromKeepalive)

class StreamerRejectKeepaliveUnknownSubscription(message.Message):
  __metaclass__ = reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _STREAMERREJECTKEEPALIVEUNKNOWNSUBSCRIPTION
  
  ZIPPYLOG_NAMESPACE = 1
  ZIPPYLOG_ENUMERATION = 43
  # @@protoc_insertion_point(class_scope:zippylog.zippylogd.StreamerRejectKeepaliveUnknownSubscription)

register_message(1, 12, 'zippylog.zippylogd_pb2', 'BrokerStartup')
register_message(1, 13, 'zippylog.zippylogd_pb2', 'BrokerShutdown')
register_message(1, 14, 'zippylog.zippylogd_pb2', 'BrokerReceiveClientMessage')
register_message(1, 27, 'zippylog.zippylogd_pb2', 'WorkerFailReceiveMessage')
register_message(1, 28, 'zippylog.zippylogd_pb2', 'WorkerReceiveEmptyMessage')
register_message(1, 29, 'zippylog.zippylogd_pb2', 'WorkerRequestParseFailure')
register_message(1, 30, 'zippylog.zippylogd_pb2', 'WorkerRequestEmptyEnvelope')
register_message(1, 31, 'zippylog.zippylogd_pb2', 'WorkerInvalidMessageEnumeration')
register_message(1, 32, 'zippylog.zippylogd_pb2', 'WorkerBeginProcessStoreInfo')
register_message(1, 33, 'zippylog.zippylogd_pb2', 'WorkerEndProcessStoreInfo')
register_message(1, 34, 'zippylog.zippylogd_pb2', 'WorkerReceiveInvalidGet')
register_message(1, 35, 'zippylog.zippylogd_pb2', 'WorkerGetInvalidStream')
register_message(1, 36, 'zippylog.zippylogd_pb2', 'WorkerGetInvalidOffset')
register_message(1, 37, 'zippylog.zippylogd_pb2', 'WorkerBeginProcessGet')
register_message(1, 38, 'zippylog.zippylogd_pb2', 'WorkerEndProcessGet')
register_message(1, 39, 'zippylog.zippylogd_pb2', 'WorkerSendErrorResponse')
register_message(1, 40, 'zippylog.zippylogd_pb2', 'StreamerSubscriptionExpired')
register_message(1, 41, 'zippylog.zippylogd_pb2', 'StreamerReceiveKeepalive')
register_message(1, 42, 'zippylog.zippylogd_pb2', 'StreamerSubscriptionRenewedFromKeepalive')
register_message(1, 43, 'zippylog.zippylogd_pb2', 'StreamerRejectKeepaliveUnknownSubscription')
# @@protoc_insertion_point(module_scope)
