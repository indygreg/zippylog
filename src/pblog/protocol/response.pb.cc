// Generated by the protocol buffer compiler.  DO NOT EDIT!

#define INTERNAL_SUPPRESS_PROTOBUF_FIELD_DEPRECATION
#include "pblog/protocol/response.pb.h"
#include <google/protobuf/stubs/once.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/wire_format_lite_inl.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>
// @@protoc_insertion_point(includes)

namespace pblog {
namespace protocol {
namespace response {

namespace {

const ::google::protobuf::Descriptor* StreamSegmentStart_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  StreamSegmentStart_reflection_ = NULL;
const ::google::protobuf::Descriptor* StreamSegmentEnd_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  StreamSegmentEnd_reflection_ = NULL;
const ::google::protobuf::Descriptor* Error_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  Error_reflection_ = NULL;
const ::google::protobuf::EnumDescriptor* ErrorCode_descriptor_ = NULL;

}  // namespace


void protobuf_AssignDesc_pblog_2fprotocol_2fresponse_2eproto() {
  protobuf_AddDesc_pblog_2fprotocol_2fresponse_2eproto();
  const ::google::protobuf::FileDescriptor* file =
    ::google::protobuf::DescriptorPool::generated_pool()->FindFileByName(
      "pblog/protocol/response.proto");
  GOOGLE_CHECK(file != NULL);
  StreamSegmentStart_descriptor_ = file->message_type(0);
  static const int StreamSegmentStart_offsets_[2] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(StreamSegmentStart, path_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(StreamSegmentStart, offset_),
  };
  StreamSegmentStart_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      StreamSegmentStart_descriptor_,
      StreamSegmentStart::default_instance_,
      StreamSegmentStart_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(StreamSegmentStart, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(StreamSegmentStart, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool(),
      ::google::protobuf::MessageFactory::generated_factory(),
      sizeof(StreamSegmentStart));
  StreamSegmentEnd_descriptor_ = file->message_type(1);
  static const int StreamSegmentEnd_offsets_[4] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(StreamSegmentEnd, path_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(StreamSegmentEnd, offset_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(StreamSegmentEnd, bytes_sent_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(StreamSegmentEnd, envelopes_sent_),
  };
  StreamSegmentEnd_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      StreamSegmentEnd_descriptor_,
      StreamSegmentEnd::default_instance_,
      StreamSegmentEnd_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(StreamSegmentEnd, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(StreamSegmentEnd, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool(),
      ::google::protobuf::MessageFactory::generated_factory(),
      sizeof(StreamSegmentEnd));
  Error_descriptor_ = file->message_type(2);
  static const int Error_offsets_[2] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Error, code_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Error, msg_),
  };
  Error_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      Error_descriptor_,
      Error::default_instance_,
      Error_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Error, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Error, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool(),
      ::google::protobuf::MessageFactory::generated_factory(),
      sizeof(Error));
  ErrorCode_descriptor_ = file->enum_type(0);
}

namespace {

GOOGLE_PROTOBUF_DECLARE_ONCE(protobuf_AssignDescriptors_once_);
inline void protobuf_AssignDescriptorsOnce() {
  ::google::protobuf::GoogleOnceInit(&protobuf_AssignDescriptors_once_,
                 &protobuf_AssignDesc_pblog_2fprotocol_2fresponse_2eproto);
}

void protobuf_RegisterTypes(const ::std::string&) {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
    StreamSegmentStart_descriptor_, &StreamSegmentStart::default_instance());
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
    StreamSegmentEnd_descriptor_, &StreamSegmentEnd::default_instance());
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
    Error_descriptor_, &Error::default_instance());
}

}  // namespace

void protobuf_ShutdownFile_pblog_2fprotocol_2fresponse_2eproto() {
  delete StreamSegmentStart::default_instance_;
  delete StreamSegmentStart_reflection_;
  delete StreamSegmentEnd::default_instance_;
  delete StreamSegmentEnd_reflection_;
  delete Error::default_instance_;
  delete Error_reflection_;
}

void protobuf_AddDesc_pblog_2fprotocol_2fresponse_2eproto() {
  static bool already_here = false;
  if (already_here) return;
  already_here = true;
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  ::google::protobuf::DescriptorPool::InternalAddGeneratedFile(
    "\n\035pblog/protocol/response.proto\022\027pblog.p"
    "rotocol.response\"2\n\022StreamSegmentStart\022\014"
    "\n\004path\030\001 \001(\t\022\016\n\006offset\030\002 \001(\004\"\\\n\020StreamSe"
    "gmentEnd\022\014\n\004path\030\001 \001(\t\022\016\n\006offset\030\002 \001(\004\022\022"
    "\n\nbytes_sent\030\003 \001(\r\022\026\n\016envelopes_sent\030\004 \001"
    "(\r\"F\n\005Error\0220\n\004code\030\001 \001(\0162\".pblog.protoc"
    "ol.response.ErrorCode\022\013\n\003msg\030\002 \001(\t*\260\002\n\tE"
    "rrorCode\022\032\n\026ENVELOPE_PARSE_FAILURE\020\001\022\035\n\031"
    "INVALID_MESSAGE_NAMESPACE\020\002\022\030\n\024UNKNOWN_R"
    "EQUEST_TYPE\020\003\022\022\n\016EMPTY_ENVELOPE\020\004\022\030\n\024MIS"
    "SING_ENUMERATIONS\020\005\022\033\n\027REQUEST_NOT_IMPLE"
    "MENTED\020\006\022\017\n\013EMPTY_FIELD\020\007\022\033\n\027FIELD_LENGT"
    "HS_DIFFERENT\020\010\022\031\n\025INVALID_STREAM_OFFSET\020"
    "\t\022\022\n\016INVALID_OFFSET\020\n\022\022\n\016PATH_NOT_FOUND\020"
    "\013\022\022\n\016LIMIT_EXCEEDED\020\014", 581);
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedFile(
    "pblog/protocol/response.proto", &protobuf_RegisterTypes);
  StreamSegmentStart::default_instance_ = new StreamSegmentStart();
  StreamSegmentEnd::default_instance_ = new StreamSegmentEnd();
  Error::default_instance_ = new Error();
  StreamSegmentStart::default_instance_->InitAsDefaultInstance();
  StreamSegmentEnd::default_instance_->InitAsDefaultInstance();
  Error::default_instance_->InitAsDefaultInstance();
  ::google::protobuf::internal::OnShutdown(&protobuf_ShutdownFile_pblog_2fprotocol_2fresponse_2eproto);
}

// Force AddDescriptors() to be called at static initialization time.
struct StaticDescriptorInitializer_pblog_2fprotocol_2fresponse_2eproto {
  StaticDescriptorInitializer_pblog_2fprotocol_2fresponse_2eproto() {
    protobuf_AddDesc_pblog_2fprotocol_2fresponse_2eproto();
  }
} static_descriptor_initializer_pblog_2fprotocol_2fresponse_2eproto_;

const ::google::protobuf::EnumDescriptor* ErrorCode_descriptor() {
  protobuf_AssignDescriptorsOnce();
  return ErrorCode_descriptor_;
}
bool ErrorCode_IsValid(int value) {
  switch(value) {
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
    case 10:
    case 11:
    case 12:
      return true;
    default:
      return false;
  }
}


// ===================================================================

const ::std::string StreamSegmentStart::_default_path_;
#ifndef _MSC_VER
const int StreamSegmentStart::kPathFieldNumber;
const int StreamSegmentStart::kOffsetFieldNumber;
#endif  // !_MSC_VER

StreamSegmentStart::StreamSegmentStart()
  : ::google::protobuf::Message() {
  SharedCtor();
}

void StreamSegmentStart::InitAsDefaultInstance() {
}

StreamSegmentStart::StreamSegmentStart(const StreamSegmentStart& from)
  : ::google::protobuf::Message() {
  SharedCtor();
  MergeFrom(from);
}

void StreamSegmentStart::SharedCtor() {
  _cached_size_ = 0;
  path_ = const_cast< ::std::string*>(&_default_path_);
  offset_ = GOOGLE_ULONGLONG(0);
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

StreamSegmentStart::~StreamSegmentStart() {
  SharedDtor();
}

void StreamSegmentStart::SharedDtor() {
  if (path_ != &_default_path_) {
    delete path_;
  }
  if (this != default_instance_) {
  }
}

void StreamSegmentStart::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* StreamSegmentStart::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return StreamSegmentStart_descriptor_;
}

const StreamSegmentStart& StreamSegmentStart::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_pblog_2fprotocol_2fresponse_2eproto();  return *default_instance_;
}

