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
  serialized_pb='\n\x18zippylog/zippylogd.proto\x12\x12zippylog.zippylogd\"=\n\rBrokerStartup\x12\x12\n\nstore_path\x18\x01 \x01(\t\x12\x18\n\x10listen_endpoints\x18\x02 \x03(\t\"\x10\n\x0e\x42rokerShutdown\"\x1c\n\x1a\x42rokerReceiveClientMessage\"\x1b\n\rWorkerStartup\x12\n\n\x02id\x18\x01 \x01(\x0c\"\x1c\n\x0eWorkerShutdown\x12\n\n\x02id\x18\x01 \x01(\x0c\"&\n\x18WorkerFailReceiveMessage\x12\n\n\x02id\x18\x01 \x01(\x0c\"\'\n\x19WorkerReceiveEmptyMessage\x12\n\n\x02id\x18\x01 \x01(\x0c\"B\n\x1fWorkerReceiveUnknownRequestType\x12\n\n\x02id\x18\x01 \x01(\x0c\x12\x13\n\x0b\x65numeration\x18\x02 \x01(\r\"\'\n\x19WorkerRequestParseFailure\x12\n\n\x02id\x18\x01 \x01(\x0c\"(\n\x1aWorkerRequestEmptyEnvelope\x12\n\n\x02id\x18\x01 \x01(\x0c\"-\n\x1fWorkerInvalidMessageEnumeration\x12\n\n\x02id\x18\x01 \x01(\x0c\")\n\x1bWorkerBeginProcessStoreInfo\x12\n\n\x02id\x18\x01 \x01(\x0c\"\'\n\x19WorkerEndProcessStoreInfo\x12\n\n\x02id\x18\x01 \x01(\x0c\"%\n\x17WorkerReceiveInvalidGet\x12\n\n\x02id\x18\x01 \x01(\x0c\"$\n\x16WorkerGetInvalidStream\x12\n\n\x02id\x18\x01 \x01(\x0c\"$\n\x16WorkerGetInvalidOffset\x12\n\n\x02id\x18\x01 \x01(\x0c\"#\n\x15WorkerBeginProcessGet\x12\n\n\x02id\x18\x01 \x01(\x0c\"!\n\x13WorkerEndProcessGet\x12\n\n\x02id\x18\x01 \x01(\x0c\"6\n\x17WorkerSendErrorResponse\x12\n\n\x02id\x18\x01 \x01(\x0c\x12\x0f\n\x07message\x18\x02 \x01(\t\"C\n\x1fWorkerForwardSubscribeKeepalive\x12\n\n\x02id\x18\x01 \x01(\x0c\x12\x14\n\x0csubscription\x18\x02 \x01(\x0c\"\x1d\n\x0fStreamerStartup\x12\n\n\x02id\x18\x01 \x01(\x0c\"\x1e\n\x10StreamerShutdown\x12\n\n\x02id\x18\x01 \x01(\x0c\"?\n\x1bStreamerSubscriptionExpired\x12\n\n\x02id\x18\x01 \x01(\x0c\x12\x14\n\x0csubscription\x18\x02 \x01(\x0c\"<\n\x18StreamerReceiveKeepalive\x12\n\n\x02id\x18\x01 \x01(\x0c\x12\x14\n\x0csubscription\x18\x02 \x01(\x0c\"L\n(StreamerSubscriptionRenewedFromKeepalive\x12\n\n\x02id\x18\x01 \x01(\x0c\x12\x14\n\x0csubscription\x18\x02 \x01(\x0c\"N\n*StreamerRejectKeepaliveUnknownSubscription\x12\n\n\x02id\x18\x01 \x01(\x0c\x12\x14\n\x0csubscription\x18\x02 \x01(\x0c\"E\n!StreamerErrorRenewingSubscription\x12\n\n\x02id\x18\x01 \x01(\x0c\x12\x14\n\x0csubscription\x18\x02 \x01(\x0c')




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


