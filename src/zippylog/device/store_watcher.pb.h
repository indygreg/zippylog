// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: zippylog/device/store_watcher.proto

#ifndef PROTOBUF_zippylog_2fdevice_2fstore_5fwatcher_2eproto__INCLUDED
#define PROTOBUF_zippylog_2fdevice_2fstore_5fwatcher_2eproto__INCLUDED

#include <string>

#include <google/protobuf/stubs/common.h>

#if GOOGLE_PROTOBUF_VERSION < 2004000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please update
#error your headers.
#endif
#if 2004001 < GOOGLE_PROTOBUF_MIN_PROTOC_VERSION
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
namespace device {
namespace store_watcher {

// Internal implementation detail -- do not call these.
void  protobuf_AddDesc_zippylog_2fdevice_2fstore_5fwatcher_2eproto();
void protobuf_AssignDesc_zippylog_2fdevice_2fstore_5fwatcher_2eproto();
void protobuf_ShutdownFile_zippylog_2fdevice_2fstore_5fwatcher_2eproto();

class Create;
class Destroy;
class RunStart;
class RunFinish;

// ===================================================================

class Create : public ::google::protobuf::Message {
 public:
  Create();
  virtual ~Create();
  
  Create(const Create& from);
  
  inline Create& operator=(const Create& from) {
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
  static const Create& default_instance();
  
  void Swap(Create* other);
  
  // implements Message ----------------------------------------------
  
  Create* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const Create& from);
  void MergeFrom(const Create& from);
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
  
  // required bytes id = 1;
  inline bool has_id() const;
  inline void clear_id();
  static const int kIdFieldNumber = 1;
  inline const ::std::string& id() const;
  inline void set_id(const ::std::string& value);
  inline void set_id(const char* value);
  inline void set_id(const void* value, size_t size);
  inline ::std::string* mutable_id();
  inline ::std::string* release_id();
  
  static const ::google::protobuf::uint32 zippylog_namespace = 1;
  static const ::google::protobuf::uint32 zippylog_enumeration = 127;
  
  bool add_to_envelope(::zippylog::Envelope *envelope);
  bool add_to_envelope(::zippylog::Envelope &envelope);
  // @@protoc_insertion_point(class_scope:zippylog.device.store_watcher.Create)
 private:
  inline void set_has_id();
  inline void clear_has_id();
  
  ::google::protobuf::UnknownFieldSet _unknown_fields_;
  
  ::std::string* id_;
  
  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(1 + 31) / 32];
  
  friend void  protobuf_AddDesc_zippylog_2fdevice_2fstore_5fwatcher_2eproto();
  friend void protobuf_AssignDesc_zippylog_2fdevice_2fstore_5fwatcher_2eproto();
  friend void protobuf_ShutdownFile_zippylog_2fdevice_2fstore_5fwatcher_2eproto();
  
  void InitAsDefaultInstance();
  static Create* default_instance_;
};
// -------------------------------------------------------------------

class Destroy : public ::google::protobuf::Message {
 public:
  Destroy();
  virtual ~Destroy();
  
  Destroy(const Destroy& from);
  
  inline Destroy& operator=(const Destroy& from) {
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
  static const Destroy& default_instance();
  
  void Swap(Destroy* other);
  
  // implements Message ----------------------------------------------
  
  Destroy* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const Destroy& from);
  void MergeFrom(const Destroy& from);
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
  
  // required bytes id = 1;
  inline bool has_id() const;
  inline void clear_id();
  static const int kIdFieldNumber = 1;
  inline const ::std::string& id() const;
  inline void set_id(const ::std::string& value);
  inline void set_id(const char* value);
  inline void set_id(const void* value, size_t size);
  inline ::std::string* mutable_id();
  inline ::std::string* release_id();
  
  static const ::google::protobuf::uint32 zippylog_namespace = 1;
  static const ::google::protobuf::uint32 zippylog_enumeration = 128;
  
  bool add_to_envelope(::zippylog::Envelope *envelope);
  bool add_to_envelope(::zippylog::Envelope &envelope);
  // @@protoc_insertion_point(class_scope:zippylog.device.store_watcher.Destroy)
 private:
  inline void set_has_id();
  inline void clear_has_id();
  
  ::google::protobuf::UnknownFieldSet _unknown_fields_;
  
  ::std::string* id_;
  
  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(1 + 31) / 32];
  
  friend void  protobuf_AddDesc_zippylog_2fdevice_2fstore_5fwatcher_2eproto();
  friend void protobuf_AssignDesc_zippylog_2fdevice_2fstore_5fwatcher_2eproto();
  friend void protobuf_ShutdownFile_zippylog_2fdevice_2fstore_5fwatcher_2eproto();
  
  void InitAsDefaultInstance();
  static Destroy* default_instance_;
};
// -------------------------------------------------------------------

class RunStart : public ::google::protobuf::Message {
 public:
  RunStart();
  virtual ~RunStart();
  
  RunStart(const RunStart& from);
  