StreamSegmentStart* StreamSegmentStart::default_instance_ = NULL;

StreamSegmentStart* StreamSegmentStart::New() const {
  return new StreamSegmentStart;
}

void StreamSegmentStart::Clear() {
  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (_has_bit(0)) {
      if (path_ != &_default_path_) {
        path_->clear();
      }
    }
    offset_ = GOOGLE_ULONGLONG(0);
  }
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  mutable_unknown_fields()->Clear();
}

bool StreamSegmentStart::MergePartialFromCodedStream(
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
        if (input->ExpectTag(16)) goto parse_offset;
        break;
      }
      
      // optional uint64 offset = 2;
      case 2: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
         parse_offset:
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::uint64, ::google::protobuf::internal::WireFormatLite::TYPE_UINT64>(
                 input, &offset_)));
          _set_bit(1);
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

void StreamSegmentStart::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // optional string path = 1;
  if (_has_bit(0)) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->path().data(), this->path().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    ::google::protobuf::internal::WireFormatLite::WriteString(
      1, this->path(), output);
  }
  
  // optional uint64 offset = 2;
  if (_has_bit(1)) {
    ::google::protobuf::internal::WireFormatLite::WriteUInt64(2, this->offset(), output);
  }
  
  if (!unknown_fields().empty()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        unknown_fields(), output);
  }
}

