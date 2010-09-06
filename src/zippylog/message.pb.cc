// Generated by the protocol buffer compiler.  DO NOT EDIT!

#define INTERNAL_SUPPRESS_PROTOBUF_FIELD_DEPRECATION
#include "zippylog/message.pb.h"
#include <google/protobuf/stubs/once.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/wire_format_lite_inl.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>
// @@protoc_insertion_point(includes)

namespace zippylog {
namespace message {

namespace {

const ::google::protobuf::Descriptor* Envelope_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  Envelope_reflection_ = NULL;
const ::google::protobuf::Descriptor* ActorInfo_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  ActorInfo_reflection_ = NULL;

}  // namespace


void protobuf_AssignDesc_zippylog_2fmessage_2eproto() {
  protobuf_AddDesc_zippylog_2fmessage_2eproto();
  const ::google::protobuf::FileDescriptor* file =
    ::google::protobuf::DescriptorPool::generated_pool()->FindFileByName(
      "zippylog/message.proto");
  GOOGLE_CHECK(file != NULL);
  Envelope_descriptor_ = file->message_type(0);
  static const int Envelope_offsets_[7] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Envelope, message_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Envelope, message_namespace_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Envelope, message_type_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Envelope, create_time_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Envelope, tag_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Envelope, actor_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Envelope, numeric_value_),
  };
  Envelope_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      Envelope_descriptor_,
      Envelope::default_instance_,
      Envelope_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Envelope, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Envelope, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool(),
      ::google::protobuf::MessageFactory::generated_factory(),
      sizeof(Envelope));
  ActorInfo_descriptor_ = file->message_type(1);
  static const int ActorInfo_offsets_[6] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(ActorInfo, touch_time_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(ActorInfo, sequence_id_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(ActorInfo, hostname_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(ActorInfo, host_id_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(ActorInfo, actor_id_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(ActorInfo, app_id_),
  };
  ActorInfo_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      ActorInfo_descriptor_,
      ActorInfo::default_instance_,
      ActorInfo_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(ActorInfo, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(ActorInfo, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool(),
      ::google::protobuf::MessageFactory::generated_factory(),
      sizeof(ActorInfo));
}

namespace {

GOOGLE_PROTOBUF_DECLARE_ONCE(protobuf_AssignDescriptors_once_);
inline void protobuf_AssignDescriptorsOnce() {
  ::google::protobuf::GoogleOnceInit(&protobuf_AssignDescriptors_once_,
                 &protobuf_AssignDesc_zippylog_2fmessage_2eproto);
}

void protobuf_RegisterTypes(const ::std::string&) {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
    Envelope_descriptor_, &Envelope::default_instance());
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
    ActorInfo_descriptor_, &ActorInfo::default_instance());
}

}  // namespace

void protobuf_ShutdownFile_zippylog_2fmessage_2eproto() {
  delete Envelope::default_instance_;
  delete Envelope_reflection_;
  delete ActorInfo::default_instance_;
  delete ActorInfo_reflection_;
}

void protobuf_AddDesc_zippylog_2fmessage_2eproto() {
  static bool already_here = false;
  if (already_here) return;
  already_here = true;
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  ::google::protobuf::DescriptorPool::InternalAddGeneratedFile(
    "\n\023zippylog/message.proto\022\rzippylog.message\"\266\001\n"
    "\010Envelope\022\017\n\007message\030\001 \003(\014\022\035\n\021message_na"
    "mespace\030\002 \003(\rB\002\020\001\022\030\n\014message_type\030\003 \003(\rB"
    "\002\020\001\022\023\n\013create_time\030\004 \001(\004\022\013\n\003tag\030\005 \003(\014\022\'\n"
    "\005actor\030\006 \003(\0132\030.zippylog.message.ActorInfo\022\025"
    "\n\rnumeric_value\030\007 \001(\004\"y\n\tActorInfo\022\022\n\nto"
    "uch_time\030\001 \001(\004\022\023\n\013sequence_id\030\002 \001(\004\022\020\n\010h"
    "ostname\030\003 \001(\t\022\017\n\007host_id\030\004 \001(\014\022\020\n\010actor_"
    "id\030\005 \001(\014\022\016\n\006app_id\030\006 \001(\014", 344);
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedFile(
    "zippylog/message.proto", &protobuf_RegisterTypes);
  Envelope::default_instance_ = new Envelope();
  ActorInfo::default_instance_ = new ActorInfo();
  Envelope::default_instance_->InitAsDefaultInstance();
  ActorInfo::default_instance_->InitAsDefaultInstance();
  ::google::protobuf::internal::OnShutdown(&protobuf_ShutdownFile_zippylog_2fmessage_2eproto);
}

// Force AddDescriptors() to be called at static initialization time.
struct StaticDescriptorInitializer_zippylog_2fmessage_2eproto {
  StaticDescriptorInitializer_zippylog_2fmessage_2eproto() {
    protobuf_AddDesc_zippylog_2fmessage_2eproto();
  }
} static_descriptor_initializer_zippylog_2fmessage_2eproto_;


// ===================================================================

#ifndef _MSC_VER
const int Envelope::kMessageFieldNumber;
const int Envelope::kMessageNamespaceFieldNumber;
const int Envelope::kMessageTypeFieldNumber;
const int Envelope::kCreateTimeFieldNumber;
const int Envelope::kTagFieldNumber;
const int Envelope::kActorFieldNumber;
const int Envelope::kNumericValueFieldNumber;
#endif  // !_MSC_VER

Envelope::Envelope()
  : ::google::protobuf::Message() {
  SharedCtor();
}

void Envelope::InitAsDefaultInstance() {
}

Envelope::Envelope(const Envelope& from)
  : ::google::protobuf::Message() {
  SharedCtor();
  MergeFrom(from);
}

void Envelope::SharedCtor() {
  _cached_size_ = 0;
  create_time_ = GOOGLE_ULONGLONG(0);
  numeric_value_ = GOOGLE_ULONGLONG(0);
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

Envelope::~Envelope() {
  SharedDtor();
}

void Envelope::SharedDtor() {
  if (this != default_instance_) {
  }
}

void Envelope::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* Envelope::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return Envelope_descriptor_;
}

const Envelope& Envelope::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_zippylog_2fmessage_2eproto();  return *default_instance_;
}

Envelope* Envelope::default_instance_ = NULL;

Envelope* Envelope::New() const {
  return new Envelope;
}

void Envelope::Clear() {
  if (_has_bits_[3 / 32] & (0xffu << (3 % 32))) {
    create_time_ = GOOGLE_ULONGLONG(0);
    numeric_value_ = GOOGLE_ULONGLONG(0);
  }
  message_.Clear();
  message_namespace_.Clear();
  message_type_.Clear();
  tag_.Clear();
  actor_.Clear();
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  mutable_unknown_fields()->Clear();
}

bool Envelope::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) return false
  ::google::protobuf::uint32 tag;
  while ((tag = input->ReadTag()) != 0) {
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // repeated bytes message = 1;
      case 1: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
         parse_message:
          DO_(::google::protobuf::internal::WireFormatLite::ReadBytes(
                input, this->add_message()));
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(10)) goto parse_message;
        if (input->ExpectTag(18)) goto parse_message_namespace;
        break;
      }
      
      // repeated uint32 message_namespace = 2 [packed = true];
      case 2: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
         parse_message_namespace:
          DO_((::google::protobuf::internal::WireFormatLite::ReadPackedPrimitive<
                   ::google::protobuf::uint32, ::google::protobuf::internal::WireFormatLite::TYPE_UINT32>(
                 input, this->mutable_message_namespace())));
        } else if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag)
                   == ::google::protobuf::internal::WireFormatLite::
                      WIRETYPE_VARINT) {
          DO_((::google::protobuf::internal::WireFormatLite::ReadRepeatedPrimitiveNoInline<
                   ::google::protobuf::uint32, ::google::protobuf::internal::WireFormatLite::TYPE_UINT32>(
                 1, 18, input, this->mutable_message_namespace())));
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(26)) goto parse_message_type;
        break;
      }
      
      // repeated uint32 message_type = 3 [packed = true];
      case 3: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
         parse_message_type:
          DO_((::google::protobuf::internal::WireFormatLite::ReadPackedPrimitive<
                   ::google::protobuf::uint32, ::google::protobuf::internal::WireFormatLite::TYPE_UINT32>(
                 input, this->mutable_message_type())));
        } else if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag)
                   == ::google::protobuf::internal::WireFormatLite::
                      WIRETYPE_VARINT) {
          DO_((::google::protobuf::internal::WireFormatLite::ReadRepeatedPrimitiveNoInline<
                   ::google::protobuf::uint32, ::google::protobuf::internal::WireFormatLite::TYPE_UINT32>(
                 1, 26, input, this->mutable_message_type())));
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(32)) goto parse_create_time;
        break;
      }
      
      // optional uint64 create_time = 4;
      case 4: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
         parse_create_time:
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::uint64, ::google::protobuf::internal::WireFormatLite::TYPE_UINT64>(
                 input, &create_time_)));
          _set_bit(3);
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(42)) goto parse_tag;
        break;
      }
      
      // repeated bytes tag = 5;
      case 5: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
         parse_tag:
          DO_(::google::protobuf::internal::WireFormatLite::ReadBytes(
                input, this->add_tag()));
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(42)) goto parse_tag;
        if (input->ExpectTag(50)) goto parse_actor;
        break;
      }
      
      // repeated .zippylog.message.ActorInfo actor = 6;
      case 6: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
         parse_actor:
          DO_(::google::protobuf::internal::WireFormatLite::ReadMessageNoVirtual(
                input, add_actor()));
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(50)) goto parse_actor;
        if (input->ExpectTag(56)) goto parse_numeric_value;
        break;
      }
      
      // optional uint64 numeric_value = 7;
      case 7: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
         parse_numeric_value:
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::uint64, ::google::protobuf::internal::WireFormatLite::TYPE_UINT64>(
                 input, &numeric_value_)));
          _set_bit(6);
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectAtEnd()) return true;
        break;
      }
      
      default: {
      handle_uninterpreted:
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_END_GROUP) {
          return true;
        }
        DO_(::google::protobuf::internal::WireFormat::SkipField(
              input, tag, mutable_unknown_fields()));
        break;
      }
    }
  }
  return true;
