// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: zippylog/message.proto

#ifndef PROTOBUF_zippylog_2fmessage_2eproto__INCLUDED
#define PROTOBUF_zippylog_2fmessage_2eproto__INCLUDED

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
// @@protoc_insertion_point(includes)

namespace zippylog {
namespace message {

// Internal implementation detail -- do not call these.
void  protobuf_AddDesc_zippylog_2fmessage_2eproto();
void protobuf_AssignDesc_zippylog_2fmessage_2eproto();
void protobuf_ShutdownFile_zippylog_2fmessage_2eproto();

class Envelope;
class ActorInfo;

// ===================================================================

class Envelope : public ::google::protobuf::Message {
 public:
  Envelope();
  virtual ~Envelope();
  
  Envelope(const Envelope& from);
  
  inline Envelope& operator=(const Envelope& from) {
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
  static const Envelope& default_instance();
  
  void Swap(Envelope* other);
  
  // implements Message ----------------------------------------------
  
  Envelope* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const Envelope& from);
  void MergeFrom(const Envelope& from);
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
  
  // optional uint32 version = 1 [default = 1];
  inline bool has_version() const;
  inline void clear_version();
  static const int kVersionFieldNumber = 1;
  inline ::google::protobuf::uint32 version() const;
  inline void set_version(::google::protobuf::uint32 value);
  
  // repeated bytes message = 2;
  inline int message_size() const;
  inline void clear_message();
  static const int kMessageFieldNumber = 2;
  inline const ::std::string& message(int index) const;
  inline ::std::string* mutable_message(int index);
  inline void set_message(int index, const ::std::string& value);
  inline void set_message(int index, const char* value);
  inline void set_message(int index, const void* value, size_t size);
  inline ::std::string* add_message();
  inline void add_message(const ::std::string& value);
  inline void add_message(const char* value);
  inline void add_message(const void* value, size_t size);
  inline const ::google::protobuf::RepeatedPtrField< ::std::string>& message() const;
  inline ::google::protobuf::RepeatedPtrField< ::std::string>* mutable_message();
  
  // repeated uint32 message_namespace = 3 [packed = true];
  inline int message_namespace_size() const;
  inline void clear_message_namespace();
  static const int kMessageNamespaceFieldNumber = 3;
  inline ::google::protobuf::uint32 message_namespace(int index) const;
  inline void set_message_namespace(int index, ::google::protobuf::uint32 value);
  inline void add_message_namespace(::google::protobuf::uint32 value);
  inline const ::google::protobuf::RepeatedField< ::google::protobuf::uint32 >&
      message_namespace() const;
  inline ::google::protobuf::RepeatedField< ::google::protobuf::uint32 >*
      mutable_message_namespace();
  
  // repeated uint32 message_type = 4 [packed = true];
  inline int message_type_size() const;
  inline void clear_message_type();
  static const int kMessageTypeFieldNumber = 4;
  inline ::google::protobuf::uint32 message_type(int index) const;
  inline void set_message_type(int index, ::google::protobuf::uint32 value);
  inline void add_message_type(::google::protobuf::uint32 value);
  inline const ::google::protobuf::RepeatedField< ::google::protobuf::uint32 >&
      message_type() const;
  inline ::google::protobuf::RepeatedField< ::google::protobuf::uint32 >*
      mutable_message_type();
  
  // optional uint64 create_time = 5;
  inline bool has_create_time() const;
  inline void clear_create_time();
  static const int kCreateTimeFieldNumber = 5;
  inline ::google::protobuf::uint64 create_time() const;
  inline void set_create_time(::google::protobuf::uint64 value);
  
  // repeated bytes tag = 6;
  inline int tag_size() const;
  inline void clear_tag();
  static const int kTagFieldNumber = 6;
  inline const ::std::string& tag(int index) const;
  inline ::std::string* mutable_tag(int index);
  inline void set_tag(int index, const ::std::string& value);
  inline void set_tag(int index, const char* value);
  inline void set_tag(int index, const void* value, size_t size);
  inline ::std::string* add_tag();
  inline void add_tag(const ::std::string& value);
  inline void add_tag(const char* value);
  inline void add_tag(const void* value, size_t size);
  inline const ::google::protobuf::RepeatedPtrField< ::std::string>& tag() const;
  inline ::google::protobuf::RepeatedPtrField< ::std::string>* mutable_tag();
  