::google::protobuf::uint8* StreamSegmentStart::SerializeWithCachedSizesToArray(
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
  
  // optional uint64 offset = 2;
  if (_has_bit(1)) {
    target = ::google::protobuf::internal::WireFormatLite::WriteUInt64ToArray(2, this->offset(), target);
  }
  
  if (!unknown_fields().empty()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        unknown_fields(), target);
  }
  return target;
}

int StreamSegmentStart::ByteSize() const {
  int total_size = 0;
  
  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    // optional string path = 1;
    if (has_path()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::StringSize(
          this->path());
    }
    
    // optional uint64 offset = 2;
    if (has_offset()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::UInt64Size(
          this->offset());
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

void StreamSegmentStart::MergeFrom(const ::google::protobuf::Message& from) {
  GOOGLE_CHECK_NE(&from, this);
  const StreamSegmentStart* source =
    ::google::protobuf::internal::dynamic_cast_if_available<const StreamSegmentStart*>(
      &from);
  if (source == NULL) {
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
    MergeFrom(*source);
  }
}

void StreamSegmentStart::MergeFrom(const StreamSegmentStart& from) {
  GOOGLE_CHECK_NE(&from, this);
  if (from._has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (from._has_bit(0)) {
      set_path(from.path());
    }
    if (from._has_bit(1)) {
      set_offset(from.offset());
    }
  }
  mutable_unknown_fields()->MergeFrom(from.unknown_fields());
}

void StreamSegmentStart::CopyFrom(const ::google::protobuf::Message& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void StreamSegmentStart::CopyFrom(const StreamSegmentStart& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool StreamSegmentStart::IsInitialized() const {
  
  return true;
}

void StreamSegmentStart::Swap(StreamSegmentStart* other) {
  if (other != this) {
    std::swap(path_, other->path_);
    std::swap(offset_, other->offset_);
    std::swap(_has_bits_[0], other->_has_bits_[0]);
    _unknown_fields_.Swap(&other->_unknown_fields_);
    std::swap(_cached_size_, other->_cached_size_);
  }
}

::google::protobuf::Metadata StreamSegmentStart::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = StreamSegmentStart_descriptor_;
  metadata.reflection = StreamSegmentStart_reflection_;
  return metadata;
}


// ===================================================================

const ::std::string StreamSegmentEnd::_default_path_;
#ifndef _MSC_VER
const int StreamSegmentEnd::kPathFieldNumber;
const int StreamSegmentEnd::kOffsetFieldNumber;
const int StreamSegmentEnd::kBytesSentFieldNumber;
const int StreamSegmentEnd::kEnvelopesSentFieldNumber;
#endif  // !_MSC_VER

StreamSegmentEnd::StreamSegmentEnd()
  : ::google::protobuf::Message() {
  SharedCtor();
}

void StreamSegmentEnd::InitAsDefaultInstance() {
}

StreamSegmentEnd::StreamSegmentEnd(const StreamSegmentEnd& from)
  : ::google::protobuf::Message() {
  SharedCtor();
  MergeFrom(from);
}

void StreamSegmentEnd::SharedCtor() {
  _cached_size_ = 0;
  path_ = const_cast< ::std::string*>(&_default_path_);
  offset_ = GOOGLE_ULONGLONG(0);
  bytes_sent_ = 0u;
  envelopes_sent_ = 0u;
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

StreamSegmentEnd::~StreamSegmentEnd() {
  SharedDtor();
}

void StreamSegmentEnd::SharedDtor() {
  if (path_ != &_default_path_) {
    delete path_;
  }
  if (this != default_instance_) {
  }
}

void StreamSegmentEnd::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* StreamSegmentEnd::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return StreamSegmentEnd_descriptor_;
}

const StreamSegmentEnd& StreamSegmentEnd::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_pblog_2fprotocol_2fresponse_2eproto();  return *default_instance_;
}

StreamSegmentEnd* StreamSegmentEnd::default_instance_ = NULL;

StreamSegmentEnd* StreamSegmentEnd::New() const {
  return new StreamSegmentEnd;
}

void StreamSegmentEnd::Clear() {
  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (_has_bit(0)) {
      if (path_ != &_default_path_) {
        path_->clear();
      }
    }
    offset_ = GOOGLE_ULONGLONG(0);
    bytes_sent_ = 0u;
    envelopes_sent_ = 0u;
  }
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  mutable_unknown_fields()->Clear();
}

bool StreamSegmentEnd::MergePartialFromCodedStream(
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
        if (input->ExpectTag(16)) goto parse_offset;
        break;
      }
      
      // optional uint64 offset = 2;
      case 2: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
         parse_offset:
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::uint64, ::google::protobuf::internal::WireFormatLite::TYPE_UINT64>(
                 input, &offset_)));
          _set_bit(1);
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(24)) goto parse_bytes_sent;
        break;
      }
      
      // optional uint32 bytes_sent = 3;
      case 3: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
         parse_bytes_sent:
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::uint32, ::google::protobuf::internal::WireFormatLite::TYPE_UINT32>(
                 input, &bytes_sent_)));
          _set_bit(2);
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(32)) goto parse_envelopes_sent;
        break;
      }
      
      // optional uint32 envelopes_sent = 4;
      case 4: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
         parse_envelopes_sent:
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::uint32, ::google::protobuf::internal::WireFormatLite::TYPE_UINT32>(
                 input, &envelopes_sent_)));
          _set_bit(3);
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

