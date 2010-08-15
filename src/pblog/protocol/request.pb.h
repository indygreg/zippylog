// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: pblog/protocol/request.proto

#ifndef PROTOBUF_pblog_2fprotocol_2frequest_2eproto__INCLUDED
#define PROTOBUF_pblog_2fprotocol_2frequest_2eproto__INCLUDED

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
// @@protoc_insertion_point(includes)

namespace pblog {
namespace protocol {
namespace request {

// Internal implementation detail -- do not call these.
void  protobuf_AddDesc_pblog_2fprotocol_2frequest_2eproto();
void protobuf_AssignDesc_pblog_2fprotocol_2frequest_2eproto();
void protobuf_ShutdownFile_pblog_2fprotocol_2frequest_2eproto();

class StoreInfo;
class Stream;
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
  
  // @@protoc_insertion_point(class_scope:pblog.protocol.request.StoreInfo)
 private:
  ::google::protobuf::UnknownFieldSet _unknown_fields_;
  mutable int _cached_size_;
  
  friend void  protobuf_AddDesc_pblog_2fprotocol_2frequest_2eproto();
  friend void protobuf_AssignDesc_pblog_2fprotocol_2frequest_2eproto();
  friend void protobuf_ShutdownFile_pblog_2fprotocol_2frequest_2eproto();
  
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

class Stream : public ::google::protobuf::Message {
 public:
  Stream();
  virtual ~Stream();
  
  Stream(const Stream& from);
  
  inline Stream& operator=(const Stream& from) {
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
  static const Stream& default_instance();
  
  void Swap(Stream* other);
  
  // implements Message ----------------------------------------------
  
  Stream* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const Stream& from);
  void MergeFrom(const Stream& from);
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
  
  // repeated string paths = 1;
  inline int paths_size() const;
  inline void clear_paths();
  static const int kPathsFieldNumber = 1;
  inline const ::std::string& paths(int index) const;
  inline ::std::string* mutable_paths(int index);
  inline void set_paths(int index, const ::std::string& value);
  inline void set_paths(int index, const char* value);
  inline void set_paths(int index, const char* value, size_t size);
  inline ::std::string* add_paths();
  inline void add_paths(const ::std::string& value);
  inline void add_paths(const char* value);
  inline void add_paths(const char* value, size_t size);
  inline const ::google::protobuf::RepeatedPtrField< ::std::string>& paths() const;
  inline ::google::protobuf::RepeatedPtrField< ::std::string>* mutable_paths();
  
  // @@protoc_insertion_point(class_scope:pblog.protocol.request.Stream)
 private:
  ::google::protobuf::UnknownFieldSet _unknown_fields_;
  mutable int _cached_size_;
  
  ::google::protobuf::RepeatedPtrField< ::std::string> paths_;
  friend void  protobuf_AddDesc_pblog_2fprotocol_2frequest_2eproto();
  friend void protobuf_AssignDesc_pblog_2fprotocol_2frequest_2eproto();
  friend void protobuf_ShutdownFile_pblog_2fprotocol_2frequest_2eproto();
  
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
  static Stream* default_instance_;
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
  
  // repeated string paths = 1;
  inline int paths_size() const;
  inline void clear_paths();
  static const int kPathsFieldNumber = 1;
  inline const ::std::string& paths(int index) const;
  inline ::std::string* mutable_paths(int index);
  inline void set_paths(int index, const ::std::string& value);
  inline void set_paths(int index, const char* value);
  inline void set_paths(int index, const char* value, size_t size);
  inline ::std::string* add_paths();
  inline void add_paths(const ::std::string& value);
  inline void add_paths(const char* value);
  inline void add_paths(const char* value, size_t size);
  inline const ::google::protobuf::RepeatedPtrField< ::std::string>& paths() const;
  inline ::google::protobuf::RepeatedPtrField< ::std::string>* mutable_paths();
  
  // repeated uint64 stream_offsets = 2;
  inline int stream_offsets_size() const;
  inline void clear_stream_offsets();
  static const int kStreamOffsetsFieldNumber = 2;
  inline ::google::protobuf::uint64 stream_offsets(int index) const;
  inline void set_stream_offsets(int index, ::google::protobuf::uint64 value);
  inline void add_stream_offsets(::google::protobuf::uint64 value);
  inline const ::google::protobuf::RepeatedField< ::google::protobuf::uint64 >&
      stream_offsets() const;
  inline ::google::protobuf::RepeatedField< ::google::protobuf::uint64 >*
      mutable_stream_offsets();
  
  // @@protoc_insertion_point(class_scope:pblog.protocol.request.Get)
 private:
  ::google::protobuf::UnknownFieldSet _unknown_fields_;
  mutable int _cached_size_;
  
