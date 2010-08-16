// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: pblog/protocol.proto

#ifndef PROTOBUF_pblog_2fprotocol_2eproto__INCLUDED
#define PROTOBUF_pblog_2fprotocol_2eproto__INCLUDED

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

// Internal implementation detail -- do not call these.
void  protobuf_AddDesc_pblog_2fprotocol_2eproto();
void protobuf_AssignDesc_pblog_2fprotocol_2eproto();
void protobuf_ShutdownFile_pblog_2fprotocol_2eproto();

class StreamInfo;
class StreamSetInfo;
class BucketInfo;
class StoreInfo;

// ===================================================================

class StreamInfo : public ::google::protobuf::Message {
 public:
  StreamInfo();
  virtual ~StreamInfo();
  
  StreamInfo(const StreamInfo& from);
  
  inline StreamInfo& operator=(const StreamInfo& from) {
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
  static const StreamInfo& default_instance();
  
  void Swap(StreamInfo* other);
  
  // implements Message ----------------------------------------------
  
  StreamInfo* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const StreamInfo& from);
  void MergeFrom(const StreamInfo& from);
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
  
  // optional uint64 length = 2;
  inline bool has_length() const;
  inline void clear_length();
  static const int kLengthFieldNumber = 2;
  inline ::google::protobuf::uint64 length() const;
  inline void set_length(::google::protobuf::uint64 value);
  
  static const ::google::protobuf::uint32 pblog_namespace = 1;
  static const ::google::protobuf::uint32 pblog_enumeration = 5;
  // @@protoc_insertion_point(class_scope:pblog.protocol.StreamInfo)
 private:
  ::google::protobuf::UnknownFieldSet _unknown_fields_;
  mutable int _cached_size_;
  
  ::std::string* path_;
  static const ::std::string _default_path_;
  ::google::protobuf::uint64 length_;
  friend void  protobuf_AddDesc_pblog_2fprotocol_2eproto();
  friend void protobuf_AssignDesc_pblog_2fprotocol_2eproto();
  friend void protobuf_ShutdownFile_pblog_2fprotocol_2eproto();
  
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
  static StreamInfo* default_instance_;
};
// -------------------------------------------------------------------

class StreamSetInfo : public ::google::protobuf::Message {
 public:
  StreamSetInfo();
  virtual ~StreamSetInfo();
  
  StreamSetInfo(const StreamSetInfo& from);
  
  inline StreamSetInfo& operator=(const StreamSetInfo& from) {
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
  static const StreamSetInfo& default_instance();
  
  void Swap(StreamSetInfo* other);
  
  // implements Message ----------------------------------------------
  
  StreamSetInfo* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const StreamSetInfo& from);
  void MergeFrom(const StreamSetInfo& from);
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
  
  // repeated .pblog.protocol.StreamInfo stream = 2;
  inline int stream_size() const;
  inline void clear_stream();
  static const int kStreamFieldNumber = 2;
  inline const ::pblog::protocol::StreamInfo& stream(int index) const;
  inline ::pblog::protocol::StreamInfo* mutable_stream(int index);
  inline ::pblog::protocol::StreamInfo* add_stream();
  inline const ::google::protobuf::RepeatedPtrField< ::pblog::protocol::StreamInfo >&
      stream() const;
  inline ::google::protobuf::RepeatedPtrField< ::pblog::protocol::StreamInfo >*
      mutable_stream();
  
  static const ::google::protobuf::uint32 pblog_namespace = 1;
  static const ::google::protobuf::uint32 pblog_enumeration = 5;
  // @@protoc_insertion_point(class_scope:pblog.protocol.StreamSetInfo)
 private:
  ::google::protobuf::UnknownFieldSet _unknown_fields_;
  mutable int _cached_size_;
  
  ::std::string* path_;
  static const ::std::string _default_path_;
  ::google::protobuf::RepeatedPtrField< ::pblog::protocol::StreamInfo > stream_;
  friend void  protobuf_AddDesc_pblog_2fprotocol_2eproto();
  friend void protobuf_AssignDesc_pblog_2fprotocol_2eproto();
  friend void protobuf_ShutdownFile_pblog_2fprotocol_2eproto();
  
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
  static StreamSetInfo* default_instance_;
};
// -------------------------------------------------------------------

class BucketInfo : public ::google::protobuf::Message {
 public:
  BucketInfo();
  virtual ~BucketInfo();
  
  BucketInfo(const BucketInfo& from);
  
  inline BucketInfo& operator=(const BucketInfo& from) {
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
  static const BucketInfo& default_instance();
  
  void Swap(BucketInfo* other);
  
  // implements Message ----------------------------------------------
  
  BucketInfo* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const BucketInfo& from);
  void MergeFrom(const BucketInfo& from);
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
  