void StreamSegmentEnd::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // optional string path = 1;
  if (_has_bit(0)) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->path().data(), this->path().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    ::google::protobuf::internal::WireFormatLite::WriteString(
      1, this->path(), output);
  }
  
  // optional uint64 offset = 2;
  if (_has_bit(1)) {
    ::google::protobuf::internal::WireFormatLite::WriteUInt64(2, this->offset(), output);
  }
  
  // optional uint32 bytes_sent = 3;
  if (_has_bit(2)) {
    ::google::protobuf::internal::WireFormatLite::WriteUInt32(3, this->bytes_sent(), output);
  }
  
  // optional uint32 envelopes_sent = 4;
  if (_has_bit(3)) {
    ::google::protobuf::internal::WireFormatLite::WriteUInt32(4, this->envelopes_sent(), output);
  }
  
  if (!unknown_fields().empty()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        unknown_fields(), output);
  }
}

::google::protobuf::uint8* StreamSegmentEnd::SerializeWithCachedSizesToArray(
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
  
  // optional uint64 offset = 2;
  if (_has_bit(1)) {
    target = ::google::protobuf::internal::WireFormatLite::WriteUInt64ToArray(2, this->offset(), target);
  }
  
  // optional uint32 bytes_sent = 3;
  if (_has_bit(2)) {
    target = ::google::protobuf::internal::WireFormatLite::WriteUInt32ToArray(3, this->bytes_sent(), target);
  }
  
  // optional uint32 envelopes_sent = 4;
  if (_has_bit(3)) {
    target = ::google::protobuf::internal::WireFormatLite::WriteUInt32ToArray(4, this->envelopes_sent(), target);
  }
  
  if (!unknown_fields().empty()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        unknown_fields(), target);
  }
  return target;
}