  // repeated .zippylog.message.ActorInfo actor = 7;
  inline int actor_size() const;
  inline void clear_actor();
  static const int kActorFieldNumber = 7;
  inline const ::zippylog::message::ActorInfo& actor(int index) const;
  inline ::zippylog::message::ActorInfo* mutable_actor(int index);
  inline ::zippylog::message::ActorInfo* add_actor();
  inline const ::google::protobuf::RepeatedPtrField< ::zippylog::message::ActorInfo >&
      actor() const;
  inline ::google::protobuf::RepeatedPtrField< ::zippylog::message::ActorInfo >*
      mutable_actor();
  
  // optional uint64 numeric_value = 8;
  inline bool has_numeric_value() const;
  inline void clear_numeric_value();
  static const int kNumericValueFieldNumber = 8;
  inline ::google::protobuf::uint64 numeric_value() const;
  inline void set_numeric_value(::google::protobuf::uint64 value);
  
  // optional string string_value = 9;
  inline bool has_string_value() const;
  inline void clear_string_value();
  static const int kStringValueFieldNumber = 9;
  inline const ::std::string& string_value() const;
  inline void set_string_value(const ::std::string& value);
  inline void set_string_value(const char* value);
  inline void set_string_value(const char* value, size_t size);
  inline ::std::string* mutable_string_value();
  inline ::std::string* release_string_value();
  
  static const ::google::protobuf::uint32 zippylog_namespace = 1;
  static const ::google::protobuf::uint32 zippylog_enumeration = 30;
  // @@protoc_insertion_point(class_scope:zippylog.message.Envelope)
 private:
  inline void set_has_version();
  inline void clear_has_version();
  inline void set_has_create_time();
  inline void clear_has_create_time();
  inline void set_has_numeric_value();
  inline void clear_has_numeric_value();
  inline void set_has_string_value();
  inline void clear_has_string_value();
  
  ::google::protobuf::UnknownFieldSet _unknown_fields_;
  
  ::google::protobuf::RepeatedPtrField< ::std::string> message_;
  ::google::protobuf::RepeatedField< ::google::protobuf::uint32 > message_namespace_;
  mutable int _message_namespace_cached_byte_size_;
  ::google::protobuf::RepeatedField< ::google::protobuf::uint32 > message_type_;
  mutable int _message_type_cached_byte_size_;
  ::google::protobuf::uint64 create_time_;
  ::google::protobuf::RepeatedPtrField< ::std::string> tag_;
  ::google::protobuf::RepeatedPtrField< ::zippylog::message::ActorInfo > actor_;
  ::google::protobuf::uint64 numeric_value_;
  ::std::string* string_value_;
  ::google::protobuf::uint32 version_;
  
  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(9 + 31) / 32];
  
  friend void  protobuf_AddDesc_zippylog_2fmessage_2eproto();
  friend void protobuf_AssignDesc_zippylog_2fmessage_2eproto();
  friend void protobuf_ShutdownFile_zippylog_2fmessage_2eproto();
  
  void InitAsDefaultInstance();
  static Envelope* default_instance_;
};
// -------------------------------------------------------------------

class ActorInfo : public ::google::protobuf::Message {
 public:
  ActorInfo();
  virtual ~ActorInfo();
  
  ActorInfo(const ActorInfo& from);
  
  inline ActorInfo& operator=(const ActorInfo& from) {
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
  static const ActorInfo& default_instance();
  
  void Swap(ActorInfo* other);
  
  // implements Message ----------------------------------------------
  
  ActorInfo* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const ActorInfo& from);
  void MergeFrom(const ActorInfo& from);
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
  
  // optional uint64 touch_time = 1;
  inline bool has_touch_time() const;
  inline void clear_touch_time();
  static const int kTouchTimeFieldNumber = 1;
  inline ::google::protobuf::uint64 touch_time() const;
  inline void set_touch_time(::google::protobuf::uint64 value);
  
  // optional uint64 sequence_id = 2;
  inline bool has_sequence_id() const;
  inline void clear_sequence_id();
  static const int kSequenceIdFieldNumber = 2;
  inline ::google::protobuf::uint64 sequence_id() const;
  inline void set_sequence_id(::google::protobuf::uint64 value);
  