  // repeated .pblog.protocol.StreamSetInfo stream_set = 2;
  inline int stream_set_size() const;
  inline void clear_stream_set();
  static const int kStreamSetFieldNumber = 2;
  inline const ::pblog::protocol::StreamSetInfo& stream_set(int index) const;
  inline ::pblog::protocol::StreamSetInfo* mutable_stream_set(int index);
  inline ::pblog::protocol::StreamSetInfo* add_stream_set();
  inline const ::google::protobuf::RepeatedPtrField< ::pblog::protocol::StreamSetInfo >&
      stream_set() const;
  inline ::google::protobuf::RepeatedPtrField< ::pblog::protocol::StreamSetInfo >*
      mutable_stream_set();
  
  static const ::google::protobuf::uint32 pblog_namespace = 1;
  static const ::google::protobuf::uint32 pblog_enumeration = 3;
  // @@protoc_insertion_point(class_scope:pblog.protocol.BucketInfo)
 private:
  ::google::protobuf::UnknownFieldSet _unknown_fields_;
  mutable int _cached_size_;
  
  ::std::string* path_;
  static const ::std::string _default_path_;
  ::google::protobuf::RepeatedPtrField< ::pblog::protocol::StreamSetInfo > stream_set_;
  friend void  protobuf_AddDesc_pblog_2fprotocol_2eproto();
  friend void protobuf_AssignDesc_pblog_2fprotocol_2eproto();
  friend void protobuf_ShutdownFile_pblog_2fprotocol_2eproto();
  
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
  static BucketInfo* default_instance_;
};
// -------------------------------------------------------------------

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
  
  // repeated .pblog.protocol.BucketInfo bucket = 1;
  inline int bucket_size() const;
  inline void clear_bucket();
  static const int kBucketFieldNumber = 1;
  inline const ::pblog::protocol::BucketInfo& bucket(int index) const;
  inline ::pblog::protocol::BucketInfo* mutable_bucket(int index);
  inline ::pblog::protocol::BucketInfo* add_bucket();
  inline const ::google::protobuf::RepeatedPtrField< ::pblog::protocol::BucketInfo >&
      bucket() const;
  inline ::google::protobuf::RepeatedPtrField< ::pblog::protocol::BucketInfo >*
      mutable_bucket();
  
  static const ::google::protobuf::uint32 pblog_namespace = 1;
  static const ::google::protobuf::uint32 pblog_enumeration = 4;
  // @@protoc_insertion_point(class_scope:pblog.protocol.StoreInfo)
 private:
  ::google::protobuf::UnknownFieldSet _unknown_fields_;
  mutable int _cached_size_;
  
  ::google::protobuf::RepeatedPtrField< ::pblog::protocol::BucketInfo > bucket_;
  friend void  protobuf_AddDesc_pblog_2fprotocol_2eproto();
  friend void protobuf_AssignDesc_pblog_2fprotocol_2eproto();
  friend void protobuf_ShutdownFile_pblog_2fprotocol_2eproto();
  
  ::google::protobuf::uint32 _has_bits_[(1 + 31) / 32];
  
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
// ===================================================================


// ===================================================================

// StreamInfo

