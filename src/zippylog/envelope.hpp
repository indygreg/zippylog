//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.

#ifndef ZIPPYLOG_ENVELOPE_HPP_
#define ZIPPYLOG_ENVELOPE_HPP_

#include <zippylog/zippylog.hpp>
#include <zippylog/message.pb.h>

#include <google/protobuf/message.h>
#include <gtest/gtest_prod.h>
#include <zmq.hpp>

#include <ostream>
#include <string>

// windows.h defines GetMessage() as a macro, which craps on us.
// here, we shovel shit
#if defined(WINDOWS) && defined(GetMessage)

inline BOOL GetMessage_Windows(LPMSG msg, HWND hwnd, UINT min, UINT max) {
    return GetMessage(msg, hwnd, min, max);
}

#undef GetMessage

inline BOOL GetMessage(LPMSG msg, HWND hwnd, UINT min, UINT max) {
    return GetMessage_Windows(msg, hwnd, min, max);
}

#endif

#if defined(MACOS) && defined(TYPE_BOOL)
#undef TYPE_BOOL
#endif

namespace zippylog {

/// Represents an envelope, a container for messages and other metadata
///
/// Envelopes are the core data element of zippylog. Envelopes are serialized
/// as protocol buffer messages. However, we purposefully wrap access to the
/// low-level protocol buffer message class with this class so we can perform
/// santitization, optimization, and provide overall convenience.
///
/// @todo optimize internal messages array so we don't realloc for every add
class ZIPPYLOG_EXPORT Envelope {
    public:
        /// Construct an empty envelope
        ///
        /// The only side effect is the envelope creation time is set to now
        Envelope();

        /// Construct from serialized binary data
        ///
        /// It is assumed the data referenced is a protocol buffer serialized
        /// envelope message.
        Envelope(const void *data, int size);

        /// Construct an envelope from a 0MQ message
        ///
        /// The optional second parameter defines the byte start offset from
        /// which to start deserialization. This is required for protocol
        /// messages, where the first byte is a format version. It defaults
        /// to 0, which means to deserialize from the entire message.
        Envelope(::zmq::message_t &m, uint32 start_offset = 0);

        /// Construct an envelope having string data
        ///
        /// This populates the string_value field of the envelope. It does
        /// not build an envelope from the serialized protocol buffer data
        /// stored in the passed string.
        ///
        /// It also sets the created time to the current time.
        Envelope(::std::string const &s);

        ~Envelope();

        /// Copies an envelope via assignment
        ///
        /// The produced envelope is a deep copy. Modifications to one should
        /// not affect the other.
        Envelope & operator=(Envelope const &orig);

        /// Construct an envelope from another one
        ///
        /// This behaves similarly to the assignment operator. A deep
        /// copy is performed.
        Envelope(Envelope const &e);

        /// Whether this envelope is equivalent to another one
        ///
        /// Will perform a deep compare of all envelope elements, including
        /// embedded messages. Will return true if two separate envelope
        /// instances contain the same data.
        bool operator==(Envelope const &other) const;

        /// Whether two envelopes are not equal
        ///
        /// This returns the opposite of the == operator
        bool operator!=(Envelope const &other) const;

        /// Serialize the envelope to the passed string
        ///
        /// The envelope is serialized to its protocol buffer representation.
        ///
        /// The serialized envelope will be appended to the string. If the
        /// original string contains data, it will be preserved.
        ///
        /// If this returns false, it likely means something in your program
        /// is very wrong, as serialization should never fail. You are likely
        /// passing in a bad string.
        ///
        /// @return whether we completely serialized to the string
        bool Serialize(::std::string &s) const;

        /// Serialize the envelope to a coded output stream
        ///
        /// The envelope's protocol buffer serialization is written to the
        /// stream.
        inline bool Serialize(::google::protobuf::io::CodedOutputStream * cos) const
        {
            return this->envelope.SerializePartialToCodedStream(cos);
        }

        /// Adds a zippylog message (protocol buffer message) to the envelope
        ///
        /// The protocol message should have an enumerated namespace and type
        /// associated with it. Those are the ns and enumeration arguments.
        ///
        /// The passed message is copied and added to the envelope. Therefore,
        /// modifications to the message after adding are not reflected.
        ///
        /// We require namespace and enumeration now. If we can get around
        /// inefficient castings, we'll likely create a new overload
        bool AddMessage(::google::protobuf::Message &m, uint32 ns, uint32 enumeration);

        /// Read an envelope from a coded input stream into the instance
        ///
        /// Returns whether the parse was successful.
        bool ParseFromCodedInputStream(::google::protobuf::io::CodedInputStream &cis);

