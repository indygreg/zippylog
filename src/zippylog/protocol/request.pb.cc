// Generated by the protocol buffer compiler.  DO NOT EDIT!

#define INTERNAL_SUPPRESS_PROTOBUF_FIELD_DEPRECATION
#include "zippylog/protocol/request.pb.h"
#include <google/protobuf/stubs/once.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/wire_format_lite_inl.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>
// @@protoc_insertion_point(includes)

namespace zippylog {
namespace protocol {
namespace request {

namespace {

const ::google::protobuf::Descriptor* StoreInfo_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  StoreInfo_reflection_ = NULL;
const ::google::protobuf::Descriptor* Get_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  Get_reflection_ = NULL;

}  // namespace


void protobuf_AssignDesc_zippylog_2fprotocol_2frequest_2eproto() {
  protobuf_AddDesc_zippylog_2fprotocol_2frequest_2eproto();
  const ::google::protobuf::FileDescriptor* file =
    ::google::protobuf::DescriptorPool::generated_pool()->FindFileByName(
      "zippylog/protocol/request.proto");
  GOOGLE_CHECK(file != NULL);
  StoreInfo_descriptor_ = file->message_type(0);
  static const int StoreInfo_offsets_[1] = {
  };
  StoreInfo_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      StoreInfo_descriptor_,
      StoreInfo::default_instance_,
      StoreInfo_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(StoreInfo, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(StoreInfo, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool(),
      ::google::protobuf::MessageFactory::generated_factory(),
      sizeof(StoreInfo));
  Get_descriptor_ = file->message_type(1);
  static const int Get_offsets_[3] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Get, path_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Get, start_offset_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Get, max_response_bytes_),
  };
  Get_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      Get_descriptor_,
      Get::default_instance_,
      Get_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Get, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Get, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool(),
      ::google::protobuf::MessageFactory::generated_factory(),
      sizeof(Get));
}

namespace {

GOOGLE_PROTOBUF_DECLARE_ONCE(protobuf_AssignDescriptors_once_);
inline void protobuf_AssignDescriptorsOnce() {
  ::google::protobuf::GoogleOnceInit(&protobuf_AssignDescriptors_once_,
                 &protobuf_AssignDesc_zippylog_2fprotocol_2frequest_2eproto);
}

void protobuf_RegisterTypes(const ::std::string&) {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
    StoreInfo_descriptor_, &StoreInfo::default_instance());
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
    Get_descriptor_, &Get::default_instance());
}

}  // namespace

void protobuf_ShutdownFile_zippylog_2fprotocol_2frequest_2eproto() {
  delete StoreInfo::default_instance_;
  delete StoreInfo_reflection_;
  delete Get::default_instance_;
  delete Get_reflection_;
}

void protobuf_AddDesc_zippylog_2fprotocol_2frequest_2eproto() {
  static bool already_here = false;
  if (already_here) return;
  already_here = true;
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  ::google::protobuf::DescriptorPool::InternalAddGeneratedFile(
    "\n\037zippylog/protocol/request.proto\022\031zippy"
    "log.protocol.request\"\013\n\tStoreInfo\"E\n\003Get"
    "\022\014\n\004path\030\001 \001(\t\022\024\n\014start_offset\030\002 \001(\004\022\032\n\022"
    "max_response_bytes\030\003 \001(\r", 144);
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedFile(
    "zippylog/protocol/request.proto", &protobuf_RegisterTypes);
  StoreInfo::default_instance_ = new StoreInfo();
  Get::default_instance_ = new Get();
  StoreInfo::default_instance_->InitAsDefaultInstance();
  Get::default_instance_->InitAsDefaultInstance();
  ::google::protobuf::internal::OnShutdown(&protobuf_ShutdownFile_zippylog_2fprotocol_2frequest_2eproto);
}

// Force AddDescriptors() to be called at static initialization time.
struct StaticDescriptorInitializer_zippylog_2fprotocol_2frequest_2eproto {
  StaticDescriptorInitializer_zippylog_2fprotocol_2frequest_2eproto() {
    protobuf_AddDesc_zippylog_2fprotocol_2frequest_2eproto();
  }
} static_descriptor_initializer_zippylog_2fprotocol_2frequest_2eproto_;


// ===================================================================

#ifndef _MSC_VER
#endif  // !_MSC_VER