#undef DO_
}

void Envelope::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // repeated bytes message = 1;
  for (int i = 0; i < this->message_size(); i++) {
    ::google::protobuf::internal::WireFormatLite::WriteBytes(
      1, this->message(i), output);
  }
  
  // repeated uint32 message_namespace = 2 [packed = true];
  if (this->message_namespace_size() > 0) {
    ::google::protobuf::internal::WireFormatLite::WriteTag(2, ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED, output);
    output->WriteVarint32(_message_namespace_cached_byte_size_);
  }
  for (int i = 0; i < this->message_namespace_size(); i++) {
    ::google::protobuf::internal::WireFormatLite::WriteUInt32NoTag(
      this->message_namespace(i), output);
  }
  
  // repeated uint32 message_type = 3 [packed = true];
  if (this->message_type_size() > 0) {
    ::google::protobuf::internal::WireFormatLite::WriteTag(3, ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED, output);
    output->WriteVarint32(_message_type_cached_byte_size_);
  }
  for (int i = 0; i < this->message_type_size(); i++) {
    ::google::protobuf::internal::WireFormatLite::WriteUInt32NoTag(
      this->message_type(i), output);
  }
  
  // optional uint64 create_time = 4;
  if (_has_bit(3)) {
    ::google::protobuf::internal::WireFormatLite::WriteUInt64(4, this->create_time(), output);
  }
  
  // repeated bytes tag = 5;
  for (int i = 0; i < this->tag_size(); i++) {
    ::google::protobuf::internal::WireFormatLite::WriteBytes(
      5, this->tag(i), output);
  }
  
  // repeated .zippylog.message.ActorInfo actor = 6;
  for (int i = 0; i < this->actor_size(); i++) {
    ::google::protobuf::internal::WireFormatLite::WriteMessageMaybeToArray(
      6, this->actor(i), output);
  }
  
  // optional uint64 numeric_value = 7;
  if (_has_bit(6)) {
    ::google::protobuf::internal::WireFormatLite::WriteUInt64(7, this->numeric_value(), output);
  }
  
  if (!unknown_fields().empty()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        unknown_fields(), output);
  }
}

