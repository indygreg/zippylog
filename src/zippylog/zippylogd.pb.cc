// Generated by the protocol buffer compiler.  DO NOT EDIT!

#define INTERNAL_SUPPRESS_PROTOBUF_FIELD_DEPRECATION
#include "zippylog/zippylogd.pb.h"
#include <google/protobuf/stubs/once.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/wire_format_lite_inl.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>
// @@protoc_insertion_point(includes)

namespace zippylog {
namespace zippylogd {

namespace {

const ::google::protobuf::Descriptor* BrokerStartup_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  BrokerStartup_reflection_ = NULL;
const ::google::protobuf::Descriptor* BrokerShutdown_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  BrokerShutdown_reflection_ = NULL;
const ::google::protobuf::Descriptor* BrokerReceiveClientMessage_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  BrokerReceiveClientMessage_reflection_ = NULL;

}  // namespace


void protobuf_AssignDesc_zippylog_2fzippylogd_2eproto() {
  protobuf_AddDesc_zippylog_2fzippylogd_2eproto();
  const ::google::protobuf::FileDescriptor* file =
    ::google::protobuf::DescriptorPool::generated_pool()->FindFileByName(
      "zippylog/zippylogd.proto");
  GOOGLE_CHECK(file != NULL);
  BrokerStartup_descriptor_ = file->message_type(0);
  static const int BrokerStartup_offsets_[2] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(BrokerStartup, store_path_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(BrokerStartup, listen_endpoints_),
  };
  BrokerStartup_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      BrokerStartup_descriptor_,
      BrokerStartup::default_instance_,
      BrokerStartup_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(BrokerStartup, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(BrokerStartup, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool(),
      ::google::protobuf::MessageFactory::generated_factory(),
      sizeof(BrokerStartup));
  BrokerShutdown_descriptor_ = file->message_type(1);
  static const int BrokerShutdown_offsets_[1] = {
  };
  BrokerShutdown_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      BrokerShutdown_descriptor_,
      BrokerShutdown::default_instance_,
      BrokerShutdown_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(BrokerShutdown, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(BrokerShutdown, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool(),
      ::google::protobuf::MessageFactory::generated_factory(),
      sizeof(BrokerShutdown));
  BrokerReceiveClientMessage_descriptor_ = file->message_type(2);
  static const int BrokerReceiveClientMessage_offsets_[1] = {
  };
  BrokerReceiveClientMessage_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      BrokerReceiveClientMessage_descriptor_,
      BrokerReceiveClientMessage::default_instance_,
      BrokerReceiveClientMessage_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(BrokerReceiveClientMessage, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(BrokerReceiveClientMessage, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool(),
      ::google::protobuf::MessageFactory::generated_factory(),
      sizeof(BrokerReceiveClientMessage));
}

namespace {

GOOGLE_PROTOBUF_DECLARE_ONCE(protobuf_AssignDescriptors_once_);
inline void protobuf_AssignDescriptorsOnce() {
  ::google::protobuf::GoogleOnceInit(&protobuf_AssignDescriptors_once_,
                 &protobuf_AssignDesc_zippylog_2fzippylogd_2eproto);
}

void protobuf_RegisterTypes(const ::std::string&) {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
    BrokerStartup_descriptor_, &BrokerStartup::default_instance());
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
    BrokerShutdown_descriptor_, &BrokerShutdown::default_instance());
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
    BrokerReceiveClientMessage_descriptor_, &BrokerReceiveClientMessage::default_instance());
}

}  // namespace

void protobuf_ShutdownFile_zippylog_2fzippylogd_2eproto() {
  delete BrokerStartup::default_instance_;
  delete BrokerStartup_reflection_;
  delete BrokerShutdown::default_instance_;
  delete BrokerShutdown_reflection_;
  delete BrokerReceiveClientMessage::default_instance_;
  delete BrokerReceiveClientMessage_reflection_;
}

void protobuf_AddDesc_zippylog_2fzippylogd_2eproto() {
  static bool already_here = false;
  if (already_here) return;
  already_here = true;
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  ::google::protobuf::DescriptorPool::InternalAddGeneratedFile(
    "\n\030zippylog/zippylogd.proto\022\022zippylog.zip"
    "pylogd\"=\n\rBrokerStartup\022\022\n\nstore_path\030\001 "
    "\001(\t\022\030\n\020listen_endpoints\030\002 \003(\t\"\020\n\016BrokerS"
    "hutdown\"\034\n\032BrokerReceiveClientMessage", 157);
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedFile(
    "zippylog/zippylogd.proto", &protobuf_RegisterTypes);
  BrokerStartup::default_instance_ = new BrokerStartup();
  BrokerShutdown::default_instance_ = new BrokerShutdown();
  BrokerReceiveClientMessage::default_instance_ = new BrokerReceiveClientMessage();
  BrokerStartup::default_instance_->InitAsDefaultInstance();
  BrokerShutdown::default_instance_->InitAsDefaultInstance();
  BrokerReceiveClientMessage::default_instance_->InitAsDefaultInstance();
  ::google::protobuf::internal::OnShutdown(&protobuf_ShutdownFile_zippylog_2fzippylogd_2eproto);
}

// Force AddDescriptors() to be called at static initialization time.
struct StaticDescriptorInitializer_zippylog_2fzippylogd_2eproto {
  StaticDescriptorInitializer_zippylog_2fzippylogd_2eproto() {
    protobuf_AddDesc_zippylog_2fzippylogd_2eproto();
  }
} static_descriptor_initializer_zippylog_2fzippylogd_2eproto_;


// ===================================================================

const ::std::string BrokerStartup::_default_store_path_;
#ifndef _MSC_VER
const int BrokerStartup::kStorePathFieldNumber;
const int BrokerStartup::kListenEndpointsFieldNumber;
#endif  // !_MSC_VER

BrokerStartup::BrokerStartup()
  : ::google::protobuf::Message() {
  SharedCtor();
}

void BrokerStartup::InitAsDefaultInstance() {
}

BrokerStartup::BrokerStartup(const BrokerStartup& from)
  : ::google::protobuf::Message() {
  SharedCtor();
  MergeFrom(from);
}

void BrokerStartup::SharedCtor() {
  _cached_size_ = 0;
  store_path_ = const_cast< ::std::string*>(&_default_store_path_);
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

BrokerStartup::~BrokerStartup() {
  SharedDtor();
}

void BrokerStartup::SharedDtor() {
  if (store_path_ != &_default_store_path_) {
    delete store_path_;
  }
  if (this != default_instance_) {
  }
}

void BrokerStartup::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* BrokerStartup::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return BrokerStartup_descriptor_;
}

const BrokerStartup& BrokerStartup::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_zippylog_2fzippylogd_2eproto();  return *default_instance_;
}

BrokerStartup* BrokerStartup::default_instance_ = NULL;

BrokerStartup* BrokerStartup::New() const {
  return new BrokerStartup;
}

void BrokerStartup::Clear() {
  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (_has_bit(0)) {
      if (store_path_ != &_default_store_path_) {
        store_path_->clear();
      }
    }
  }
  listen_endpoints_.Clear();
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  mutable_unknown_fields()->Clear();
}

