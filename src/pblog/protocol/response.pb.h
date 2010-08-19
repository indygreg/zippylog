// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: pblog/protocol/response.proto

#ifndef PROTOBUF_pblog_2fprotocol_2fresponse_2eproto__INCLUDED
#define PROTOBUF_pblog_2fprotocol_2fresponse_2eproto__INCLUDED

#include <string>

#include <google/protobuf/stubs/common.h>

#if GOOGLE_PROTOBUF_VERSION < 2003000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please update
#error your headers.
#endif
#if 2003000 < GOOGLE_PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/repeated_field.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/generated_message_reflection.h>
#include <pblog/envelope.hpp>
// @@protoc_insertion_point(includes)

namespace pblog {
namespace protocol {
namespace response {

// Internal implementation detail -- do not call these.
void  protobuf_AddDesc_pblog_2fprotocol_2fresponse_2eproto();
void protobuf_AssignDesc_pblog_2fprotocol_2fresponse_2eproto();
void protobuf_ShutdownFile_pblog_2fprotocol_2fresponse_2eproto();

class Error;

enum ErrorCode {
  ENVELOPE_PARSE_FAILURE = 1,
  INVALID_MESSAGE_NAMESPACE = 2,
  UNKNOWN_REQUEST_TYPE = 3,
  EMPTY_ENVELOPE = 4,
  MISSING_ENUMERATIONS = 5,
  REQUEST_NOT_IMPLEMENTED = 6,
  EMPTY_FIELD = 7,
  FIELD_LENGTHS_DIFFERENT = 8,
  INVALID_STREAM_OFFSET = 9,
  INVALID_OFFSET = 10
};
bool ErrorCode_IsValid(int value);
const ErrorCode ErrorCode_MIN = ENVELOPE_PARSE_FAILURE;
const ErrorCode ErrorCode_MAX = INVALID_OFFSET;
const int ErrorCode_ARRAYSIZE = ErrorCode_MAX + 1;

const ::google::protobuf::EnumDescriptor* ErrorCode_descriptor();
inline const ::std::string& ErrorCode_Name(ErrorCode value) {
  return ::google::protobuf::internal::NameOfEnum(
    ErrorCode_descriptor(), value);
}
inline bool ErrorCode_Parse(
    const ::std::string& name, ErrorCode* value) {
  return ::google::protobuf::internal::ParseNamedEnum<ErrorCode>(
    ErrorCode_descriptor(), name, value);
}
// ===================================================================

class Error : public ::google::protobuf::Message {
 public:
  Error();
  virtual ~Error();
  
  Error(const Error& from);
  
  inline Error& operator=(const Error& from) {
    CopyFrom(from);
    return *this;
  }
  
  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _unknown_fields_;
  }
  
  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return &_unknown_fields_;
  }
  
  static const ::google::protobuf::Descriptor* descriptor();
  static const Error& default_instance();
  
  void Swap(Error* other);
  
  // implements Message ----------------------------------------------
  
  Error* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const Error& from);
  void MergeFrom(const Error& from);
  void Clear();
  bool IsInitialized() const;
  
  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const;
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  public:
  
  ::google::protobuf::Metadata GetMetadata() const;
  
  // nested types ----------------------------------------------------
  
  // accessors -------------------------------------------------------
  
  // optional .pblog.protocol.response.ErrorCode code = 1;
  inline bool has_code() const;
  inline void clear_code();
  static const int kCodeFieldNumber = 1;
  inline pblog::protocol::response::ErrorCode code() const;
  inline void set_code(pblog::protocol::response::ErrorCode value);
  
  // optional string msg = 2;
  inline bool has_msg() const;
  inline void clear_msg();
  static const int kMsgFieldNumber = 2;
  inline const ::std::string& msg() const;
  inline void set_msg(const ::std::string& value);
  inline void set_msg(const char* value);
  inline void set_msg(const char* value, size_t size);
  inline ::std::string* mutable_msg();
  
  static const ::google::protobuf::uint32 pblog_namespace = 1;
  static const ::google::protobuf::uint32 pblog_enumeration = 7;
  bool add_to_envelope(::pblog::Envelope *envelope);
  // @@protoc_insertion_point(class_scope:pblog.protocol.response.Error)
 private:
  ::google::protobuf::UnknownFieldSet _unknown_fields_;
  mutable int _cached_size_;
  
  int code_;
  ::std::string* msg_;
  static const ::std::string _default_msg_;
  friend void  protobuf_AddDesc_pblog_2fprotocol_2fresponse_2eproto();
  friend void protobuf_AssignDesc_pblog_2fprotocol_2fresponse_2eproto();
  friend void protobuf_ShutdownFile_pblog_2fprotocol_2fresponse_2eproto();
  
  ::google::protobuf::uint32 _has_bits_[(2 + 31) / 32];
  
  // WHY DOES & HAVE LOWER PRECEDENCE THAN != !?
  inline bool _has_bit(int index) const {
    return (_has_bits_[index / 32] & (1u << (index % 32))) != 0;
  }
  inline void _set_bit(int index) {
    _has_bits_[index / 32] |= (1u << (index % 32));
  }
  inline void _clear_bit(int index) {
    _has_bits_[index / 32] &= ~(1u << (index % 32));
  }
  
  void InitAsDefaultInstance();
  static Error* default_instance_;
};
// ===================================================================


// ===================================================================

// Error

// optional .pblog.protocol.response.ErrorCode code = 1;
inline bool Error::has_code() const {
  return _has_bit(0);
}
inline void Error::clear_code() {
  code_ = 1;
  _clear_bit(0);
}
inline pblog::protocol::response::ErrorCode Error::code() const {
  return static_cast< pblog::protocol::response::ErrorCode >(code_);
}
inline void Error::set_code(pblog::protocol::response::ErrorCode value) {
  GOOGLE_DCHECK(pblog::protocol::response::ErrorCode_IsValid(value));
  _set_bit(0);
  code_ = value;
}

// optional string msg = 2;
inline bool Error::has_msg() const {
  return _has_bit(1);
}
inline void Error::clear_msg() {
  if (msg_ != &_default_msg_) {
    msg_->clear();
  }
  _clear_bit(1);
}
inline const ::std::string& Error::msg() const {
  return *msg_;
}
inline void Error::set_msg(const ::std::string& value) {
  _set_bit(1);
  if (msg_ == &_default_msg_) {
    msg_ = new ::std::string;
  }
  msg_->assign(value);
}
inline void Error::set_msg(const char* value) {
  _set_bit(1);
  if (msg_ == &_default_msg_) {
    msg_ = new ::std::string;
  }
  msg_->assign(value);
}
inline void Error::set_msg(const char* value, size_t size) {
  _set_bit(1);
  if (msg_ == &_default_msg_) {
    msg_ = new ::std::string;
  }
  msg_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* Error::mutable_msg() {
  _set_bit(1);
  if (msg_ == &_default_msg_) {
    msg_ = new ::std::string;
  }
  return msg_;
}


// @@protoc_insertion_point(namespace_scope)

}  // namespace response
}  // namespace protocol
}  // namespace pblog

#ifndef SWIG
namespace google {
namespace protobuf {

template <>
inline const EnumDescriptor* GetEnumDescriptor< pblog::protocol::response::ErrorCode>() {
  return pblog::protocol::response::ErrorCode_descriptor();
}

}  // namespace google
}  // namespace protobuf
#endif  // SWIG

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_pblog_2fprotocol_2fresponse_2eproto__INCLUDED