// optional string path = 1;
inline bool StreamInfo::has_path() const {
  return _has_bit(0);
}
inline void StreamInfo::clear_path() {
  if (path_ != &_default_path_) {
    path_->clear();
  }
  _clear_bit(0);
}
inline const ::std::string& StreamInfo::path() const {
  return *path_;
}
inline void StreamInfo::set_path(const ::std::string& value) {
  _set_bit(0);
  if (path_ == &_default_path_) {
    path_ = new ::std::string;
  }
  path_->assign(value);
}
inline void StreamInfo::set_path(const char* value) {
  _set_bit(0);
  if (path_ == &_default_path_) {
    path_ = new ::std::string;
  }
  path_->assign(value);
}
inline void StreamInfo::set_path(const char* value, size_t size) {
  _set_bit(0);
  if (path_ == &_default_path_) {
    path_ = new ::std::string;
  }
  path_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* StreamInfo::mutable_path() {
  _set_bit(0);
  if (path_ == &_default_path_) {
    path_ = new ::std::string;
  }
  return path_;
}

// optional uint64 length = 2;
inline bool StreamInfo::has_length() const {
  return _has_bit(1);
}
inline void StreamInfo::clear_length() {
  length_ = GOOGLE_ULONGLONG(0);
  _clear_bit(1);
}
inline ::google::protobuf::uint64 StreamInfo::length() const {
  return length_;
}
inline void StreamInfo::set_length(::google::protobuf::uint64 value) {
  _set_bit(1);
  length_ = value;
}

// -------------------------------------------------------------------

// StreamSetInfo

// optional string path = 1;
inline bool StreamSetInfo::has_path() const {
  return _has_bit(0);
}
inline void StreamSetInfo::clear_path() {
  if (path_ != &_default_path_) {
    path_->clear();
  }
  _clear_bit(0);
}
inline const ::std::string& StreamSetInfo::path() const {
  return *path_;
}
inline void StreamSetInfo::set_path(const ::std::string& value) {
  _set_bit(0);
  if (path_ == &_default_path_) {
    path_ = new ::std::string;
  }
  path_->assign(value);
}
inline void StreamSetInfo::set_path(const char* value) {
  _set_bit(0);
  if (path_ == &_default_path_) {
    path_ = new ::std::string;
  }
  path_->assign(value);
}
inline void StreamSetInfo::set_path(const char* value, size_t size) {
  _set_bit(0);
  if (path_ == &_default_path_) {
    path_ = new ::std::string;
  }
  path_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* StreamSetInfo::mutable_path() {
  _set_bit(0);
  if (path_ == &_default_path_) {
    path_ = new ::std::string;
  }
  return path_;
}

// repeated .pblog.protocol.StreamInfo stream = 2;
inline int StreamSetInfo::stream_size() const {
  return stream_.size();
}
inline void StreamSetInfo::clear_stream() {
  stream_.Clear();
}
inline const ::pblog::protocol::StreamInfo& StreamSetInfo::stream(int index) const {
  return stream_.Get(index);
}
inline ::pblog::protocol::StreamInfo* StreamSetInfo::mutable_stream(int index) {
  return stream_.Mutable(index);
}
inline ::pblog::protocol::StreamInfo* StreamSetInfo::add_stream() {
  return stream_.Add();
}
inline const ::google::protobuf::RepeatedPtrField< ::pblog::protocol::StreamInfo >&
StreamSetInfo::stream() const {
  return stream_;
}
inline ::google::protobuf::RepeatedPtrField< ::pblog::protocol::StreamInfo >*
StreamSetInfo::mutable_stream() {
  return &stream_;
}

// -------------------------------------------------------------------

// BucketInfo

// optional string path = 1;
inline bool BucketInfo::has_path() const {
  return _has_bit(0);
}
inline void BucketInfo::clear_path() {
  if (path_ != &_default_path_) {
    path_->clear();
  }
  _clear_bit(0);
}
inline const ::std::string& BucketInfo::path() const {
  return *path_;
}
inline void BucketInfo::set_path(const ::std::string& value) {
  _set_bit(0);
  if (path_ == &_default_path_) {
    path_ = new ::std::string;
  }
  path_->assign(value);
}
inline void BucketInfo::set_path(const char* value) {
  _set_bit(0);
  if (path_ == &_default_path_) {
    path_ = new ::std::string;
  }
  path_->assign(value);
}
inline void BucketInfo::set_path(const char* value, size_t size) {
  _set_bit(0);
  if (path_ == &_default_path_) {
    path_ = new ::std::string;
  }
  path_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* BucketInfo::mutable_path() {
  _set_bit(0);
  if (path_ == &_default_path_) {
    path_ = new ::std::string;
  }
  return path_;
}

// repeated .pblog.protocol.StreamSetInfo stream_set = 2;
inline int BucketInfo::stream_set_size() const {
  return stream_set_.size();
}
inline void BucketInfo::clear_stream_set() {
  stream_set_.Clear();
}
inline const ::pblog::protocol::StreamSetInfo& BucketInfo::stream_set(int index) const {
  return stream_set_.Get(index);
}
inline ::pblog::protocol::StreamSetInfo* BucketInfo::mutable_stream_set(int index) {
  return stream_set_.Mutable(index);
}
inline ::pblog::protocol::StreamSetInfo* BucketInfo::add_stream_set() {
  return stream_set_.Add();
}
inline const ::google::protobuf::RepeatedPtrField< ::pblog::protocol::StreamSetInfo >&
BucketInfo::stream_set() const {
  return stream_set_;
}
inline ::google::protobuf::RepeatedPtrField< ::pblog::protocol::StreamSetInfo >*
BucketInfo::mutable_stream_set() {
  return &stream_set_;
}

// -------------------------------------------------------------------

// StoreInfo

// repeated .pblog.protocol.BucketInfo bucket = 1;
inline int StoreInfo::bucket_size() const {
  return bucket_.size();
}
inline void StoreInfo::clear_bucket() {
  bucket_.Clear();
}
inline const ::pblog::protocol::BucketInfo& StoreInfo::bucket(int index) const {
  return bucket_.Get(index);
}
inline ::pblog::protocol::BucketInfo* StoreInfo::mutable_bucket(int index) {
  return bucket_.Mutable(index);
}
inline ::pblog::protocol::BucketInfo* StoreInfo::add_bucket() {
  return bucket_.Add();
}
inline const ::google::protobuf::RepeatedPtrField< ::pblog::protocol::BucketInfo >&
StoreInfo::bucket() const {
  return bucket_;
}
inline ::google::protobuf::RepeatedPtrField< ::pblog::protocol::BucketInfo >*
StoreInfo::mutable_bucket() {
  return &bucket_;
}


// @@protoc_insertion_point(namespace_scope)

}  // namespace protocol
}  // namespace pblog

#ifndef SWIG
namespace google {
namespace protobuf {


}  // namespace google
}  // namespace protobuf
#endif  // SWIG

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_pblog_2fprotocol_2eproto__INCLUDED