  // optional string hostname = 3;
  inline bool has_hostname() const;
  inline void clear_hostname();
  static const int kHostnameFieldNumber = 3;
  inline const ::std::string& hostname() const;
  inline void set_hostname(const ::std::string& value);
  inline void set_hostname(const char* value);
  inline void set_hostname(const char* value, size_t size);
  inline ::std::string* mutable_hostname();
  inline ::std::string* release_hostname();
  
  // optional bytes host_id = 4;
  inline bool has_host_id() const;
  inline void clear_host_id();
  static const int kHostIdFieldNumber = 4;
  inline const ::std::string& host_id() const;
  inline void set_host_id(const ::std::string& value);
  inline void set_host_id(const char* value);
  inline void set_host_id(const void* value, size_t size);
  inline ::std::string* mutable_host_id();
  inline ::std::string* release_host_id();
  
  // optional bytes actor_id = 5;
  inline bool has_actor_id() const;
  inline void clear_actor_id();
  static const int kActorIdFieldNumber = 5;
  inline const ::std::string& actor_id() const;
  inline void set_actor_id(const ::std::string& value);
  inline void set_actor_id(const char* value);
  inline void set_actor_id(const void* value, size_t size);
  inline ::std::string* mutable_actor_id();
  inline ::std::string* release_actor_id();
  
  // optional bytes app_id = 6;
  inline bool has_app_id() const;
  inline void clear_app_id();
  static const int kAppIdFieldNumber = 6;
  inline const ::std::string& app_id() const;
  inline void set_app_id(const ::std::string& value);
  inline void set_app_id(const char* value);
  inline void set_app_id(const void* value, size_t size);
  inline ::std::string* mutable_app_id();
  inline ::std::string* release_app_id();
  
  static const ::google::protobuf::uint32 zippylog_namespace = 1;
  static const ::google::protobuf::uint32 zippylog_enumeration = 31;
  // @@protoc_insertion_point(class_scope:zippylog.message.ActorInfo)
 private:
  inline void set_has_touch_time();
  inline void clear_has_touch_time();
  inline void set_has_sequence_id();
  inline void clear_has_sequence_id();
  inline void set_has_hostname();
  inline void clear_has_hostname();
  inline void set_has_host_id();
  inline void clear_has_host_id();
  inline void set_has_actor_id();
  inline void clear_has_actor_id();
  inline void set_has_app_id();
  inline void clear_has_app_id();
  
  ::google::protobuf::UnknownFieldSet _unknown_fields_;
  
  ::google::protobuf::uint64 touch_time_;
  ::google::protobuf::uint64 sequence_id_;
  ::std::string* hostname_;
  ::std::string* host_id_;
  ::std::string* actor_id_;
  ::std::string* app_id_;
  
  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(6 + 31) / 32];
  
  friend void  protobuf_AddDesc_zippylog_2fmessage_2eproto();
  friend void protobuf_AssignDesc_zippylog_2fmessage_2eproto();
  friend void protobuf_ShutdownFile_zippylog_2fmessage_2eproto();
  
  void InitAsDefaultInstance();
  static ActorInfo* default_instance_;
};
// ===================================================================


// ===================================================================

// Envelope

// optional uint32 version = 1 [default = 1];
inline bool Envelope::has_version() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void Envelope::set_has_version() {
  _has_bits_[0] |= 0x00000001u;
}
inline void Envelope::clear_has_version() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void Envelope::clear_version() {
  version_ = 1u;
  clear_has_version();
}
inline ::google::protobuf::uint32 Envelope::version() const {
  return version_;
}
inline void Envelope::set_version(::google::protobuf::uint32 value) {
  set_has_version();
  version_ = value;
}

// repeated bytes message = 2;
inline int Envelope::message_size() const {
  return message_.size();
}
inline void Envelope::clear_message() {
  message_.Clear();
}
inline const ::std::string& Envelope::message(int index) const {
  return message_.Get(index);
}
inline ::std::string* Envelope::mutable_message(int index) {
  return message_.Mutable(index);
}
inline void Envelope::set_message(int index, const ::std::string& value) {
  message_.Mutable(index)->assign(value);
}
inline void Envelope::set_message(int index, const char* value) {
  message_.Mutable(index)->assign(value);
}
inline void Envelope::set_message(int index, const void* value, size_t size) {
  message_.Mutable(index)->assign(
    reinterpret_cast<const char*>(value), size);
}
inline ::std::string* Envelope::add_message() {
  return message_.Add();
}
inline void Envelope::add_message(const ::std::string& value) {
  message_.Add()->assign(value);
}
inline void Envelope::add_message(const char* value) {
  message_.Add()->assign(value);
}
inline void Envelope::add_message(const void* value, size_t size) {
  message_.Add()->assign(reinterpret_cast<const char*>(value), size);
}
inline const ::google::protobuf::RepeatedPtrField< ::std::string>&
Envelope::message() const {
  return message_;
}
inline ::google::protobuf::RepeatedPtrField< ::std::string>*
Envelope::mutable_message() {
  return &message_;
}