  inline RunStart& operator=(const RunStart& from) {
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
  static const RunStart& default_instance();
  
  void Swap(RunStart* other);
  
  // implements Message ----------------------------------------------
  
  RunStart* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const RunStart& from);
  void MergeFrom(const RunStart& from);
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
  
  // required bytes id = 1;
  inline bool has_id() const;
  inline void clear_id();
  static const int kIdFieldNumber = 1;
  inline const ::std::string& id() const;
  inline void set_id(const ::std::string& value);
  inline void set_id(const char* value);
  inline void set_id(const void* value, size_t size);
  inline ::std::string* mutable_id();
  inline ::std::string* release_id();
  
  static const ::google::protobuf::uint32 zippylog_namespace = 1;
  static const ::google::protobuf::uint32 zippylog_enumeration = 129;
  
  bool add_to_envelope(::zippylog::Envelope *envelope);
  bool add_to_envelope(::zippylog::Envelope &envelope);
  // @@protoc_insertion_point(class_scope:zippylog.device.store_watcher.RunStart)
 private:
  inline void set_has_id();
  inline void clear_has_id();
  
  ::google::protobuf::UnknownFieldSet _unknown_fields_;
  
  ::std::string* id_;
  
  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(1 + 31) / 32];
  
  friend void  protobuf_AddDesc_zippylog_2fdevice_2fstore_5fwatcher_2eproto();
  friend void protobuf_AssignDesc_zippylog_2fdevice_2fstore_5fwatcher_2eproto();
  friend void protobuf_ShutdownFile_zippylog_2fdevice_2fstore_5fwatcher_2eproto();
  
  void InitAsDefaultInstance();
  static RunStart* default_instance_;
};
// -------------------------------------------------------------------

class RunFinish : public ::google::protobuf::Message {
 public:
  RunFinish();
  virtual ~RunFinish();
  
  RunFinish(const RunFinish& from);
  
  inline RunFinish& operator=(const RunFinish& from) {
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
  static const RunFinish& default_instance();
  
  void Swap(RunFinish* other);
  
  // implements Message ----------------------------------------------
  
  RunFinish* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const RunFinish& from);
  void MergeFrom(const RunFinish& from);
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
  
  // required bytes id = 1;
  inline bool has_id() const;
  inline void clear_id();
  static const int kIdFieldNumber = 1;
  inline const ::std::string& id() const;
  inline void set_id(const ::std::string& value);
  inline void set_id(const char* value);
  inline void set_id(const void* value, size_t size);
  inline ::std::string* mutable_id();
  inline ::std::string* release_id();
  
  static const ::google::protobuf::uint32 zippylog_namespace = 1;
  static const ::google::protobuf::uint32 zippylog_enumeration = 130;
  
  bool add_to_envelope(::zippylog::Envelope *envelope);
  bool add_to_envelope(::zippylog::Envelope &envelope);
  // @@protoc_insertion_point(class_scope:zippylog.device.store_watcher.RunFinish)
 private:
  inline void set_has_id();
  inline void clear_has_id();
  
  ::google::protobuf::UnknownFieldSet _unknown_fields_;
  
  ::std::string* id_;
  
  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(1 + 31) / 32];
  
  friend void  protobuf_AddDesc_zippylog_2fdevice_2fstore_5fwatcher_2eproto();
  friend void protobuf_AssignDesc_zippylog_2fdevice_2fstore_5fwatcher_2eproto();
  friend void protobuf_ShutdownFile_zippylog_2fdevice_2fstore_5fwatcher_2eproto();
  
  void InitAsDefaultInstance();
  static RunFinish* default_instance_;
};
// ===================================================================


// ===================================================================

// Create