::google::protobuf::uint8* Envelope::SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* target) const {
  // repeated bytes message = 1;
  for (int i = 0; i < this->message_size(); i++) {
    target = ::google::protobuf::internal::WireFormatLite::
      WriteBytesToArray(1, this->message(i), target);
  }
  
  // repeated uint32 message_namespace = 2 [packed = true];
  if (this->message_namespace_size() > 0) {
    target = ::google::protobuf::internal::WireFormatLite::WriteTagToArray(
      2,
      ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED,
      target);
    target = ::google::protobuf::io::CodedOutputStream::WriteVarint32ToArray(
      _message_namespace_cached_byte_size_, target);
  }
  for (int i = 0; i < this->message_namespace_size(); i++) {
    target = ::google::protobuf::internal::WireFormatLite::
      WriteUInt32NoTagToArray(this->message_namespace(i), target);
  }
  
  // repeated uint32 message_type = 3 [packed = true];
  if (this->message_type_size() > 0) {
    target = ::google::protobuf::internal::WireFormatLite::WriteTagToArray(
      3,
      ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED,
      target);
    target = ::google::protobuf::io::CodedOutputStream::WriteVarint32ToArray(
      _message_type_cached_byte_size_, target);
  }
  for (int i = 0; i < this->message_type_size(); i++) {
    target = ::google::protobuf::internal::WireFormatLite::
      WriteUInt32NoTagToArray(this->message_type(i), target);
  }
  
  // optional uint64 create_time = 4;
  if (_has_bit(3)) {
    target = ::google::protobuf::internal::WireFormatLite::WriteUInt64ToArray(4, this->create_time(), target);
  }
  
  // repeated bytes tag = 5;
  for (int i = 0; i < this->tag_size(); i++) {
    target = ::google::protobuf::internal::WireFormatLite::
      WriteBytesToArray(5, this->tag(i), target);
  }
  
  // repeated .zippylog.message.ActorInfo actor = 6;
  for (int i = 0; i < this->actor_size(); i++) {
    target = ::google::protobuf::internal::WireFormatLite::
      WriteMessageNoVirtualToArray(
        6, this->actor(i), target);
  }
  
  // optional uint64 numeric_value = 7;
  if (_has_bit(6)) {
    target = ::google::protobuf::internal::WireFormatLite::WriteUInt64ToArray(7, this->numeric_value(), target);
  }
  
  if (!unknown_fields().empty()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        unknown_fields(), target);
  }
  return target;
}