// repeated uint32 message_namespace = 3 [packed = true];
inline int Envelope::message_namespace_size() const {
  return message_namespace_.size();
}
inline void Envelope::clear_message_namespace() {
  message_namespace_.Clear();
}
inline ::google::protobuf::uint32 Envelope::message_namespace(int index) const {
  return message_namespace_.Get(index);
}
inline void Envelope::set_message_namespace(int index, ::google::protobuf::uint32 value) {
  message_namespace_.Set(index, value);
}
inline void Envelope::add_message_namespace(::google::protobuf::uint32 value) {
  message_namespace_.Add(value);
}
inline const ::google::protobuf::RepeatedField< ::google::protobuf::uint32 >&
Envelope::message_namespace() const {
  return message_namespace_;
}
inline ::google::protobuf::RepeatedField< ::google::protobuf::uint32 >*
Envelope::mutable_message_namespace() {
  return &message_namespace_;
}

// repeated uint32 message_type = 4 [packed = true];
inline int Envelope::message_type_size() const {
  return message_type_.size();
}
inline void Envelope::clear_message_type() {
  message_type_.Clear();
}
inline ::google::protobuf::uint32 Envelope::message_type(int index) const {
  return message_type_.Get(index);
}
inline void Envelope::set_message_type(int index, ::google::protobuf::uint32 value) {
  message_type_.Set(index, value);
}
inline void Envelope::add_message_type(::google::protobuf::uint32 value) {
  message_type_.Add(value);
}
inline const ::google::protobuf::RepeatedField< ::google::protobuf::uint32 >&
Envelope::message_type() const {
  return message_type_;
}
inline ::google::protobuf::RepeatedField< ::google::protobuf::uint32 >*
Envelope::mutable_message_type() {
  return &message_type_;
}

// optional uint64 create_time = 5;
inline bool Envelope::has_create_time() const {
  return (_has_bits_[0] & 0x00000010u) != 0;
}
inline void Envelope::set_has_create_time() {
  _has_bits_[0] |= 0x00000010u;
}
inline void Envelope::clear_has_create_time() {
  _has_bits_[0] &= ~0x00000010u;
}
inline void Envelope::clear_create_time() {
  create_time_ = GOOGLE_ULONGLONG(0);
  clear_has_create_time();
}
inline ::google::protobuf::uint64 Envelope::create_time() const {
  return create_time_;
}
inline void Envelope::set_create_time(::google::protobuf::uint64 value) {
  set_has_create_time();
  create_time_ = value;
}

// repeated bytes tag = 6;
inline int Envelope::tag_size() const {
  return tag_.size();
}
inline void Envelope::clear_tag() {
  tag_.Clear();
}
inline const ::std::string& Envelope::tag(int index) const {
  return tag_.Get(index);
}
inline ::std::string* Envelope::mutable_tag(int index) {
  return tag_.Mutable(index);
}
inline void Envelope::set_tag(int index, const ::std::string& value) {
  tag_.Mutable(index)->assign(value);
}
inline void Envelope::set_tag(int index, const char* value) {
  tag_.Mutable(index)->assign(value);
}
inline void Envelope::set_tag(int index, const void* value, size_t size) {
  tag_.Mutable(index)->assign(
    reinterpret_cast<const char*>(value), size);
}
inline ::std::string* Envelope::add_tag() {
  return tag_.Add();
}
inline void Envelope::add_tag(const ::std::string& value) {
  tag_.Add()->assign(value);
}
inline void Envelope::add_tag(const char* value) {
  tag_.Add()->assign(value);
}
inline void Envelope::add_tag(const void* value, size_t size) {
  tag_.Add()->assign(reinterpret_cast<const char*>(value), size);
}
inline const ::google::protobuf::RepeatedPtrField< ::std::string>&
Envelope::tag() const {
  return tag_;
}
inline ::google::protobuf::RepeatedPtrField< ::std::string>*
Envelope::mutable_tag() {
  return &tag_;
}