        /// Serializes the envelope into a 0MQ message
        ///
        /// The entire data payload of the 0MQ message will be reset to the
        /// serialized content of this envelope.
        ///
        /// This does @b not add a protocol format header. For that, see
        /// ToZmqProtocolMessage().
        bool ToZmqMessage(::zmq::message_t &msg);

        /// Serializes to a 0MQ message for the zippylog protocol
        ///
        /// This adds the appropriate header to the message to identify it as
        /// an envelope.
        bool ToProtocolZmqMessage(::zmq::message_t &msg) const;

        /// The number of messages contained within this envelope
        ///
        /// @return a non-negative number
        int MessageCount() const;

        /// Returns the namespace enumeration of a message at an index
        ///
        /// Index starts at 0.
        inline uint32 MessageNamespace(int index)
        {
            return this->envelope.message_namespace(index);
        }

        /// Returns the enumeration of a message at an index
        ///
        /// Index starts at 0.
        inline uint32 MessageType(int index)
        {
            return this->envelope.message_type(index);
        }

        /// The number of tags on the underlying envelope
        inline int TagSize() const
        {
            return this->envelope.tag_size();
        }

        /// Adds a tag to the envelope
        inline void AddTag(::std::string const &s)
        {
            this->envelope.add_tag(s);
        }

        /// Obtains a tag at the specified offset
        inline ::std::string const & GetTag(int index) const
        {
            return this->envelope.tag(index);
        }

        /// The number of elements in the message_namespace field
        inline int MessageNamespaceSize() const
        {
            return this->envelope.message_namespace_size();
        }

        /// The number of elements in the message_type field
        inline int MessageTypeSize() const
        {
            return this->envelope.message_type_size();
        }

        /// The size in bytes of the serialized envelope
        inline int SerializedByteSize() const
        {
            return this->envelope.ByteSize();
        }

        /// Whether the envelope has a simple string value
        inline bool HasStringValue() const
        {
            return this->envelope.has_string_value();
        }

        /// Obtains the string value field on the envelope
        inline ::std::string const & GetStringValueField() const
        {
            return this->envelope.string_value();
        }

        /// Sets the string value field for the envelope
        inline void SetStringValueField(::std::string const &s)
        {
            this->envelope.set_string_value(s);
        }

        /// Obtain the protocol buffer message at given index
        ///
        /// The returned pointer is owned by the envelope instance from which
        /// it came. In other words, the caller is not responsible for freeing
        /// it. And, the message isn't guaranteed to be accessible once the
        /// envelope is destroyed.
        ///
        /// The returned pointer points to the protocol buffer message type
        /// of the message at the requested index. So, many callers will
        /// cast the returned value to the type they were expecting.
        ///
        /// If called multiple times, the obtained pointer will refer to the
        /// same object.
        ///
        /// Changes to the obtained message won't be reflected in the
        /// envelope.
        ///
        /// Upon failure, NULL is returned. Failure could occur for a number
        /// of reasons:
        ///  - Index supplied is outside message range [0..MessageCount()]
        ///  - The envelope doesn't know the enumeration of that message
        ///  - This message's enumeration isn't mapped to a message type
        ///  - Error deserializing message
        ///
        /// @throw invalid_argument if index less than 0
        /// @return message pointer on success or NULL on failure
        ::google::protobuf::Message * GetMessage(int index);

        /// Copy an individual message into another envelope
        ///
        /// The message will be appended to the destination envelope.
        ///
        /// @param index Index of message to copy. Initial index is 0
        /// @param dest Envelope to copy message to
        /// @return Whether the copy succeeded
        bool CopyMessage(int index, Envelope &dest) const;

        /// Remove the message at the specified index
        ///
        /// @param index Index of message to remove. Initial index is 0.
        /// @return Whether message was removed successfully
        bool RemoveMessage(int32 index);

        /// Clears the contents of the envelope
        void Clear();

        /// Returns a human-readable string that describes the envelope
        ///
        /// The returned string shows all the envelope's fields and messages
        /// within.
        ::std::string ToString();

        /// Try to print a message using its custom formatter
        ///
        /// @param m Message to print
        /// @param s Stream to write message to
        /// @return Whether message had a custom formatter and we printed
        static bool CustomPrintMessage(::google::protobuf::Message const *m, ::std::ostream &s);

    protected:
        /// The underlying protocol buffer message
        message::Envelope envelope;

        /// Initializes the envelope from a buffer
        ///
        /// This is likely only called from a constructor.
        void InitializeFromBuffer(const void * data, int size);

        /// Cache of deserialized, obtained messages
        ::google::protobuf::Message ** messages;

        /// Size of messages array
        int messages_size;

    private:
        FRIEND_TEST(EnvelopeTest, UnknownMessageTypes);
        FRIEND_TEST(EnvelopeTest, MessageCacheResizing);
};

} // namespace

#endif