int Envelope::ByteSize() const {
  int total_size = 0;
  
  if (_has_bits_[3 / 32] & (0xffu << (3 % 32))) {
    // optional uint64 create_time = 4;
    if (has_create_time()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::UInt64Size(
          this->create_time());
    }
    
    // optional uint64 numeric_value = 7;
    if (has_numeric_value()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::UInt64Size(
          this->numeric_value());
    }
    
  }
  // repeated bytes message = 1;
  total_size += 1 * this->message_size();
  for (int i = 0; i < this->message_size(); i++) {
    total_size += ::google::protobuf::internal::WireFormatLite::BytesSize(
      this->message(i));
  }
  
  // repeated uint32 message_namespace = 2 [packed = true];
  {
    int data_size = 0;
    for (int i = 0; i < this->message_namespace_size(); i++) {
      data_size += ::google::protobuf::internal::WireFormatLite::
        UInt32Size(this->message_namespace(i));
    }
    if (data_size > 0) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::Int32Size(data_size);
    }
    _message_namespace_cached_byte_size_ = data_size;
    total_size += data_size;
  }
  
  // repeated uint32 message_type = 3 [packed = true];
  {
    int data_size = 0;
    for (int i = 0; i < this->message_type_size(); i++) {
      data_size += ::google::protobuf::internal::WireFormatLite::
        UInt32Size(this->message_type(i));
    }
    if (data_size > 0) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::Int32Size(data_size);
    }
    _message_type_cached_byte_size_ = data_size;
    total_size += data_size;
  }
  
  // repeated bytes tag = 5;
  total_size += 1 * this->tag_size();
  for (int i = 0; i < this->tag_size(); i++) {
    total_size += ::google::protobuf::internal::WireFormatLite::BytesSize(
      this->tag(i));
  }
  
  // repeated .zippylog.message.ActorInfo actor = 6;
  total_size += 1 * this->actor_size();
  for (int i = 0; i < this->actor_size(); i++) {
    total_size +=
      ::google::protobuf::internal::WireFormatLite::MessageSizeNoVirtual(
        this->actor(i));
  }
  
  if (!unknown_fields().empty()) {
    total_size +=
      ::google::protobuf::internal::WireFormat::ComputeUnknownFieldsSize(
        unknown_fields());
  }
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = total_size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
  return total_size;
}