// repeated .zippylog.message.ActorInfo actor = 7;
inline int Envelope::actor_size() const {
  return actor_.size();
}
inline void Envelope::clear_actor() {
  actor_.Clear();
}
inline const ::zippylog::message::ActorInfo& Envelope::actor(int index) const {
  return actor_.Get(index);
}
inline ::zippylog::message::ActorInfo* Envelope::mutable_actor(int index) {
  return actor_.Mutable(index);
}
inline ::zippylog::message::ActorInfo* Envelope::add_actor() {
  return actor_.Add();
}
inline const ::google::protobuf::RepeatedPtrField< ::zippylog::message::ActorInfo >&
Envelope::actor() const {
  return actor_;
}
inline ::google::protobuf::RepeatedPtrField< ::zippylog::message::ActorInfo >*
Envelope::mutable_actor() {
  return &actor_;
}

// optional uint64 numeric_value = 8;
inline bool Envelope::has_numeric_value() const {
  return (_has_bits_[0] & 0x00000080u) != 0;
}
inline void Envelope::set_has_numeric_value() {
  _has_bits_[0] |= 0x00000080u;
}
inline void Envelope::clear_has_numeric_value() {
  _has_bits_[0] &= ~0x00000080u;
}
inline void Envelope::clear_numeric_value() {
  numeric_value_ = GOOGLE_ULONGLONG(0);
  clear_has_numeric_value();
}
inline ::google::protobuf::uint64 Envelope::numeric_value() const {
  return numeric_value_;
}
inline void Envelope::set_numeric_value(::google::protobuf::uint64 value) {
  set_has_numeric_value();
  numeric_value_ = value;
}