  ::google::protobuf::RepeatedPtrField< ::std::string> paths_;
  ::google::protobuf::RepeatedField< ::google::protobuf::uint64 > stream_offsets_;
  friend void  protobuf_AddDesc_pblog_2fprotocol_2frequest_2eproto();
  friend void protobuf_AssignDesc_pblog_2fprotocol_2frequest_2eproto();
  friend void protobuf_ShutdownFile_pblog_2fprotocol_2frequest_2eproto();
  
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
  static Get* default_instance_;
};
// ===================================================================


// ===================================================================

// StoreInfo

// -------------------------------------------------------------------

// Stream

// repeated string paths = 1;
inline int Stream::paths_size() const {
  return paths_.size();
}
inline void Stream::clear_paths() {
  paths_.Clear();
}
inline const ::std::string& Stream::paths(int index) const {
  return paths_.Get(index);
}
inline ::std::string* Stream::mutable_paths(int index) {
  return paths_.Mutable(index);
}
inline void Stream::set_paths(int index, const ::std::string& value) {
  paths_.Mutable(index)->assign(value);
}
inline void Stream::set_paths(int index, const char* value) {
  paths_.Mutable(index)->assign(value);
}
inline void Stream::set_paths(int index, const char* value, size_t size) {
  paths_.Mutable(index)->assign(
    reinterpret_cast<const char*>(value), size);
}
inline ::std::string* Stream::add_paths() {
  return paths_.Add();
}
inline void Stream::add_paths(const ::std::string& value) {
  paths_.Add()->assign(value);
}
inline void Stream::add_paths(const char* value) {
  paths_.Add()->assign(value);
}
inline void Stream::add_paths(const char* value, size_t size) {
  paths_.Add()->assign(reinterpret_cast<const char*>(value), size);
}
inline const ::google::protobuf::RepeatedPtrField< ::std::string>&
Stream::paths() const {
  return paths_;
}
inline ::google::protobuf::RepeatedPtrField< ::std::string>*
Stream::mutable_paths() {
  return &paths_;
}

// -------------------------------------------------------------------

// Get

// repeated string paths = 1;
inline int Get::paths_size() const {
  return paths_.size();
}
inline void Get::clear_paths() {
  paths_.Clear();
}
inline const ::std::string& Get::paths(int index) const {
  return paths_.Get(index);
}
inline ::std::string* Get::mutable_paths(int index) {
  return paths_.Mutable(index);
}
inline void Get::set_paths(int index, const ::std::string& value) {
  paths_.Mutable(index)->assign(value);
}
inline void Get::set_paths(int index, const char* value) {
  paths_.Mutable(index)->assign(value);
}
inline void Get::set_paths(int index, const char* value, size_t size) {
  paths_.Mutable(index)->assign(
    reinterpret_cast<const char*>(value), size);
}
inline ::std::string* Get::add_paths() {
  return paths_.Add();
}
inline void Get::add_paths(const ::std::string& value) {
  paths_.Add()->assign(value);
}
inline void Get::add_paths(const char* value) {
  paths_.Add()->assign(value);
}
inline void Get::add_paths(const char* value, size_t size) {
  paths_.Add()->assign(reinterpret_cast<const char*>(value), size);
}
inline const ::google::protobuf::RepeatedPtrField< ::std::string>&
Get::paths() const {
  return paths_;
}
inline ::google::protobuf::RepeatedPtrField< ::std::string>*
Get::mutable_paths() {
  return &paths_;
}

// repeated uint64 stream_offsets = 2;
inline int Get::stream_offsets_size() const {
  return stream_offsets_.size();
}
inline void Get::clear_stream_offsets() {
  stream_offsets_.Clear();
}
inline ::google::protobuf::uint64 Get::stream_offsets(int index) const {
  return stream_offsets_.Get(index);
}
inline void Get::set_stream_offsets(int index, ::google::protobuf::uint64 value) {
  stream_offsets_.Set(index, value);
}
inline void Get::add_stream_offsets(::google::protobuf::uint64 value) {
  stream_offsets_.Add(value);
}
inline const ::google::protobuf::RepeatedField< ::google::protobuf::uint64 >&
Get::stream_offsets() const {
  return stream_offsets_;
}
inline ::google::protobuf::RepeatedField< ::google::protobuf::uint64 >*
Get::mutable_stream_offsets() {
  return &stream_offsets_;
}


// @@protoc_insertion_point(namespace_scope)

}  // namespace request
}  // namespace protocol
}  // namespace pblog

#ifndef SWIG
namespace google {
namespace protobuf {


}  // namespace google
}  // namespace protobuf
#endif  // SWIG

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_pblog_2fprotocol_2frequest_2eproto__INCLUDED
