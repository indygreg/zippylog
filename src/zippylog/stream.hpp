//  Copyright 2011 Gregory Szorc
//
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

#ifndef ZIPPYLOG_STREAM_HPP_
#define ZIPPYLOG_STREAM_HPP_

#include <zippylog/zippylog.hpp>
#include <zippylog/envelope.hpp>
#include <zippylog/platform.hpp>

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <string>

namespace zippylog {

// since streams are only ever appended to, it doesn't make sense to model a
// read/write stream, so we have a separate input and output stream

/// Represents a stream that we read from
///
/// This is an abstract base class for all readable streams.
///
/// Derived classes must:
///
///  - Implement ConstructCodedInputStream()
///  - Call RebuildCodedInputStream() in their constructor
///
/// We have some annoying code to work around a Protocol Buffers limitation
/// for the total number of bytes that can be read from a CodedInputStream.
/// CodedInputStream instances can only read a max of INT_MAX or ~2 billion
/// bytes before they barf. Google people have said on the PB mailing list
/// that the preferred solution is for CodedInputStream to be short-lived.
/// I think this is silly, especially for a high performance application
/// (the alternative message parse APIs construct a CodedInputStream on the
/// stack for each read message). So, we work around this by rebuilding
/// CodedInputStreams periodically, well before they will hit their read
/// limit.
class ZIPPYLOG_EXPORT InputStream {
    public:
        /// Construct an empty input stream
        ///
        /// This is provided so a default constructor is available. It is
        /// pretty useless.
        InputStream();

        virtual ~InputStream();

        /// Size (in bytes) of the next envelope in the stream
        /// does NOT include size of envelope size encoding
        uint32 NextEnvelopeSize();

        /// Read an envelope from the stream
        ///
        /// If returns true, an envelope is deserialized and placed in the
        /// variable passed and bytes_read is set to the number of bytes read
        /// from the stream to obtain this envelope. This number could be
        /// smaller or larger than the serialized envelope size due to stream
        /// errors or stream storage techniques.
        bool ReadEnvelope(::zippylog::Envelope &envelope, uint32 &bytes_read);

        /// Obtains the current envelope offset of the stream
        ///
        /// Note that if the stream is backed by a file descriptor or similar,
        /// that offset is different from this one. This offset is the offset
        /// within the stream at which this class itself is reading.
        ///
        /// The offset value can also disagree slightly with how much data has
        /// already been read, as the class sometimes reads ahead.
        int64 CurrentEnvelopeOffset() const { return this->offset; }

        /// Can we set absolute stream offsets
        ///
        /// If true, calls to SetAbsoluteOffset() are expected to return
        /// success.
        virtual bool CanSetAbsoluteOffset() const = 0;

        /// Sets the absolute offset of the stream
        ///
        /// Returns false if the operation not supported or if the operation
        /// failed. See CanSetAbsoluteOffset() to determine whether setting
        /// absolute offsets is possible.
        virtual bool SetAbsoluteOffset(int64 offset) = 0;

    protected:
        /// Constructs the CodedInputStream for the current stream
        ///
        /// This function is called when the CodedInputStream that the base
        /// class reads from needs to be rebuilt. Derived classes should
        /// return a pointer to a newly allocated CodedInputStream that is
        /// ready for reading. If the function cannot do this, it should throw
        /// an exception, as this is a fatal error. The returned memory
        /// address is owned by this base class and will be freed by it, not
        /// the derived class.
        ///
        /// The implementation should assume that the original
        /// CodedInputStream has already been destroyed (a side-effect is that
        /// bytes were returned to wherever they came from). Implementations
        /// should not set this->cis when called but should let the caller
        /// (this class) do that.
        virtual ::google::protobuf::io::CodedInputStream *
            ConstructCodedInputStream() = 0;

        /// Reads the stream version
        ///
        /// Returns true if successful and the stream is supported.
        /// This function is meant to be called by stream implementations when
        /// they have opened the stream.
        bool ReadVersion();

        /// Rebuilds the CodedInputStream instance on this stream
        ///
        /// This is a convenience method, as a number of APIs can invoke
        /// stream rebuild.
        void RebuildCodedInputStream();

        /// Low-level stream doing the reading
        ::google::protobuf::io::CodedInputStream *cis;

        /// Number of bytes current input stream has read
        ///
        /// This is used to keep track of when to rebuild the input stream due
        /// to read limits.
        int32 coded_input_stream_read_bytes;

        /// Stream version
        char version;

        /// Whether the size of the next envelope is available
        bool have_next_size;