// optional string string_value = 9;
inline bool Envelope::has_string_value() const {
  return (_has_bits_[0] & 0x00000100u) != 0;
}
inline void Envelope::set_has_string_value() {
  _has_bits_[0] |= 0x00000100u;
}
inline void Envelope::clear_has_string_value() {
  _has_bits_[0] &= ~0x00000100u;
}
inline void Envelope::clear_string_value() {
  if (string_value_ != &::google::protobuf::internal::kEmptyString) {
    string_value_->clear();
  }
  clear_has_string_value();
}
inline const ::std::string& Envelope::string_value() const {
  return *string_value_;
}
inline void Envelope::set_string_value(const ::std::string& value) {
  set_has_string_value();
  if (string_value_ == &::google::protobuf::internal::kEmptyString) {
    string_value_ = new ::std::string;
  }
  string_value_->assign(value);
}
inline void Envelope::set_string_value(const char* value) {
  set_has_string_value();
  if (string_value_ == &::google::protobuf::internal::kEmptyString) {
    string_value_ = new ::std::string;
  }
  string_value_->assign(value);
}
inline void Envelope::set_string_value(const char* value, size_t size) {
  set_has_string_value();
  if (string_value_ == &::google::protobuf::internal::kEmptyString) {
    string_value_ = new ::std::string;
  }
  string_value_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* Envelope::mutable_string_value() {
  set_has_string_value();
  if (string_value_ == &::google::protobuf::internal::kEmptyString) {
    string_value_ = new ::std::string;
  }
  return string_value_;
}
inline ::std::string* Envelope::release_string_value() {
  clear_has_string_value();
  if (string_value_ == &::google::protobuf::internal::kEmptyString) {
    return NULL;
  } else {
    ::std::string* temp = string_value_;
    string_value_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
    return temp;
  }
}

// -------------------------------------------------------------------

// ActorInfo

// optional uint64 touch_time = 1;
inline bool ActorInfo::has_touch_time() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void ActorInfo::set_has_touch_time() {
  _has_bits_[0] |= 0x00000001u;
}
inline void ActorInfo::clear_has_touch_time() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void ActorInfo::clear_touch_time() {
  touch_time_ = GOOGLE_ULONGLONG(0);
  clear_has_touch_time();
}
inline ::google::protobuf::uint64 ActorInfo::touch_time() const {
  return touch_time_;
}
inline void ActorInfo::set_touch_time(::google::protobuf::uint64 value) {
  set_has_touch_time();
  touch_time_ = value;
}

// optional uint64 sequence_id = 2;
inline bool ActorInfo::has_sequence_id() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void ActorInfo::set_has_sequence_id() {
  _has_bits_[0] |= 0x00000002u;
}
inline void ActorInfo::clear_has_sequence_id() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void ActorInfo::clear_sequence_id() {
  sequence_id_ = GOOGLE_ULONGLONG(0);
  clear_has_sequence_id();
}
inline ::google::protobuf::uint64 ActorInfo::sequence_id() const {
  return sequence_id_;
}
inline void ActorInfo::set_sequence_id(::google::protobuf::uint64 value) {
  set_has_sequence_id();
  sequence_id_ = value;
}

// optional string hostname = 3;
inline bool ActorInfo::has_hostname() const {
  return (_has_bits_[0] & 0x00000004u) != 0;
}
inline void ActorInfo::set_has_hostname() {
  _has_bits_[0] |= 0x00000004u;
}
inline void ActorInfo::clear_has_hostname() {
  _has_bits_[0] &= ~0x00000004u;
}
inline void ActorInfo::clear_hostname() {
  if (hostname_ != &::google::protobuf::internal::kEmptyString) {
    hostname_->clear();
  }
  clear_has_hostname();
}
inline const ::std::string& ActorInfo::hostname() const {
  return *hostname_;
}
inline void ActorInfo::set_hostname(const ::std::string& value) {
  set_has_hostname();
  if (hostname_ == &::google::protobuf::internal::kEmptyString) {
    hostname_ = new ::std::string;
  }
  hostname_->assign(value);
}
inline void ActorInfo::set_hostname(const char* value) {
  set_has_hostname();
  if (hostname_ == &::google::protobuf::internal::kEmptyString) {
    hostname_ = new ::std::string;
  }
  hostname_->assign(value);
}
inline void ActorInfo::set_hostname(const char* value, size_t size) {
  set_has_hostname();
  if (hostname_ == &::google::protobuf::internal::kEmptyString) {
    hostname_ = new ::std::string;
  }
  hostname_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* ActorInfo::mutable_hostname() {
  set_has_hostname();
  if (hostname_ == &::google::protobuf::internal::kEmptyString) {
    hostname_ = new ::std::string;
  }
  return hostname_;
}
inline ::std::string* ActorInfo::release_hostname() {
  clear_has_hostname();
  if (hostname_ == &::google::protobuf::internal::kEmptyString) {
    return NULL;
  } else {
    ::std::string* temp = hostname_;
    hostname_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
    return temp;
  }
}

// optional bytes host_id = 4;
inline bool ActorInfo::has_host_id() const {
  return (_has_bits_[0] & 0x00000008u) != 0;
}
inline void ActorInfo::set_has_host_id() {
  _has_bits_[0] |= 0x00000008u;
}
inline void ActorInfo::clear_has_host_id() {
  _has_bits_[0] &= ~0x00000008u;
}
inline void ActorInfo::clear_host_id() {
  if (host_id_ != &::google::protobuf::internal::kEmptyString) {
    host_id_->clear();
  }
  clear_has_host_id();
}
inline const ::std::string& ActorInfo::host_id() const {
  return *host_id_;
}
inline void ActorInfo::set_host_id(const ::std::string& value) {
  set_has_host_id();
  if (host_id_ == &::google::protobuf::internal::kEmptyString) {
    host_id_ = new ::std::string;
  }
  host_id_->assign(value);
}
inline void ActorInfo::set_host_id(const char* value) {
  set_has_host_id();
  if (host_id_ == &::google::protobuf::internal::kEmptyString) {
    host_id_ = new ::std::string;
  }
  host_id_->assign(value);
}
inline void ActorInfo::set_host_id(const void* value, size_t size) {
  set_has_host_id();
  if (host_id_ == &::google::protobuf::internal::kEmptyString) {
    host_id_ = new ::std::string;
  }
  host_id_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* ActorInfo::mutable_host_id() {
  set_has_host_id();
  if (host_id_ == &::google::protobuf::internal::kEmptyString) {
    host_id_ = new ::std::string;
  }
  return host_id_;
}
inline ::std::string* ActorInfo::release_host_id() {
  clear_has_host_id();
  if (host_id_ == &::google::protobuf::internal::kEmptyString) {
    return NULL;
  } else {
    ::std::string* temp = host_id_;
    host_id_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
    return temp;
  }
}

// optional bytes actor_id = 5;
inline bool ActorInfo::has_actor_id() const {
  return (_has_bits_[0] & 0x00000010u) != 0;
}
inline void ActorInfo::set_has_actor_id() {
  _has_bits_[0] |= 0x00000010u;
}
inline void ActorInfo::clear_has_actor_id() {
  _has_bits_[0] &= ~0x00000010u;
}
inline void ActorInfo::clear_actor_id() {
  if (actor_id_ != &::google::protobuf::internal::kEmptyString) {
    actor_id_->clear();
  }
  clear_has_actor_id();
}
inline const ::std::string& ActorInfo::actor_id() const {
  return *actor_id_;
}
inline void ActorInfo::set_actor_id(const ::std::string& value) {
  set_has_actor_id();
  if (actor_id_ == &::google::protobuf::internal::kEmptyString) {
    actor_id_ = new ::std::string;
  }
  actor_id_->assign(value);
}
inline void ActorInfo::set_actor_id(const char* value) {
  set_has_actor_id();
  if (actor_id_ == &::google::protobuf::internal::kEmptyString) {
    actor_id_ = new ::std::string;
  }
  actor_id_->assign(value);
}
inline void ActorInfo::set_actor_id(const void* value, size_t size) {
  set_has_actor_id();
  if (actor_id_ == &::google::protobuf::internal::kEmptyString) {
    actor_id_ = new ::std::string;
  }
  actor_id_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* ActorInfo::mutable_actor_id() {
  set_has_actor_id();
  if (actor_id_ == &::google::protobuf::internal::kEmptyString) {
    actor_id_ = new ::std::string;
  }
  return actor_id_;
}
inline ::std::string* ActorInfo::release_actor_id() {
  clear_has_actor_id();
  if (actor_id_ == &::google::protobuf::internal::kEmptyString) {
    return NULL;
  } else {
    ::std::string* temp = actor_id_;
    actor_id_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
    return temp;
  }
}

// optional bytes app_id = 6;
inline bool ActorInfo::has_app_id() const {
  return (_has_bits_[0] & 0x00000020u) != 0;
}
inline void ActorInfo::set_has_app_id() {
  _has_bits_[0] |= 0x00000020u;
}
inline void ActorInfo::clear_has_app_id() {
  _has_bits_[0] &= ~0x00000020u;
}
inline void ActorInfo::clear_app_id() {
  if (app_id_ != &::google::protobuf::internal::kEmptyString) {
    app_id_->clear();
  }
  clear_has_app_id();
}
inline const ::std::string& ActorInfo::app_id() const {
  return *app_id_;
}
inline void ActorInfo::set_app_id(const ::std::string& value) {
  set_has_app_id();
  if (app_id_ == &::google::protobuf::internal::kEmptyString) {
    app_id_ = new ::std::string;
  }
  app_id_->assign(value);
}
inline void ActorInfo::set_app_id(const char* value) {
  set_has_app_id();
  if (app_id_ == &::google::protobuf::internal::kEmptyString) {
    app_id_ = new ::std::string;
  }
  app_id_->assign(value);
}
inline void ActorInfo::set_app_id(const void* value, size_t size) {
  set_has_app_id();
  if (app_id_ == &::google::protobuf::internal::kEmptyString) {
    app_id_ = new ::std::string;
  }
  app_id_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* ActorInfo::mutable_app_id() {
  set_has_app_id();
  if (app_id_ == &::google::protobuf::internal::kEmptyString) {
    app_id_ = new ::std::string;
  }
  return app_id_;
}
inline ::std::string* ActorInfo::release_app_id() {
  clear_has_app_id();
  if (app_id_ == &::google::protobuf::internal::kEmptyString) {
    return NULL;
  } else {
    ::std::string* temp = app_id_;
    app_id_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
    return temp;
  }
}


// @@protoc_insertion_point(namespace_scope)

}  // namespace message
}  // namespace zippylog

#ifndef SWIG
namespace google {
namespace protobuf {


}  // namespace google
}  // namespace protobuf
#endif  // SWIG

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_zippylog_2fmessage_2eproto__INCLUDED