StoreInfo::StoreInfo()
  : ::google::protobuf::Message() {
  SharedCtor();
}

void StoreInfo::InitAsDefaultInstance() {
}

StoreInfo::StoreInfo(const StoreInfo& from)
  : ::google::protobuf::Message() {
  SharedCtor();
  MergeFrom(from);
}

void StoreInfo::SharedCtor() {
  _cached_size_ = 0;
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

StoreInfo::~StoreInfo() {
  SharedDtor();
}

void StoreInfo::SharedDtor() {
  if (this != default_instance_) {
  }
}

void StoreInfo::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* StoreInfo::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return StoreInfo_descriptor_;
}

const StoreInfo& StoreInfo::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_zippylog_2fprotocol_2frequest_2eproto();  return *default_instance_;
}

StoreInfo* StoreInfo::default_instance_ = NULL;

StoreInfo* StoreInfo::New() const {
  return new StoreInfo;
}

void StoreInfo::Clear() {
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  mutable_unknown_fields()->Clear();
}

bool StoreInfo::MergePartialFromCodedStream(
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

void StoreInfo::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  if (!unknown_fields().empty()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        unknown_fields(), output);
  }
}

::google::protobuf::uint8* StoreInfo::SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* target) const {
  if (!unknown_fields().empty()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        unknown_fields(), target);
  }
  return target;
}

int StoreInfo::ByteSize() const {
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

void StoreInfo::MergeFrom(const ::google::protobuf::Message& from) {
  GOOGLE_CHECK_NE(&from, this);
  const StoreInfo* source =
    ::google::protobuf::internal::dynamic_cast_if_available<const StoreInfo*>(
      &from);
  if (source == NULL) {
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
    MergeFrom(*source);
  }
}

void StoreInfo::MergeFrom(const StoreInfo& from) {
  GOOGLE_CHECK_NE(&from, this);
  mutable_unknown_fields()->MergeFrom(from.unknown_fields());
}

void StoreInfo::CopyFrom(const ::google::protobuf::Message& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void StoreInfo::CopyFrom(const StoreInfo& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool StoreInfo::IsInitialized() const {
  
  return true;
}

void StoreInfo::Swap(StoreInfo* other) {
  if (other != this) {
    _unknown_fields_.Swap(&other->_unknown_fields_);
    std::swap(_cached_size_, other->_cached_size_);
  }
}

::google::protobuf::Metadata StoreInfo::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = StoreInfo_descriptor_;
  metadata.reflection = StoreInfo_reflection_;
  return metadata;
}


// ===================================================================

const ::std::string Get::_default_path_;
#ifndef _MSC_VER
const int Get::kPathFieldNumber;
const int Get::kStartOffsetFieldNumber;
const int Get::kMaxResponseBytesFieldNumber;
#endif  // !_MSC_VER

Get::Get()
  : ::google::protobuf::Message() {
  SharedCtor();
}

void Get::InitAsDefaultInstance() {
}

Get::Get(const Get& from)
  : ::google::protobuf::Message() {
  SharedCtor();
  MergeFrom(from);
}

void Get::SharedCtor() {
  _cached_size_ = 0;
  path_ = const_cast< ::std::string*>(&_default_path_);
  start_offset_ = GOOGLE_ULONGLONG(0);
  max_response_bytes_ = 0u;
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

Get::~Get() {
  SharedDtor();
}

void Get::SharedDtor() {
  if (path_ != &_default_path_) {
    delete path_;
  }
  if (this != default_instance_) {
  }
}

void Get::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* Get::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return Get_descriptor_;
}

const Get& Get::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_zippylog_2fprotocol_2frequest_2eproto();  return *default_instance_;
}

Get* Get::default_instance_ = NULL;

Get* Get::New() const {
  return new Get;
}

void Get::Clear() {
  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (_has_bit(0)) {
      if (path_ != &_default_path_) {
        path_->clear();
      }
    }
    start_offset_ = GOOGLE_ULONGLONG(0);
    max_response_bytes_ = 0u;
  }
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  mutable_unknown_fields()->Clear();
}