        /// The size of the next envelope (if available)
        uint32 next_envelope_size;

        /// Current stream read offset
        int64 offset;

    private:
        // disable copy constructor and assignment operator
        InputStream(InputStream const &orig);
        InputStream & operator=(InputStream const &orig);
};

/// An input stream backed by a file
class ZIPPYLOG_EXPORT FileInputStream : public InputStream {
    public:
        /// Construct a stream from an existing file
        ///
        /// Optionally seek to specified offset in stream (in bytes)
        ///
        /// @param path Filesystem path to open
        /// @param start_offset Seek offset within stream
        FileInputStream(::std::string const &path, int64 start_offset = 0);

        /// Construct a new stream from a file descriptor
        ///
        /// The file descriptor must be opened for reading. In addition, the
        /// stream version must be obtained before calling this constructor.
        ///
        /// @param fd File descriptor to construct stream from
        /// @param version Stream version
        FileInputStream(int fd, char version);

        ~FileInputStream();

        bool CanSetAbsoluteOffset() const { return true; }

        bool SetAbsoluteOffset(int64 offset);

    protected:
        ::google::protobuf::io::CodedInputStream * ConstructCodedInputStream();

        /// The file object representing the underlying stream
        ::zippylog::platform::File file;

        /// Protobuf buffer stream wrapper
        ::google::protobuf::io::FileInputStream *fis;

    private:
        FileInputStream(FileInputStream const &orig);
        FileInputStream & operator=(FileInputStream const &orig);
};

/// A stream used for writing data
///
/// This is an abstract base class. You are likely interested in a class that
/// actually does something, like FileOutputStream.
///
/// Currently, OutputStream's are not thread safe. In fact, multiple instances
/// pointing to the same file are not safe. The reason is buffering.
/// Currently, the protocol buffers output streams buffer data before it is
/// written to an underlying file descriptor. Flushes don't necessarily occur
/// at boundaries we desire. If multiple output streams are writing to the same
/// file, for example, a partial envelope could be flushed by one and another
/// could write out a full envelope shortly thereafter, corrupting the stream.
///
/// In a nutshell, only allow one OutputStream per output file globally
/// @todo establish better sanity around output stream usage
class ZIPPYLOG_EXPORT OutputStream {
    public:
        /// Create an empty output stream object
        ///
        /// Really doesn't do much of anything. Provided for API completeness.
        OutputStream();
        virtual ~OutputStream();

        /// Writes an envelope to the stream
        ///
        /// NOT thread safe. if multiple threads call this, it may write a corrupt stream
        bool WriteEnvelope(::zippylog::Envelope &envelope);

        /// Writes an envelope from a buffer
        ///
        /// Unlike WriteData(), this will add the varint length to the stream,
        /// as is necessary from the stream specification
        bool WriteEnvelope(const void *data, int length);

        /// Flush the written data to the backing store
        ///
        /// This may be a noop in implementations.
        virtual bool Flush() = 0;

protected:
        /// Writes data with specified length to stream
        ///
        /// This is a very low-level API. It should not be used unless
        /// you know what you are doing.
        bool WriteData(const void *data, int length);

        /// Writes data inside a string to stream
        ///
        /// Equivalent to WriteData(), but for data within a string
        bool WriteString(::std::string const &s) { return this->WriteData(s.data(), s.length()); }

        /// Writes the stream header
        ///
        /// This must be called on new streams. It currently writes the stream
        /// version byte and flushes output so the stream is available for
        /// reading immediately.
        bool WriteStreamHeader();

        /// Protocol buffer wrapper for output stream
        ::google::protobuf::io::CodedOutputStream *cos;
};

class ZIPPYLOG_EXPORT FileOutputStream : public OutputStream {
    public:
        /// Opens the file specified by path for writing
        ///
        /// File is opened in append mode so all writes automatically go to
        /// end of file. Caller can optionally request a write lock on
        /// the file. The write lock will be exclusive across the entire
        /// operating system and will apply to the whole file. If the write
        /// lock cannot be obtained, the constructor will throw an exception.
        FileOutputStream(::std::string const &path, bool write_lock = false);

        ~FileOutputStream();

        /// Flushes the output stream
        ///
        /// This will flush all data buffered in the protocol buffer output
        /// stream classes and flush the underlying file descriptor. This
        /// should cause pending data to commit to disk.
        bool Flush();

    protected:
        /// Underlying file object being used
        platform::File file;

        /// Protobuf buffer wrapper for file writing
        ::google::protobuf::io::FileOutputStream *os;
};

} // namespace

#endif