void Envelope::MergeFrom(const ::google::protobuf::Message& from) {
  GOOGLE_CHECK_NE(&from, this);
  const Envelope* source =
    ::google::protobuf::internal::dynamic_cast_if_available<const Envelope*>(
      &from);
  if (source == NULL) {
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
    MergeFrom(*source);
  }
}

void Envelope::MergeFrom(const Envelope& from) {
  GOOGLE_CHECK_NE(&from, this);
  message_.MergeFrom(from.message_);
  message_namespace_.MergeFrom(from.message_namespace_);
  message_type_.MergeFrom(from.message_type_);
  tag_.MergeFrom(from.tag_);
  actor_.MergeFrom(from.actor_);
  if (from._has_bits_[3 / 32] & (0xffu << (3 % 32))) {
    if (from._has_bit(3)) {
      set_create_time(from.create_time());
    }
    if (from._has_bit(6)) {
      set_numeric_value(from.numeric_value());
    }
  }
  mutable_unknown_fields()->MergeFrom(from.unknown_fields());
}

void Envelope::CopyFrom(const ::google::protobuf::Message& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void Envelope::CopyFrom(const Envelope& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool Envelope::IsInitialized() const {
  
  return true;
}

void Envelope::Swap(Envelope* other) {
  if (other != this) {
    message_.Swap(&other->message_);
    message_namespace_.Swap(&other->message_namespace_);
    message_type_.Swap(&other->message_type_);
    std::swap(create_time_, other->create_time_);
    tag_.Swap(&other->tag_);
    actor_.Swap(&other->actor_);
    std::swap(numeric_value_, other->numeric_value_);
    std::swap(_has_bits_[0], other->_has_bits_[0]);
    _unknown_fields_.Swap(&other->_unknown_fields_);
    std::swap(_cached_size_, other->_cached_size_);
  }
}

::google::protobuf::Metadata Envelope::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = Envelope_descriptor_;
  metadata.reflection = Envelope_reflection_;
  return metadata;
}


// ===================================================================

const ::std::string ActorInfo::_default_hostname_;
const ::std::string ActorInfo::_default_host_id_;
const ::std::string ActorInfo::_default_actor_id_;
const ::std::string ActorInfo::_default_app_id_;
#ifndef _MSC_VER
const int ActorInfo::kTouchTimeFieldNumber;
const int ActorInfo::kSequenceIdFieldNumber;
const int ActorInfo::kHostnameFieldNumber;
const int ActorInfo::kHostIdFieldNumber;
const int ActorInfo::kActorIdFieldNumber;
const int ActorInfo::kAppIdFieldNumber;
#endif  // !_MSC_VER

ActorInfo::ActorInfo()
  : ::google::protobuf::Message() {
  SharedCtor();
}

void ActorInfo::InitAsDefaultInstance() {
}

ActorInfo::ActorInfo(const ActorInfo& from)
  : ::google::protobuf::Message() {
  SharedCtor();
  MergeFrom(from);
}

void ActorInfo::SharedCtor() {
  _cached_size_ = 0;
  touch_time_ = GOOGLE_ULONGLONG(0);
  sequence_id_ = GOOGLE_ULONGLONG(0);
  hostname_ = const_cast< ::std::string*>(&_default_hostname_);
  host_id_ = const_cast< ::std::string*>(&_default_host_id_);
  actor_id_ = const_cast< ::std::string*>(&_default_actor_id_);
  app_id_ = const_cast< ::std::string*>(&_default_app_id_);
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

ActorInfo::~ActorInfo() {
  SharedDtor();
}

void ActorInfo::SharedDtor() {
  if (hostname_ != &_default_hostname_) {
    delete hostname_;
  }
  if (host_id_ != &_default_host_id_) {
    delete host_id_;
  }
  if (actor_id_ != &_default_actor_id_) {
    delete actor_id_;
  }
  if (app_id_ != &_default_app_id_) {
    delete app_id_;
  }
  if (this != default_instance_) {
  }
}

void ActorInfo::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* ActorInfo::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return ActorInfo_descriptor_;
}

const ActorInfo& ActorInfo::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_zippylog_2fmessage_2eproto();  return *default_instance_;
}

ActorInfo* ActorInfo::default_instance_ = NULL;

ActorInfo* ActorInfo::New() const {
  return new ActorInfo;
}