bool BrokerStartup::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) return false
  ::google::protobuf::uint32 tag;
  while ((tag = input->ReadTag()) != 0) {
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // optional string store_path = 1;
      case 1: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
          DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                input, this->mutable_store_path()));
          ::google::protobuf::internal::WireFormat::VerifyUTF8String(
            this->store_path().data(), this->store_path().length(),
            ::google::protobuf::internal::WireFormat::PARSE);
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(18)) goto parse_listen_endpoints;
        break;
      }
      
      // repeated string listen_endpoints = 2;
      case 2: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
         parse_listen_endpoints:
          DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                input, this->add_listen_endpoints()));
          ::google::protobuf::internal::WireFormat::VerifyUTF8String(
            this->listen_endpoints(0).data(), this->listen_endpoints(0).length(),
            ::google::protobuf::internal::WireFormat::PARSE);
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(18)) goto parse_listen_endpoints;
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

void BrokerStartup::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // optional string store_path = 1;
  if (_has_bit(0)) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->store_path().data(), this->store_path().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    ::google::protobuf::internal::WireFormatLite::WriteString(
      1, this->store_path(), output);
  }
  
  // repeated string listen_endpoints = 2;
  for (int i = 0; i < this->listen_endpoints_size(); i++) {
  ::google::protobuf::internal::WireFormat::VerifyUTF8String(
    this->listen_endpoints(i).data(), this->listen_endpoints(i).length(),
    ::google::protobuf::internal::WireFormat::SERIALIZE);
    ::google::protobuf::internal::WireFormatLite::WriteString(
      2, this->listen_endpoints(i), output);
  }
  
  if (!unknown_fields().empty()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        unknown_fields(), output);
  }
}

::google::protobuf::uint8* BrokerStartup::SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* target) const {
  // optional string store_path = 1;
  if (_has_bit(0)) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->store_path().data(), this->store_path().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    target =
      ::google::protobuf::internal::WireFormatLite::WriteStringToArray(
        1, this->store_path(), target);
  }
  
  // repeated string listen_endpoints = 2;
  for (int i = 0; i < this->listen_endpoints_size(); i++) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->listen_endpoints(i).data(), this->listen_endpoints(i).length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    target = ::google::protobuf::internal::WireFormatLite::
      WriteStringToArray(2, this->listen_endpoints(i), target);
  }
  
  if (!unknown_fields().empty()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        unknown_fields(), target);
  }
  return target;
}