int StreamSegmentEnd::ByteSize() const {
  int total_size = 0;
  
  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    // optional string path = 1;
    if (has_path()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::StringSize(
          this->path());
    }
    
    // optional uint64 offset = 2;
    if (has_offset()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::UInt64Size(
          this->offset());
    }
    
    // optional uint32 bytes_sent = 3;
    if (has_bytes_sent()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::UInt32Size(
          this->bytes_sent());
    }
    
    // optional uint32 envelopes_sent = 4;
    if (has_envelopes_sent()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::UInt32Size(
          this->envelopes_sent());
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

void StreamSegmentEnd::MergeFrom(const ::google::protobuf::Message& from) {
  GOOGLE_CHECK_NE(&from, this);
  const StreamSegmentEnd* source =
    ::google::protobuf::internal::dynamic_cast_if_available<const StreamSegmentEnd*>(
      &from);
  if (source == NULL) {
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
    MergeFrom(*source);
  }
}

void StreamSegmentEnd::MergeFrom(const StreamSegmentEnd& from) {
  GOOGLE_CHECK_NE(&from, this);
  if (from._has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (from._has_bit(0)) {
      set_path(from.path());
    }
    if (from._has_bit(1)) {
      set_offset(from.offset());
    }
    if (from._has_bit(2)) {
      set_bytes_sent(from.bytes_sent());
    }
    if (from._has_bit(3)) {
      set_envelopes_sent(from.envelopes_sent());
    }
  }
  mutable_unknown_fields()->MergeFrom(from.unknown_fields());
}

void StreamSegmentEnd::CopyFrom(const ::google::protobuf::Message& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void StreamSegmentEnd::CopyFrom(const StreamSegmentEnd& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool StreamSegmentEnd::IsInitialized() const {
  
  return true;
}

void StreamSegmentEnd::Swap(StreamSegmentEnd* other) {
  if (other != this) {
    std::swap(path_, other->path_);
    std::swap(offset_, other->offset_);
    std::swap(bytes_sent_, other->bytes_sent_);
    std::swap(envelopes_sent_, other->envelopes_sent_);
    std::swap(_has_bits_[0], other->_has_bits_[0]);
    _unknown_fields_.Swap(&other->_unknown_fields_);
    std::swap(_cached_size_, other->_cached_size_);
  }
}

::google::protobuf::Metadata StreamSegmentEnd::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = StreamSegmentEnd_descriptor_;
  metadata.reflection = StreamSegmentEnd_reflection_;
  return metadata;
}


// ===================================================================

const ::std::string Error::_default_msg_;
#ifndef _MSC_VER
const int Error::kCodeFieldNumber;
const int Error::kMsgFieldNumber;
#endif  // !_MSC_VER

Error::Error()
  : ::google::protobuf::Message() {
  SharedCtor();
}

void Error::InitAsDefaultInstance() {
}

Error::Error(const Error& from)
  : ::google::protobuf::Message() {
  SharedCtor();
  MergeFrom(from);
}

void Error::SharedCtor() {
  _cached_size_ = 0;
  code_ = 1;
  msg_ = const_cast< ::std::string*>(&_default_msg_);
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

Error::~Error() {
  SharedDtor();
}

void Error::SharedDtor() {
  if (msg_ != &_default_msg_) {
    delete msg_;
  }
  if (this != default_instance_) {
  }
}

void Error::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* Error::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return Error_descriptor_;
}

const Error& Error::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_pblog_2fprotocol_2fresponse_2eproto();  return *default_instance_;
}

Error* Error::default_instance_ = NULL;

Error* Error::New() const {
  return new Error;
}

void Error::Clear() {
  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    code_ = 1;
    if (_has_bit(1)) {
      if (msg_ != &_default_msg_) {
        msg_->clear();
      }
    }
  }
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  mutable_unknown_fields()->Clear();
}

bool Error::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) return false
  ::google::protobuf::uint32 tag;
  while ((tag = input->ReadTag()) != 0) {
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // optional .pblog.protocol.response.ErrorCode code = 1;
      case 1: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
          int value;
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   int, ::google::protobuf::internal::WireFormatLite::TYPE_ENUM>(
                 input, &value)));
          if (pblog::protocol::response::ErrorCode_IsValid(value)) {
            set_code(static_cast< pblog::protocol::response::ErrorCode >(value));
          } else {
            mutable_unknown_fields()->AddVarint(1, value);
          }
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(18)) goto parse_msg;
        break;
      }
      
      // optional string msg = 2;
      case 2: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
         parse_msg:
          DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                input, this->mutable_msg()));
          ::google::protobuf::internal::WireFormat::VerifyUTF8String(
            this->msg().data(), this->msg().length(),
            ::google::protobuf::internal::WireFormat::PARSE);
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