void ActorInfo::Clear() {
  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    touch_time_ = GOOGLE_ULONGLONG(0);
    sequence_id_ = GOOGLE_ULONGLONG(0);
    if (_has_bit(2)) {
      if (hostname_ != &_default_hostname_) {
        hostname_->clear();
      }
    }
    if (_has_bit(3)) {
      if (host_id_ != &_default_host_id_) {
        host_id_->clear();
      }
    }
    if (_has_bit(4)) {
      if (actor_id_ != &_default_actor_id_) {
        actor_id_->clear();
      }
    }
    if (_has_bit(5)) {
      if (app_id_ != &_default_app_id_) {
        app_id_->clear();
      }
    }
  }
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  mutable_unknown_fields()->Clear();
}

bool ActorInfo::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) return false
  ::google::protobuf::uint32 tag;
  while ((tag = input->ReadTag()) != 0) {
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // optional uint64 touch_time = 1;
      case 1: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::uint64, ::google::protobuf::internal::WireFormatLite::TYPE_UINT64>(
                 input, &touch_time_)));
          _set_bit(0);
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(16)) goto parse_sequence_id;
        break;
      }
      
      // optional uint64 sequence_id = 2;
      case 2: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
         parse_sequence_id:
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::uint64, ::google::protobuf::internal::WireFormatLite::TYPE_UINT64>(
                 input, &sequence_id_)));
          _set_bit(1);
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(26)) goto parse_hostname;
        break;
      }
      
      // optional string hostname = 3;
      case 3: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
         parse_hostname:
          DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                input, this->mutable_hostname()));
          ::google::protobuf::internal::WireFormat::VerifyUTF8String(
            this->hostname().data(), this->hostname().length(),
            ::google::protobuf::internal::WireFormat::PARSE);
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(34)) goto parse_host_id;
        break;
      }
      
      // optional bytes host_id = 4;
      case 4: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
         parse_host_id:
          DO_(::google::protobuf::internal::WireFormatLite::ReadBytes(
                input, this->mutable_host_id()));
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(42)) goto parse_actor_id;
        break;
      }
      
      // optional bytes actor_id = 5;
      case 5: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
         parse_actor_id:
          DO_(::google::protobuf::internal::WireFormatLite::ReadBytes(
                input, this->mutable_actor_id()));
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(50)) goto parse_app_id;
        break;
      }
      
      // optional bytes app_id = 6;
      case 6: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
         parse_app_id:
          DO_(::google::protobuf::internal::WireFormatLite::ReadBytes(
                input, this->mutable_app_id()));
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectAtEnd()) return true;
        break;
      }
      
      default: {
      handle_uninterpreted:
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_END_GROUP) {
          return true;
        }
        DO_(::google::protobuf::internal::WireFormat::SkipField(
              input, tag, mutable_unknown_fields()));
        break;
      }
    }
  }
  return true;
#undef DO_
}

void ActorInfo::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // optional uint64 touch_time = 1;
  if (_has_bit(0)) {
    ::google::protobuf::internal::WireFormatLite::WriteUInt64(1, this->touch_time(), output);
  }
  
  // optional uint64 sequence_id = 2;
  if (_has_bit(1)) {
    ::google::protobuf::internal::WireFormatLite::WriteUInt64(2, this->sequence_id(), output);
  }
  
  // optional string hostname = 3;
  if (_has_bit(2)) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->hostname().data(), this->hostname().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    ::google::protobuf::internal::WireFormatLite::WriteString(
      3, this->hostname(), output);
  }
  
  // optional bytes host_id = 4;
  if (_has_bit(3)) {
    ::google::protobuf::internal::WireFormatLite::WriteBytes(
      4, this->host_id(), output);
  }
  
  // optional bytes actor_id = 5;
  if (_has_bit(4)) {
    ::google::protobuf::internal::WireFormatLite::WriteBytes(
      5, this->actor_id(), output);
  }
  
  // optional bytes app_id = 6;
  if (_has_bit(5)) {
    ::google::protobuf::internal::WireFormatLite::WriteBytes(
      6, this->app_id(), output);
  }
  
  if (!unknown_fields().empty()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        unknown_fields(), output);
  }
}