int BrokerStartup::ByteSize() const {
  int total_size = 0;
  
  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    // optional string store_path = 1;
    if (has_store_path()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::StringSize(
          this->store_path());
    }
    
  }
  // repeated string listen_endpoints = 2;
  total_size += 1 * this->listen_endpoints_size();
  for (int i = 0; i < this->listen_endpoints_size(); i++) {
    total_size += ::google::protobuf::internal::WireFormatLite::StringSize(
      this->listen_endpoints(i));
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

void BrokerStartup::MergeFrom(const ::google::protobuf::Message& from) {
  GOOGLE_CHECK_NE(&from, this);
  const BrokerStartup* source =
    ::google::protobuf::internal::dynamic_cast_if_available<const BrokerStartup*>(
      &from);
  if (source == NULL) {
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
    MergeFrom(*source);
  }
}

void BrokerStartup::MergeFrom(const BrokerStartup& from) {
  GOOGLE_CHECK_NE(&from, this);
  listen_endpoints_.MergeFrom(from.listen_endpoints_);
  if (from._has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (from._has_bit(0)) {
      set_store_path(from.store_path());
    }
  }
  mutable_unknown_fields()->MergeFrom(from.unknown_fields());
}

void BrokerStartup::CopyFrom(const ::google::protobuf::Message& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void BrokerStartup::CopyFrom(const BrokerStartup& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool BrokerStartup::IsInitialized() const {
  
  return true;
}

void BrokerStartup::Swap(BrokerStartup* other) {
  if (other != this) {
    std::swap(store_path_, other->store_path_);
    listen_endpoints_.Swap(&other->listen_endpoints_);
    std::swap(_has_bits_[0], other->_has_bits_[0]);
    _unknown_fields_.Swap(&other->_unknown_fields_);
    std::swap(_cached_size_, other->_cached_size_);
  }
}

::google::protobuf::Metadata BrokerStartup::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = BrokerStartup_descriptor_;
  metadata.reflection = BrokerStartup_reflection_;
  return metadata;
}


// ===================================================================

#ifndef _MSC_VER
#endif  // !_MSC_VER

BrokerShutdown::BrokerShutdown()
  : ::google::protobuf::Message() {
  SharedCtor();
}

void BrokerShutdown::InitAsDefaultInstance() {
}

BrokerShutdown::BrokerShutdown(const BrokerShutdown& from)
  : ::google::protobuf::Message() {
  SharedCtor();
  MergeFrom(from);
}

void BrokerShutdown::SharedCtor() {
  _cached_size_ = 0;
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

BrokerShutdown::~BrokerShutdown() {
  SharedDtor();
}

void BrokerShutdown::SharedDtor() {
  if (this != default_instance_) {
  }
}

void BrokerShutdown::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* BrokerShutdown::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return BrokerShutdown_descriptor_;
}

const BrokerShutdown& BrokerShutdown::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_zippylog_2fzippylogd_2eproto();  return *default_instance_;
}

BrokerShutdown* BrokerShutdown::default_instance_ = NULL;

BrokerShutdown* BrokerShutdown::New() const {
  return new BrokerShutdown;
}

void BrokerShutdown::Clear() {
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  mutable_unknown_fields()->Clear();
}

bool BrokerShutdown::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) return false
  ::google::protobuf::uint32 tag;
  while ((tag = input->ReadTag()) != 0) {
    if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
        ::google::protobuf::internal::WireFormatLite::WIRETYPE_END_GROUP) {
      return true;
    }
    DO_(::google::protobuf::internal::WireFormat::SkipField(
          input, tag, mutable_unknown_fields()));
  }
  return true;
#undef DO_
}

void BrokerShutdown::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  if (!unknown_fields().empty()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        unknown_fields(), output);
  }
}

::google::protobuf::uint8* BrokerShutdown::SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* target) const {
  if (!unknown_fields().empty()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        unknown_fields(), target);
  }
  return target;
}