void Error::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // optional .pblog.protocol.response.ErrorCode code = 1;
  if (_has_bit(0)) {
    ::google::protobuf::internal::WireFormatLite::WriteEnum(
      1, this->code(), output);
  }
  
  // optional string msg = 2;
  if (_has_bit(1)) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->msg().data(), this->msg().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    ::google::protobuf::internal::WireFormatLite::WriteString(
      2, this->msg(), output);
  }
  
  if (!unknown_fields().empty()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        unknown_fields(), output);
  }
}

::google::protobuf::uint8* Error::SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* target) const {
  // optional .pblog.protocol.response.ErrorCode code = 1;
  if (_has_bit(0)) {
    target = ::google::protobuf::internal::WireFormatLite::WriteEnumToArray(
      1, this->code(), target);
  }
  
  // optional string msg = 2;
  if (_has_bit(1)) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->msg().data(), this->msg().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    target =
      ::google::protobuf::internal::WireFormatLite::WriteStringToArray(
        2, this->msg(), target);
  }
  
  if (!unknown_fields().empty()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        unknown_fields(), target);
  }
  return target;
}

int Error::ByteSize() const {
  int total_size = 0;
  
  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    // optional .pblog.protocol.response.ErrorCode code = 1;
    if (has_code()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::EnumSize(this->code());
    }
    
    // optional string msg = 2;
    if (has_msg()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::StringSize(
          this->msg());
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

void Error::MergeFrom(const ::google::protobuf::Message& from) {
  GOOGLE_CHECK_NE(&from, this);
  const Error* source =
    ::google::protobuf::internal::dynamic_cast_if_available<const Error*>(
      &from);
  if (source == NULL) {
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
    MergeFrom(*source);
  }
}

void Error::MergeFrom(const Error& from) {
  GOOGLE_CHECK_NE(&from, this);
  if (from._has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (from._has_bit(0)) {
      set_code(from.code());
    }
    if (from._has_bit(1)) {
      set_msg(from.msg());
    }
  }
  mutable_unknown_fields()->MergeFrom(from.unknown_fields());
}

void Error::CopyFrom(const ::google::protobuf::Message& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void Error::CopyFrom(const Error& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool Error::IsInitialized() const {
  
  return true;
}

void Error::Swap(Error* other) {
  if (other != this) {
    std::swap(code_, other->code_);
    std::swap(msg_, other->msg_);
    std::swap(_has_bits_[0], other->_has_bits_[0]);
    _unknown_fields_.Swap(&other->_unknown_fields_);
    std::swap(_cached_size_, other->_cached_size_);
  }
}

::google::protobuf::Metadata Error::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = Error_descriptor_;
  metadata.reflection = Error_reflection_;
  return metadata;
}



bool StreamSegmentStart::add_to_envelope(::pblog::Envelope *envelope) {
    return envelope->add_message(this, pblog_namespace, pblog_enumeration);
}

bool StreamSegmentEnd::add_to_envelope(::pblog::Envelope *envelope) {
    return envelope->add_message(this, pblog_namespace, pblog_enumeration);
}

bool Error::add_to_envelope(::pblog::Envelope *envelope) {
    return envelope->add_message(this, pblog_namespace, pblog_enumeration);
}
// @@protoc_insertion_point(namespace_scope)

}  // namespace response
}  // namespace protocol
}  // namespace pblog

// @@protoc_insertion_point(global_scope)