::google::protobuf::uint8* ActorInfo::SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* target) const {
  // optional uint64 touch_time = 1;
  if (_has_bit(0)) {
    target = ::google::protobuf::internal::WireFormatLite::WriteUInt64ToArray(1, this->touch_time(), target);
  }
  
  // optional uint64 sequence_id = 2;
  if (_has_bit(1)) {
    target = ::google::protobuf::internal::WireFormatLite::WriteUInt64ToArray(2, this->sequence_id(), target);
  }
  
  // optional string hostname = 3;
  if (_has_bit(2)) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->hostname().data(), this->hostname().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    target =
      ::google::protobuf::internal::WireFormatLite::WriteStringToArray(
        3, this->hostname(), target);
  }
  
  // optional bytes host_id = 4;
  if (_has_bit(3)) {
    target =
      ::google::protobuf::internal::WireFormatLite::WriteBytesToArray(
        4, this->host_id(), target);
  }
  
  // optional bytes actor_id = 5;
  if (_has_bit(4)) {
    target =
      ::google::protobuf::internal::WireFormatLite::WriteBytesToArray(
        5, this->actor_id(), target);
  }
  
  // optional bytes app_id = 6;
  if (_has_bit(5)) {
    target =
      ::google::protobuf::internal::WireFormatLite::WriteBytesToArray(
        6, this->app_id(), target);
  }
  
  if (!unknown_fields().empty()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        unknown_fields(), target);
  }
  return target;
}

int ActorInfo::ByteSize() const {
  int total_size = 0;
  
  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    // optional uint64 touch_time = 1;
    if (has_touch_time()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::UInt64Size(
          this->touch_time());
    }
    
    // optional uint64 sequence_id = 2;
    if (has_sequence_id()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::UInt64Size(
          this->sequence_id());
    }
    
    // optional string hostname = 3;
    if (has_hostname()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::StringSize(
          this->hostname());
    }
    
    // optional bytes host_id = 4;
    if (has_host_id()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::BytesSize(
          this->host_id());
    }
    
    // optional bytes actor_id = 5;
    if (has_actor_id()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::BytesSize(
          this->actor_id());
    }
    
    // optional bytes app_id = 6;
    if (has_app_id()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::BytesSize(
          this->app_id());
    }
    
  }
  if (!unknown_fields().empty()) {
    total_size +=
      ::google::protobuf::internal::WireFormat::ComputeUnknownFieldsSize(
        unknown_fields());
  }
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = total_size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
  return total_size;
}

void ActorInfo::MergeFrom(const ::google::protobuf::Message& from) {
  GOOGLE_CHECK_NE(&from, this);
  const ActorInfo* source =
    ::google::protobuf::internal::dynamic_cast_if_available<const ActorInfo*>(
      &from);
  if (source == NULL) {
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
    MergeFrom(*source);
  }
}

void ActorInfo::MergeFrom(const ActorInfo& from) {
  GOOGLE_CHECK_NE(&from, this);
  if (from._has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (from._has_bit(0)) {
      set_touch_time(from.touch_time());
    }
    if (from._has_bit(1)) {
      set_sequence_id(from.sequence_id());
    }
    if (from._has_bit(2)) {
      set_hostname(from.hostname());
    }
    if (from._has_bit(3)) {
      set_host_id(from.host_id());
    }
    if (from._has_bit(4)) {
      set_actor_id(from.actor_id());
    }
    if (from._has_bit(5)) {
      set_app_id(from.app_id());
    }
  }
  mutable_unknown_fields()->MergeFrom(from.unknown_fields());
}

void ActorInfo::CopyFrom(const ::google::protobuf::Message& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void ActorInfo::CopyFrom(const ActorInfo& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool ActorInfo::IsInitialized() const {
  
  return true;
}

void ActorInfo::Swap(ActorInfo* other) {
  if (other != this) {
    std::swap(touch_time_, other->touch_time_);
    std::swap(sequence_id_, other->sequence_id_);
    std::swap(hostname_, other->hostname_);
    std::swap(host_id_, other->host_id_);
    std::swap(actor_id_, other->actor_id_);
    std::swap(app_id_, other->app_id_);
    std::swap(_has_bits_[0], other->_has_bits_[0]);
    _unknown_fields_.Swap(&other->_unknown_fields_);
    std::swap(_cached_size_, other->_cached_size_);
  }
}

::google::protobuf::Metadata ActorInfo::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = ActorInfo_descriptor_;
  metadata.reflection = ActorInfo_reflection_;
  return metadata;
}


// @@protoc_insertion_point(namespace_scope)

}  // namespace message
}  // namespace zippylog

// @@protoc_insertion_point(global_scope)