_WORKERSTARTUP = descriptor.Descriptor(
  name='WorkerStartup',
  full_name='zippylog.zippylogd.WorkerStartup',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    descriptor.FieldDescriptor(
      name='id', full_name='zippylog.zippylogd.WorkerStartup.id', index=0,
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
  serialized_start=159,
  serialized_end=186,
)


_WORKERSHUTDOWN = descriptor.Descriptor(
  name='WorkerShutdown',
  full_name='zippylog.zippylogd.WorkerShutdown',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    descriptor.FieldDescriptor(
      name='id', full_name='zippylog.zippylogd.WorkerShutdown.id', index=0,
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
  serialized_start=188,
  serialized_end=216,
)


_WORKERFAILRECEIVEMESSAGE = descriptor.Descriptor(
  name='WorkerFailReceiveMessage',
  full_name='zippylog.zippylogd.WorkerFailReceiveMessage',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    descriptor.FieldDescriptor(
      name='id', full_name='zippylog.zippylogd.WorkerFailReceiveMessage.id', index=0,
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
  serialized_start=218,
  serialized_end=256,
)


_WORKERRECEIVEEMPTYMESSAGE = descriptor.Descriptor(
  name='WorkerReceiveEmptyMessage',
  full_name='zippylog.zippylogd.WorkerReceiveEmptyMessage',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    descriptor.FieldDescriptor(
      name='id', full_name='zippylog.zippylogd.WorkerReceiveEmptyMessage.id', index=0,
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
  serialized_start=258,
  serialized_end=297,
)


_WORKERRECEIVEUNKNOWNREQUESTTYPE = descriptor.Descriptor(
  name='WorkerReceiveUnknownRequestType',
  full_name='zippylog.zippylogd.WorkerReceiveUnknownRequestType',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    descriptor.FieldDescriptor(
      name='id', full_name='zippylog.zippylogd.WorkerReceiveUnknownRequestType.id', index=0,
      number=1, type=12, cpp_type=9, label=1,
      has_default_value=False, default_value="",
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    descriptor.FieldDescriptor(
      name='enumeration', full_name='zippylog.zippylogd.WorkerReceiveUnknownRequestType.enumeration', index=1,
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
  serialized_start=299,
  serialized_end=365,
)


_WORKERREQUESTPARSEFAILURE = descriptor.Descriptor(
  name='WorkerRequestParseFailure',
  full_name='zippylog.zippylogd.WorkerRequestParseFailure',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    descriptor.FieldDescriptor(
      name='id', full_name='zippylog.zippylogd.WorkerRequestParseFailure.id', index=0,
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
  serialized_start=367,
  serialized_end=406,
)


_WORKERREQUESTEMPTYENVELOPE = descriptor.Descriptor(
  name='WorkerRequestEmptyEnvelope',
  full_name='zippylog.zippylogd.WorkerRequestEmptyEnvelope',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    descriptor.FieldDescriptor(
      name='id', full_name='zippylog.zippylogd.WorkerRequestEmptyEnvelope.id', index=0,
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
  serialized_start=408,
  serialized_end=448,
)


_WORKERINVALIDMESSAGEENUMERATION = descriptor.Descriptor(
  name='WorkerInvalidMessageEnumeration',
  full_name='zippylog.zippylogd.WorkerInvalidMessageEnumeration',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    descriptor.FieldDescriptor(
      name='id', full_name='zippylog.zippylogd.WorkerInvalidMessageEnumeration.id', index=0,
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
  serialized_start=450,
  serialized_end=495,
)


_WORKERBEGINPROCESSSTOREINFO = descriptor.Descriptor(
  name='WorkerBeginProcessStoreInfo',
  full_name='zippylog.zippylogd.WorkerBeginProcessStoreInfo',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    descriptor.FieldDescriptor(
      name='id', full_name='zippylog.zippylogd.WorkerBeginProcessStoreInfo.id', index=0,
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
  serialized_start=497,
  serialized_end=538,
)


_WORKERENDPROCESSSTOREINFO = descriptor.Descriptor(
  name='WorkerEndProcessStoreInfo',
  full_name='zippylog.zippylogd.WorkerEndProcessStoreInfo',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    descriptor.FieldDescriptor(
      name='id', full_name='zippylog.zippylogd.WorkerEndProcessStoreInfo.id', index=0,
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
  serialized_end=579,
)


_WORKERRECEIVEINVALIDGET = descriptor.Descriptor(
  name='WorkerReceiveInvalidGet',
  full_name='zippylog.zippylogd.WorkerReceiveInvalidGet',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    descriptor.FieldDescriptor(
      name='id', full_name='zippylog.zippylogd.WorkerReceiveInvalidGet.id', index=0,
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
  serialized_start=581,
  serialized_end=618,
)


_WORKERGETINVALIDSTREAM = descriptor.Descriptor(
  name='WorkerGetInvalidStream',
  full_name='zippylog.zippylogd.WorkerGetInvalidStream',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    descriptor.FieldDescriptor(
      name='id', full_name='zippylog.zippylogd.WorkerGetInvalidStream.id', index=0,
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
  serialized_start=620,
  serialized_end=656,
)


_WORKERGETINVALIDOFFSET = descriptor.Descriptor(
  name='WorkerGetInvalidOffset',
  full_name='zippylog.zippylogd.WorkerGetInvalidOffset',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    descriptor.FieldDescriptor(
      name='id', full_name='zippylog.zippylogd.WorkerGetInvalidOffset.id', index=0,
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
  serialized_start=658,
  serialized_end=694,
)


_WORKERBEGINPROCESSGET = descriptor.Descriptor(
  name='WorkerBeginProcessGet',
  full_name='zippylog.zippylogd.WorkerBeginProcessGet',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    descriptor.FieldDescriptor(
      name='id', full_name='zippylog.zippylogd.WorkerBeginProcessGet.id', index=0,
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
  serialized_start=696,
  serialized_end=731,
)


_WORKERENDPROCESSGET = descriptor.Descriptor(
  name='WorkerEndProcessGet',
  full_name='zippylog.zippylogd.WorkerEndProcessGet',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    descriptor.FieldDescriptor(
      name='id', full_name='zippylog.zippylogd.WorkerEndProcessGet.id', index=0,
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
  serialized_start=733,
  serialized_end=766,
)


_WORKERSENDERRORRESPONSE = descriptor.Descriptor(
  name='WorkerSendErrorResponse',
  full_name='zippylog.zippylogd.WorkerSendErrorResponse',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    descriptor.FieldDescriptor(
      name='id', full_name='zippylog.zippylogd.WorkerSendErrorResponse.id', index=0,
      number=1, type=12, cpp_type=9, label=1,
      has_default_value=False, default_value="",
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    descriptor.FieldDescriptor(
      name='message', full_name='zippylog.zippylogd.WorkerSendErrorResponse.message', index=1,
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
  serialized_start=768,
  serialized_end=822,
)


_WORKERFORWARDSUBSCRIBEKEEPALIVE = descriptor.Descriptor(
  name='WorkerForwardSubscribeKeepalive',
  full_name='zippylog.zippylogd.WorkerForwardSubscribeKeepalive',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    descriptor.FieldDescriptor(
      name='id', full_name='zippylog.zippylogd.WorkerForwardSubscribeKeepalive.id', index=0,
      number=1, type=12, cpp_type=9, label=1,
      has_default_value=False, default_value="",
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    descriptor.FieldDescriptor(
      name='subscription', full_name='zippylog.zippylogd.WorkerForwardSubscribeKeepalive.subscription', index=1,
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
  serialized_start=824,
  serialized_end=891,
)


_STREAMERSTARTUP = descriptor.Descriptor(
  name='StreamerStartup',
  full_name='zippylog.zippylogd.StreamerStartup',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    descriptor.FieldDescriptor(
      name='id', full_name='zippylog.zippylogd.StreamerStartup.id', index=0,
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
  serialized_start=893,
  serialized_end=922,
)


_STREAMERSHUTDOWN = descriptor.Descriptor(
  name='StreamerShutdown',
  full_name='zippylog.zippylogd.StreamerShutdown',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    descriptor.FieldDescriptor(
      name='id', full_name='zippylog.zippylogd.StreamerShutdown.id', index=0,
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
  serialized_start=924,
  serialized_end=954,
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
    descriptor.FieldDescriptor(
      name='subscription', full_name='zippylog.zippylogd.StreamerSubscriptionExpired.subscription', index=1,
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
  serialized_start=956,
  serialized_end=1019,
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
    descriptor.FieldDescriptor(
      name='subscription', full_name='zippylog.zippylogd.StreamerReceiveKeepalive.subscription', index=1,
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
  serialized_start=1021,
  serialized_end=1081,
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
    descriptor.FieldDescriptor(
      name='subscription', full_name='zippylog.zippylogd.StreamerSubscriptionRenewedFromKeepalive.subscription', index=1,
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
  serialized_start=1083,
  serialized_end=1159,
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
    descriptor.FieldDescriptor(
      name='subscription', full_name='zippylog.zippylogd.StreamerRejectKeepaliveUnknownSubscription.subscription', index=1,
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
  serialized_start=1161,
  serialized_end=1239,
)


_STREAMERERRORRENEWINGSUBSCRIPTION = descriptor.Descriptor(
  name='StreamerErrorRenewingSubscription',
  full_name='zippylog.zippylogd.StreamerErrorRenewingSubscription',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    descriptor.FieldDescriptor(
      name='id', full_name='zippylog.zippylogd.StreamerErrorRenewingSubscription.id', index=0,
      number=1, type=12, cpp_type=9, label=1,
      has_default_value=False, default_value="",
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    descriptor.FieldDescriptor(
      name='subscription', full_name='zippylog.zippylogd.StreamerErrorRenewingSubscription.subscription', index=1,
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
  serialized_start=1241,
  serialized_end=1310,
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

class WorkerStartup(message.Message):
  __metaclass__ = reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _WORKERSTARTUP
  
  ZIPPYLOG_NAMESPACE = 1
  ZIPPYLOG_ENUMERATION = 45
  # @@protoc_insertion_point(class_scope:zippylog.zippylogd.WorkerStartup)

class WorkerShutdown(message.Message):
  __metaclass__ = reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _WORKERSHUTDOWN
  
  ZIPPYLOG_NAMESPACE = 1
  ZIPPYLOG_ENUMERATION = 46
  # @@protoc_insertion_point(class_scope:zippylog.zippylogd.WorkerShutdown)

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

class WorkerReceiveUnknownRequestType(message.Message):
  __metaclass__ = reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _WORKERRECEIVEUNKNOWNREQUESTTYPE
  
  ZIPPYLOG_NAMESPACE = 1
  ZIPPYLOG_ENUMERATION = 47
  # @@protoc_insertion_point(class_scope:zippylog.zippylogd.WorkerReceiveUnknownRequestType)

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

class WorkerForwardSubscribeKeepalive(message.Message):
  __metaclass__ = reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _WORKERFORWARDSUBSCRIBEKEEPALIVE
  
  ZIPPYLOG_NAMESPACE = 1
  ZIPPYLOG_ENUMERATION = 48
  # @@protoc_insertion_point(class_scope:zippylog.zippylogd.WorkerForwardSubscribeKeepalive)

class StreamerStartup(message.Message):
  __metaclass__ = reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _STREAMERSTARTUP
  
  ZIPPYLOG_NAMESPACE = 1
  ZIPPYLOG_ENUMERATION = 49
  # @@protoc_insertion_point(class_scope:zippylog.zippylogd.StreamerStartup)

class StreamerShutdown(message.Message):
  __metaclass__ = reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _STREAMERSHUTDOWN
  
  ZIPPYLOG_NAMESPACE = 1
  ZIPPYLOG_ENUMERATION = 50
  # @@protoc_insertion_point(class_scope:zippylog.zippylogd.StreamerShutdown)

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

class StreamerErrorRenewingSubscription(message.Message):
  __metaclass__ = reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _STREAMERERRORRENEWINGSUBSCRIPTION
  
  ZIPPYLOG_NAMESPACE = 1
  ZIPPYLOG_ENUMERATION = 44
  # @@protoc_insertion_point(class_scope:zippylog.zippylogd.StreamerErrorRenewingSubscription)

register_message(1, 12, 'zippylog.zippylogd_pb2', 'BrokerStartup')
register_message(1, 13, 'zippylog.zippylogd_pb2', 'BrokerShutdown')
register_message(1, 14, 'zippylog.zippylogd_pb2', 'BrokerReceiveClientMessage')
register_message(1, 45, 'zippylog.zippylogd_pb2', 'WorkerStartup')
register_message(1, 46, 'zippylog.zippylogd_pb2', 'WorkerShutdown')
register_message(1, 27, 'zippylog.zippylogd_pb2', 'WorkerFailReceiveMessage')
register_message(1, 28, 'zippylog.zippylogd_pb2', 'WorkerReceiveEmptyMessage')
register_message(1, 47, 'zippylog.zippylogd_pb2', 'WorkerReceiveUnknownRequestType')
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
register_message(1, 48, 'zippylog.zippylogd_pb2', 'WorkerForwardSubscribeKeepalive')
register_message(1, 49, 'zippylog.zippylogd_pb2', 'StreamerStartup')
register_message(1, 50, 'zippylog.zippylogd_pb2', 'StreamerShutdown')
register_message(1, 40, 'zippylog.zippylogd_pb2', 'StreamerSubscriptionExpired')
register_message(1, 41, 'zippylog.zippylogd_pb2', 'StreamerReceiveKeepalive')
register_message(1, 42, 'zippylog.zippylogd_pb2', 'StreamerSubscriptionRenewedFromKeepalive')
register_message(1, 43, 'zippylog.zippylogd_pb2', 'StreamerRejectKeepaliveUnknownSubscription')
register_message(1, 44, 'zippylog.zippylogd_pb2', 'StreamerErrorRenewingSubscription')
# @@protoc_insertion_point(module_scope)