// required bytes id = 1;
inline bool Create::has_id() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void Create::set_has_id() {
  _has_bits_[0] |= 0x00000001u;
}
inline void Create::clear_has_id() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void Create::clear_id() {
  if (id_ != &::google::protobuf::internal::kEmptyString) {
    id_->clear();
  }
  clear_has_id();
}
inline const ::std::string& Create::id() const {
  return *id_;
}
inline void Create::set_id(const ::std::string& value) {
  set_has_id();
  if (id_ == &::google::protobuf::internal::kEmptyString) {
    id_ = new ::std::string;
  }
  id_->assign(value);
}
inline void Create::set_id(const char* value) {
  set_has_id();
  if (id_ == &::google::protobuf::internal::kEmptyString) {
    id_ = new ::std::string;
  }
  id_->assign(value);
}
inline void Create::set_id(const void* value, size_t size) {
  set_has_id();
  if (id_ == &::google::protobuf::internal::kEmptyString) {
    id_ = new ::std::string;
  }
  id_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* Create::mutable_id() {
  set_has_id();
  if (id_ == &::google::protobuf::internal::kEmptyString) {
    id_ = new ::std::string;
  }
  return id_;
}
inline ::std::string* Create::release_id() {
  clear_has_id();
  if (id_ == &::google::protobuf::internal::kEmptyString) {
    return NULL;
  } else {
    ::std::string* temp = id_;
    id_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
    return temp;
  }
}

// -------------------------------------------------------------------

// Destroy

// required bytes id = 1;
inline bool Destroy::has_id() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void Destroy::set_has_id() {
  _has_bits_[0] |= 0x00000001u;
}
inline void Destroy::clear_has_id() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void Destroy::clear_id() {
  if (id_ != &::google::protobuf::internal::kEmptyString) {
    id_->clear();
  }
  clear_has_id();
}
inline const ::std::string& Destroy::id() const {
  return *id_;
}
inline void Destroy::set_id(const ::std::string& value) {
  set_has_id();
  if (id_ == &::google::protobuf::internal::kEmptyString) {
    id_ = new ::std::string;
  }
  id_->assign(value);
}
inline void Destroy::set_id(const char* value) {
  set_has_id();
  if (id_ == &::google::protobuf::internal::kEmptyString) {
    id_ = new ::std::string;
  }
  id_->assign(value);
}
inline void Destroy::set_id(const void* value, size_t size) {
  set_has_id();
  if (id_ == &::google::protobuf::internal::kEmptyString) {
    id_ = new ::std::string;
  }
  id_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* Destroy::mutable_id() {
  set_has_id();
  if (id_ == &::google::protobuf::internal::kEmptyString) {
    id_ = new ::std::string;
  }
  return id_;
}
inline ::std::string* Destroy::release_id() {
  clear_has_id();
  if (id_ == &::google::protobuf::internal::kEmptyString) {
    return NULL;
  } else {
    ::std::string* temp = id_;
    id_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
    return temp;
  }
}

// -------------------------------------------------------------------

// RunStart

// required bytes id = 1;
inline bool RunStart::has_id() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void RunStart::set_has_id() {
  _has_bits_[0] |= 0x00000001u;
}
inline void RunStart::clear_has_id() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void RunStart::clear_id() {
  if (id_ != &::google::protobuf::internal::kEmptyString) {
    id_->clear();
  }
  clear_has_id();
}
inline const ::std::string& RunStart::id() const {
  return *id_;
}
inline void RunStart::set_id(const ::std::string& value) {
  set_has_id();
  if (id_ == &::google::protobuf::internal::kEmptyString) {
    id_ = new ::std::string;
  }
  id_->assign(value);
}
inline void RunStart::set_id(const char* value) {
  set_has_id();
  if (id_ == &::google::protobuf::internal::kEmptyString) {
    id_ = new ::std::string;
  }
  id_->assign(value);
}
inline void RunStart::set_id(const void* value, size_t size) {
  set_has_id();
  if (id_ == &::google::protobuf::internal::kEmptyString) {
    id_ = new ::std::string;
  }
  id_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* RunStart::mutable_id() {
  set_has_id();
  if (id_ == &::google::protobuf::internal::kEmptyString) {
    id_ = new ::std::string;
  }
  return id_;
}
inline ::std::string* RunStart::release_id() {
  clear_has_id();
  if (id_ == &::google::protobuf::internal::kEmptyString) {
    return NULL;
  } else {
    ::std::string* temp = id_;
    id_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
    return temp;
  }
}

// -------------------------------------------------------------------

// RunFinish

// required bytes id = 1;
inline bool RunFinish::has_id() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void RunFinish::set_has_id() {
  _has_bits_[0] |= 0x00000001u;
}
inline void RunFinish::clear_has_id() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void RunFinish::clear_id() {
  if (id_ != &::google::protobuf::internal::kEmptyString) {
    id_->clear();
  }
  clear_has_id();
}
inline const ::std::string& RunFinish::id() const {
  return *id_;
}
inline void RunFinish::set_id(const ::std::string& value) {
  set_has_id();
  if (id_ == &::google::protobuf::internal::kEmptyString) {
    id_ = new ::std::string;
  }
  id_->assign(value);
}
inline void RunFinish::set_id(const char* value) {
  set_has_id();
  if (id_ == &::google::protobuf::internal::kEmptyString) {
    id_ = new ::std::string;
  }
  id_->assign(value);
}
inline void RunFinish::set_id(const void* value, size_t size) {
  set_has_id();
  if (id_ == &::google::protobuf::internal::kEmptyString) {
    id_ = new ::std::string;
  }
  id_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* RunFinish::mutable_id() {
  set_has_id();
  if (id_ == &::google::protobuf::internal::kEmptyString) {
    id_ = new ::std::string;
  }
  return id_;
}
inline ::std::string* RunFinish::release_id() {
  clear_has_id();
  if (id_ == &::google::protobuf::internal::kEmptyString) {
    return NULL;
  } else {
    ::std::string* temp = id_;
    id_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
    return temp;
  }
}


// @@protoc_insertion_point(namespace_scope)

}  // namespace store_watcher
}  // namespace device
}  // namespace zippylog

#ifndef SWIG
namespace google {
namespace protobuf {


}  // namespace google
}  // namespace protobuf
#endif  // SWIG

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_zippylog_2fdevice_2fstore_5fwatcher_2eproto__INCLUDED