int BrokerShutdown::ByteSize() const {
  int total_size = 0;
  
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

void BrokerShutdown::MergeFrom(const ::google::protobuf::Message& from) {
  GOOGLE_CHECK_NE(&from, this);
  const BrokerShutdown* source =
    ::google::protobuf::internal::dynamic_cast_if_available<const BrokerShutdown*>(
      &from);
  if (source == NULL) {
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
    MergeFrom(*source);
  }
}

void BrokerShutdown::MergeFrom(const BrokerShutdown& from) {
  GOOGLE_CHECK_NE(&from, this);
  mutable_unknown_fields()->MergeFrom(from.unknown_fields());
}

void BrokerShutdown::CopyFrom(const ::google::protobuf::Message& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void BrokerShutdown::CopyFrom(const BrokerShutdown& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool BrokerShutdown::IsInitialized() const {
  
  return true;
}

void BrokerShutdown::Swap(BrokerShutdown* other) {
  if (other != this) {
    _unknown_fields_.Swap(&other->_unknown_fields_);
    std::swap(_cached_size_, other->_cached_size_);
  }
}

::google::protobuf::Metadata BrokerShutdown::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = BrokerShutdown_descriptor_;
  metadata.reflection = BrokerShutdown_reflection_;
  return metadata;
}


// ===================================================================

#ifndef _MSC_VER
#endif  // !_MSC_VER

BrokerReceiveClientMessage::BrokerReceiveClientMessage()
  : ::google::protobuf::Message() {
  SharedCtor();
}

void BrokerReceiveClientMessage::InitAsDefaultInstance() {
}

BrokerReceiveClientMessage::BrokerReceiveClientMessage(const BrokerReceiveClientMessage& from)
  : ::google::protobuf::Message() {
  SharedCtor();
  MergeFrom(from);
}

void BrokerReceiveClientMessage::SharedCtor() {
  _cached_size_ = 0;
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

BrokerReceiveClientMessage::~BrokerReceiveClientMessage() {
  SharedDtor();
}

void BrokerReceiveClientMessage::SharedDtor() {
  if (this != default_instance_) {
  }
}

void BrokerReceiveClientMessage::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* BrokerReceiveClientMessage::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return BrokerReceiveClientMessage_descriptor_;
}

const BrokerReceiveClientMessage& BrokerReceiveClientMessage::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_zippylog_2fzippylogd_2eproto();  return *default_instance_;
}

BrokerReceiveClientMessage* BrokerReceiveClientMessage::default_instance_ = NULL;

BrokerReceiveClientMessage* BrokerReceiveClientMessage::New() const {
  return new BrokerReceiveClientMessage;
}

void BrokerReceiveClientMessage::Clear() {
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  mutable_unknown_fields()->Clear();
}

bool BrokerReceiveClientMessage::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) return false
  ::google::protobuf::uint32 tag;
  while ((tag = input->ReadTag()) != 0) {
    if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
        ::google::protobuf::internal::WireFormatLite::WIRETYPE_END_GROUP) {
      return true;
    }
    DO_(::google::protobuf::internal::WireFormat::SkipField(
          input, tag, mutable_unknown_fields()));
  }
  return true;
#undef DO_
}

void BrokerReceiveClientMessage::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  if (!unknown_fields().empty()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        unknown_fields(), output);
  }
}

::google::protobuf::uint8* BrokerReceiveClientMessage::SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* target) const {
  if (!unknown_fields().empty()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        unknown_fields(), target);
  }
  return target;
}

int BrokerReceiveClientMessage::ByteSize() const {
  int total_size = 0;
  
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

void BrokerReceiveClientMessage::MergeFrom(const ::google::protobuf::Message& from) {
  GOOGLE_CHECK_NE(&from, this);
  const BrokerReceiveClientMessage* source =
    ::google::protobuf::internal::dynamic_cast_if_available<const BrokerReceiveClientMessage*>(
      &from);
  if (source == NULL) {
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
    MergeFrom(*source);
  }
}

void BrokerReceiveClientMessage::MergeFrom(const BrokerReceiveClientMessage& from) {
  GOOGLE_CHECK_NE(&from, this);
  mutable_unknown_fields()->MergeFrom(from.unknown_fields());
}

void BrokerReceiveClientMessage::CopyFrom(const ::google::protobuf::Message& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void BrokerReceiveClientMessage::CopyFrom(const BrokerReceiveClientMessage& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool BrokerReceiveClientMessage::IsInitialized() const {
  
  return true;
}

void BrokerReceiveClientMessage::Swap(BrokerReceiveClientMessage* other) {
  if (other != this) {
    _unknown_fields_.Swap(&other->_unknown_fields_);
    std::swap(_cached_size_, other->_cached_size_);
  }
}

::google::protobuf::Metadata BrokerReceiveClientMessage::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = BrokerReceiveClientMessage_descriptor_;
  metadata.reflection = BrokerReceiveClientMessage_reflection_;
  return metadata;
}



bool BrokerStartup::add_to_envelope(::zippylog::Envelope *envelope) {
    return envelope->AddMessage(*this, zippylog_namespace, zippylog_enumeration);
}

bool BrokerShutdown::add_to_envelope(::zippylog::Envelope *envelope) {
    return envelope->AddMessage(*this, zippylog_namespace, zippylog_enumeration);
}

bool BrokerReceiveClientMessage::add_to_envelope(::zippylog::Envelope *envelope) {
    return envelope->AddMessage(*this, zippylog_namespace, zippylog_enumeration);
}
// @@protoc_insertion_point(namespace_scope)

}  // namespace zippylogd
}  // namespace zippylog

// @@protoc_insertion_point(global_scope)
