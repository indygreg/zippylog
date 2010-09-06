// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: zippylog/protocol/request.proto

#ifndef PROTOBUF_zippylog_2fprotocol_2frequest_2eproto__INCLUDED
#define PROTOBUF_zippylog_2fprotocol_2frequest_2eproto__INCLUDED

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
#include <zippylog/envelope.hpp>
// @@protoc_insertion_point(includes)

namespace zippylog {
namespace protocol {
namespace request {

// Internal implementation detail -- do not call these.
void  protobuf_AddDesc_zippylog_2fprotocol_2frequest_2eproto();
void protobuf_AssignDesc_zippylog_2fprotocol_2frequest_2eproto();
void protobuf_ShutdownFile_zippylog_2fprotocol_2frequest_2eproto();

class StoreInfo;
class Get;

// ===================================================================

class StoreInfo : public ::google::protobuf::Message {
 public:
  StoreInfo();
  virtual ~StoreInfo();
  
  StoreInfo(const StoreInfo& from);
  
  inline StoreInfo& operator=(const StoreInfo& from) {
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
  static const StoreInfo& default_instance();
  
  void Swap(StoreInfo* other);
  
  // implements Message ----------------------------------------------
  
  StoreInfo* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const StoreInfo& from);
  void MergeFrom(const StoreInfo& from);
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
  
  static const ::google::protobuf::uint32 zippylog_namespace = 1;
  static const ::google::protobuf::uint32 zippylog_enumeration = 10;
  bool add_to_envelope(::zippylog::Envelope *envelope);
  // @@protoc_insertion_point(class_scope:zippylog.protocol.request.StoreInfo)
 private:
  ::google::protobuf::UnknownFieldSet _unknown_fields_;
  mutable int _cached_size_;
  
  friend void  protobuf_AddDesc_zippylog_2fprotocol_2frequest_2eproto();
  friend void protobuf_AssignDesc_zippylog_2fprotocol_2frequest_2eproto();
  friend void protobuf_ShutdownFile_zippylog_2fprotocol_2frequest_2eproto();
  
  ::google::protobuf::uint32 _has_bits_[1];
  
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
  static StoreInfo* default_instance_;
};
// -------------------------------------------------------------------

class Get : public ::google::protobuf::Message {
 public:
  Get();
  virtual ~Get();
  
  Get(const Get& from);
  
  inline Get& operator=(const Get& from) {
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
  static const Get& default_instance();
  
  void Swap(Get* other);
  
  // implements Message ----------------------------------------------
  
  Get* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const Get& from);
  void MergeFrom(const Get& from);
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
  
  // optional string path = 1;
  inline bool has_path() const;
  inline void clear_path();
  static const int kPathFieldNumber = 1;
  inline const ::std::string& path() const;
  inline void set_path(const ::std::string& value);
  inline void set_path(const char* value);
  inline void set_path(const char* value, size_t size);
  inline ::std::string* mutable_path();
  
  // optional uint64 start_offset = 2;
  inline bool has_start_offset() const;
  inline void clear_start_offset();
  static const int kStartOffsetFieldNumber = 2;
  inline ::google::protobuf::uint64 start_offset() const;
  inline void set_start_offset(::google::protobuf::uint64 value);
  
  // optional uint32 max_response_bytes = 3;
  inline bool has_max_response_bytes() const;
  inline void clear_max_response_bytes();
  static const int kMaxResponseBytesFieldNumber = 3;
  inline ::google::protobuf::uint32 max_response_bytes() const;
  inline void set_max_response_bytes(::google::protobuf::uint32 value);
  
  static const ::google::protobuf::uint32 zippylog_namespace = 1;
  static const ::google::protobuf::uint32 zippylog_enumeration = 11;
  bool add_to_envelope(::zippylog::Envelope *envelope);
  // @@protoc_insertion_point(class_scope:zippylog.protocol.request.Get)
 private:
  ::google::protobuf::UnknownFieldSet _unknown_fields_;
  mutable int _cached_size_;
  
  ::std::string* path_;
  static const ::std::string _default_path_;
  ::google::protobuf::uint64 start_offset_;
  ::google::protobuf::uint32 max_response_bytes_;
  friend void  protobuf_AddDesc_zippylog_2fprotocol_2frequest_2eproto();
  friend void protobuf_AssignDesc_zippylog_2fprotocol_2frequest_2eproto();
  friend void protobuf_ShutdownFile_zippylog_2fprotocol_2frequest_2eproto();
  
  ::google::protobuf::uint32 _has_bits_[(3 + 31) / 32];
  
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
  static Get* default_instance_;
};
// ===================================================================


// ===================================================================

// StoreInfo

// -------------------------------------------------------------------

// Get

// optional string path = 1;
inline bool Get::has_path() const {
  return _has_bit(0);
}
inline void Get::clear_path() {
  if (path_ != &_default_path_) {
    path_->clear();
  }
  _clear_bit(0);
}
inline const ::std::string& Get::path() const {
  return *path_;
}
inline void Get::set_path(const ::std::string& value) {
  _set_bit(0);
  if (path_ == &_default_path_) {
    path_ = new ::std::string;
  }
  path_->assign(value);
}
inline void Get::set_path(const char* value) {
  _set_bit(0);
  if (path_ == &_default_path_) {
    path_ = new ::std::string;
  }
  path_->assign(value);
}
inline void Get::set_path(const char* value, size_t size) {
  _set_bit(0);
  if (path_ == &_default_path_) {
    path_ = new ::std::string;
  }
  path_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* Get::mutable_path() {
  _set_bit(0);
  if (path_ == &_default_path_) {
    path_ = new ::std::string;
  }
  return path_;
}

// optional uint64 start_offset = 2;
inline bool Get::has_start_offset() const {
  return _has_bit(1);
}
inline void Get::clear_start_offset() {
  start_offset_ = GOOGLE_ULONGLONG(0);
  _clear_bit(1);
}
inline ::google::protobuf::uint64 Get::start_offset() const {
  return start_offset_;
}
inline void Get::set_start_offset(::google::protobuf::uint64 value) {
  _set_bit(1);
  start_offset_ = value;
}

// optional uint32 max_response_bytes = 3;
inline bool Get::has_max_response_bytes() const {
  return _has_bit(2);
}
inline void Get::clear_max_response_bytes() {
  max_response_bytes_ = 0u;
  _clear_bit(2);
}
inline ::google::protobuf::uint32 Get::max_response_bytes() const {
  return max_response_bytes_;
}
inline void Get::set_max_response_bytes(::google::protobuf::uint32 value) {
  _set_bit(2);
  max_response_bytes_ = value;
}


// @@protoc_insertion_point(namespace_scope)

}  // namespace request
}  // namespace protocol
}  // namespace zippylog

#ifndef SWIG
namespace google {
namespace protobuf {


}  // namespace google
}  // namespace protobuf
#endif  // SWIG

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_zippylog_2fprotocol_2frequest_2eproto__INCLUDED