bool Get::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) return false
  ::google::protobuf::uint32 tag;
  while ((tag = input->ReadTag()) != 0) {
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // optional string path = 1;
      case 1: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
          DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                input, this->mutable_path()));
          ::google::protobuf::internal::WireFormat::VerifyUTF8String(
            this->path().data(), this->path().length(),
            ::google::protobuf::internal::WireFormat::PARSE);
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(16)) goto parse_start_offset;
        break;
      }
      
      // optional uint64 start_offset = 2;
      case 2: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
         parse_start_offset:
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::uint64, ::google::protobuf::internal::WireFormatLite::TYPE_UINT64>(
                 input, &start_offset_)));
          _set_bit(1);
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(24)) goto parse_max_response_bytes;
        break;
      }
      
      // optional uint32 max_response_bytes = 3;
      case 3: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
         parse_max_response_bytes:
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::uint32, ::google::protobuf::internal::WireFormatLite::TYPE_UINT32>(
                 input, &max_response_bytes_)));
          _set_bit(2);
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

void Get::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // optional string path = 1;
  if (_has_bit(0)) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->path().data(), this->path().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    ::google::protobuf::internal::WireFormatLite::WriteString(
      1, this->path(), output);
  }
  
  // optional uint64 start_offset = 2;
  if (_has_bit(1)) {
    ::google::protobuf::internal::WireFormatLite::WriteUInt64(2, this->start_offset(), output);
  }
  
  // optional uint32 max_response_bytes = 3;
  if (_has_bit(2)) {
    ::google::protobuf::internal::WireFormatLite::WriteUInt32(3, this->max_response_bytes(), output);
  }
  
  if (!unknown_fields().empty()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        unknown_fields(), output);
  }
}

::google::protobuf::uint8* Get::SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* target) const {
  // optional string path = 1;
  if (_has_bit(0)) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->path().data(), this->path().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    target =
      ::google::protobuf::internal::WireFormatLite::WriteStringToArray(
        1, this->path(), target);
  }
  
  // optional uint64 start_offset = 2;
  if (_has_bit(1)) {
    target = ::google::protobuf::internal::WireFormatLite::WriteUInt64ToArray(2, this->start_offset(), target);
  }
  
  // optional uint32 max_response_bytes = 3;
  if (_has_bit(2)) {
    target = ::google::protobuf::internal::WireFormatLite::WriteUInt32ToArray(3, this->max_response_bytes(), target);
  }
  
  if (!unknown_fields().empty()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        unknown_fields(), target);
  }
  return target;
}

int Get::ByteSize() const {
  int total_size = 0;
  
  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    // optional string path = 1;
    if (has_path()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::StringSize(
          this->path());
    }
    
    // optional uint64 start_offset = 2;
    if (has_start_offset()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::UInt64Size(
          this->start_offset());
    }
    
    // optional uint32 max_response_bytes = 3;
    if (has_max_response_bytes()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::UInt32Size(
          this->max_response_bytes());
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

void Get::MergeFrom(const ::google::protobuf::Message& from) {
  GOOGLE_CHECK_NE(&from, this);
  const Get* source =
    ::google::protobuf::internal::dynamic_cast_if_available<const Get*>(
      &from);
  if (source == NULL) {
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
    MergeFrom(*source);
  }
}

void Get::MergeFrom(const Get& from) {
  GOOGLE_CHECK_NE(&from, this);
  if (from._has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (from._has_bit(0)) {
      set_path(from.path());
    }
    if (from._has_bit(1)) {
      set_start_offset(from.start_offset());
    }
    if (from._has_bit(2)) {
      set_max_response_bytes(from.max_response_bytes());
    }
  }
  mutable_unknown_fields()->MergeFrom(from.unknown_fields());
}

void Get::CopyFrom(const ::google::protobuf::Message& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void Get::CopyFrom(const Get& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool Get::IsInitialized() const {
  
  return true;
}

void Get::Swap(Get* other) {
  if (other != this) {
    std::swap(path_, other->path_);
    std::swap(start_offset_, other->start_offset_);
    std::swap(max_response_bytes_, other->max_response_bytes_);
    std::swap(_has_bits_[0], other->_has_bits_[0]);
    _unknown_fields_.Swap(&other->_unknown_fields_);
    std::swap(_cached_size_, other->_cached_size_);
  }
}

::google::protobuf::Metadata Get::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = Get_descriptor_;
  metadata.reflection = Get_reflection_;
  return metadata;
}


// @@protoc_insertion_point(namespace_scope)

}  // namespace request
}  // namespace protocol
}  // namespace zippylog

// @@protoc_insertion_point(global_scope)
