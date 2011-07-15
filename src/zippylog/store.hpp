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

#ifndef ZIPPYLOG_STORE_HPP_
#define ZIPPYLOG_STORE_HPP_

#include <zippylog/zippylog.hpp>

#include <zippylog/platform.hpp>
#include <zippylog/protocol.pb.h>
#include <zippylog/stream.hpp>

#include <map>
#include <vector>

namespace zippylog {

/// Record used by store to keep track of open output streams
class OpenOutputStream {
    public:
        OpenOutputStream() : stream(NULL), last_write_time(-1) { }

        OutputStream *stream;
        int64 last_write_time;
};

class ZIPPYLOG_EXPORT InvalidStoreUriException : public ::std::invalid_argument
{
    public:
        InvalidStoreUriException(::std::string const &s) : ::std::invalid_argument(s) { }
};

class ZIPPYLOG_EXPORT UnsupportedStoreUriException : public ::std::invalid_argument
{
    public:
        UnsupportedStoreUriException(::std::string const &s) : ::std::invalid_argument(s) { }
};

/// Represents a stream store
///
/// A stream store is a collection of buckets, stream sets, and streams. The
/// layout of a stream store is well-defined by the zippylog project.
///
/// Stores can be backed in any way possible, as long as basic primitives
/// are supported. Therefore, this class is an abstract base class that
/// implementations must inherit from.
///
/// Store objects are currently not thread safe. It is recommended that
/// multithreaded applications instantiate multiple class instances for
/// individual stores. However, even this is not guaranteed to be thread safe
/// because store implementations may not be safe. Consult the store
/// implementation's documentation for thread and process safety.
///
/// Store instances maintain handles to output streams, which are used for
/// writing. Each store instance maintains its own set of handles. The store
/// is guaranteed to have only one output stream open for each physical stream.
class ZIPPYLOG_EXPORT Store {
    public:
        virtual ~Store();

        /// Create a store object
        ///
        /// The store is created by calling the constructor of the store
        /// implementation referenced by the URI-like string passed.
        ///
        /// Currently, the implementation is hard-coded to only support the
        /// simple directory store, but this will change in future versions.
        ///
        /// The passed string should resemble:
        ///
        ///    simpledirectory:///path/to/root/dir
        static Store * CreateStore(::std::string const &uri);

        /// Validates that a store path string is sane
        ///
        /// This checks for things like beginning with a '/', not containing
        /// illegal characters like '.', etc. This function only checks string
        /// validity. It does not check whether a path actually exists.
        static bool ValidatePath(const ::std::string path);

        /// Parses a store path string into its components
        ///
        /// If this returns true, the path is valid and the strings passed
        /// by reference are updated to the extracted values. If a path field
        /// is not set, the corresponding string is empty().
        static bool ParsePath(::std::string const &path, ::std::string &bucket, ::std::string &set, ::std::string &stream);

        /// Parses the bucket name from a path string
        ///
        /// If the path contains additional path components, they will be ignored.
        /// If the path does not contain a bucket, will return false.
        static bool ParseBucketPath(::std::string const &path, ::std::string &bucket);

        /// Parses the bucket and stream set names from a path string
        ///
        /// If the path contains extra fields, they are ignored.
        /// If the path does not contain a stream set, returns false.
        static bool ParseStreamSetPath(::std::string const &path, ::std::string &bucket, ::std::string &set);

        /// Returns whether the path is a path to a bucket
        static bool IsBucketPath(::std::string const &path);

        /// Returns whether the path is a path to a stream set
        static bool IsStreamSetPath(::std::string const &path);

        /// Returns whether the specified path is a path to a stream
        static bool IsStreamPath(::std::string const &path);

        /// Return the store path to a specific bucket
        static ::std::string BucketPath(::std::string const &bucket);

        /// Return the store path to a specific stream set
        static ::std::string StreamsetPath(::std::string const &bucket, ::std::string const &set);

        /// Return the store path to a stream within a bucket and set
        static ::std::string StreamPath(::std::string const &bucket, ::std::string const &set, ::std::string const &stream);

        /// returns the name of a stream for a particular time value
        static ::std::string StreamNameForTime(int64 time, int seconds_per_file);
        static ::std::string StreamNameForTime(platform::Time &time, int seconds_per_file);

        /// Returns whether the entity identified by the path exists
        ///
        /// This is a convenient wrapper for the *Exists() functions that
        /// determines the appropriate function to call.
        bool PathExists(::std::string const &path);

        /// Obtain the set of all paths to known buckets
        ///
        /// This is like BucketNames() except the strings are store paths, not
        /// bucket names.
        bool BucketPaths(::std::vector< ::std::string > &paths);

        /// Obtain the set of all paths to known stream sets
        ///
        /// This is like StreamSetNames() except the strings are store paths.
        bool StreamsetPaths(::std::vector< ::std::string > &paths);

        /// Obtain the set of all paths to all known streams
        ///
        /// This is like StreamNames() except the strings are store paths.
        bool StreamPaths(::std::vector< ::std::string > &paths);

        /// Obtain a store info message describing this store
        /// @todo should protocol messages be part of the C++ API?
        bool StoreInfo(protocol::StoreInfoV1 &info);

        /// Obtain a bucket info message describing the specified bucket
        bool BucketInfo(::std::string const &bucket, protocol::BucketInfoV1 &info);

        /// Obtain a stream set info for the stream set specified
        bool StreamsetInfo(::std::string const &bucket, ::std::string const &set, protocol::StreamSetInfoV1 &info);

        /// Obtain a stream set info message for the stream set identifier by a path
        bool StreamsetInfo(::std::string const &path, protocol::StreamSetInfoV1 &info);

        bool StreamInfo(::std::string const &bucket, ::std::string const &set, ::std::string const &stream, protocol::StreamInfoV1 &info);
        bool StreamInfo(::std::string const &path, protocol::StreamInfoV1 &info);

        /// Obtain an input stream from the store
        ///
        /// Returned pointer is NULL if the stream could not be opened for any
        /// reason. The returned memory address is owned by the store but the
        /// returned object is given to the caller and only the caller. This
        /// means the object will be cleaned up when the store is destroyed and
        /// the caller need not worry about thread contention on the object.
        InputStream * GetInputStream(::std::string const &path);

        /// Writes an envelope to the stream set specified
        ///
        /// The stream written to corresponds to the time passed. If -1 (the
        /// default), the current time will be used. If a positive number, this
        /// represents the number of microseconds since UNIX epoch and the
        /// stream will be calculated from that.
        bool WriteEnvelope(::std::string const &bucket, ::std::string const &set, Envelope &e, int64 time=-1);

        /// Writes envelope data from a buffer to the stream set specified
        ///
        /// Data is written with the proper serialization format for
        /// envelopes. Time semantics are equivalent to the function above.
        bool WriteEnvelope(::std::string const &bucket, ::std::string const &set, const void *data, int length, int64 time=-1);

        /// Flushes all registered output streams
        ///
        /// Output streams will cache data in an im-memory buffer. When the
        /// buffer is full, it will then write to the underlying file
        /// descriptor. This API not only forces the buffer to write to
        /// the file descriptor but also calls the system API to flush the
        /// descriptor to its backing store.
        bool FlushOutputStreams();

        /// Obtain the set of bucket names in the store
        virtual bool BucketNames(::std::vector< ::std::string > &buckets) = 0;

        /// Obtain the set of stream sets in a specific bucket
        virtual bool StreamSetNames(::std::string const &bucket, ::std::vector< ::std::string > &buckets) = 0;

        /// Obtain the set of stream names in a specific stream set
        virtual bool StreamNames(::std::string const &bucket, ::std::string const &set, ::std::vector< ::std::string > &streams) = 0;

        /// Obtain the length of a stream
        ///
        /// The stream is specified by the store path.
        virtual bool StreamLength(::std::string const &path, int64 &length) = 0;

        /// Obtain an input stream from the store
        ///
        /// See the above function for usage.
        virtual InputStream * GetInputStream(::std::string const &bucket, ::std::string const &set, ::std::string const &stream) = 0;

        /// Create a bucket
        ///
        // If it exists already, will return true.
        virtual bool CreateBucket(::std::string const &bucket) = 0;

        /// Create a stream set
        ///
        /// If it already exists, will return true.
        virtual bool CreateStreamset(::std::string const &bucket, ::std::string const &stream_set) = 0;

        /// Returns whether a bucket exists
        virtual bool BucketExists(::std::string const &bucket) = 0;

        /// Returns whether a stream set exists
        virtual bool StreamSetExists(::std::string const &bucket, ::std::string const &stream_set) = 0;

        /// Returns whether a stream exists
        virtual bool StreamExists(::std::string const &bucket, ::std::string const &stream_set, ::std::string const &stream) = 0;

    protected:
        Store() { };
        bool ObtainOutputStream(::std::string const &bucket, ::std::string const &set, int seconds_per_file, OpenOutputStream &stream, int64 time=-1);

        /// Create an output stream object
        virtual OutputStream * CreateOutputStream(::std::string const &bucket, ::std::string const &set, ::std::string const &stream) = 0;

        /// holds pointers to retrieved input streams
        ///
        /// streams will be destroyed upon object destruction
        ::std::vector<InputStream *> input_streams;

        /// holds output streams the store has open
        ///
        /// Key is the stream path and value is an object containing details
        /// about the stream. Every time we write, we first try to obtain an
        /// output stream from here. If we can't find one, we create an output
        /// stream and put it here.
        ///
        /// @todo prune this data structure as needed or consider a ceiling
        /// limit of open streams
        ::std::map< ::std::string, OpenOutputStream> out_streams;
    private:
        // disable copy constructor and assignment operator
        Store(Store const &orig);
        Store & operator=(Store const &orig);
};

class ZIPPYLOG_EXPORT StorePathNotDirectoryException : public Exception
{
    public:
        StorePathNotDirectoryException(::std::string const &s) : Exception(s) { }
};

/// A stream store backed by a single directory
///
/// The store is created by specifying the filesystem path to a directory.
/// Buckets are subdirectories and stream sets subdirectories of the bucket
/// directories.
///
/// This is likely the simplest of stream store engines, since there are very
/// few moving parts. Just a simple directory tree constituting the store.
///
/// Output streams obtained from this implementation have exclusive write
/// locks on files. This means the first store in the whole operating system
/// to grab a handle to a specific stream wins. Output streams don't relinquish
/// the locks until they are closed and output streams generally aren't
/// closed aggressively by the store implementation. So, care must be taken
/// to avoid contention on systems running many stores.
class ZIPPYLOG_EXPORT SimpleDirectoryStore : public Store {
    public:
        /// Create a store using the specified path as the root directory
        ///
        /// The path should be a valid filesystem path for the current
        /// system. For UNIX, something like "/var/zippylog/store" or "store"
        /// should work. For Windows, "C:\zippylog" or similar.
        SimpleDirectoryStore(::std::string const &path);

        ~SimpleDirectoryStore() { };

        // Return the filesystem path to this store
        const ::std::string RootDirectoryPath() const;

        ::std::string PathToFilesystemPath(::std::string const &path) const;

        // implement virtual functions
        bool BucketNames(::std::vector< ::std::string > &buckets);
        bool StreamSetNames(::std::string const &bucket, ::std::vector< ::std::string > &buckets);
        bool StreamNames(::std::string const &bucket, ::std::string const &set, ::std::vector< ::std::string > &streams);
        bool StreamLength(::std::string const &path, int64 &length);
        InputStream * GetInputStream(::std::string const &bucket, ::std::string const &set, ::std::string const &stream);
        bool CreateBucket(::std::string const &bucket);
        bool CreateStreamset(::std::string const &bucket, ::std::string const &stream_set);
        bool BucketExists(::std::string const &bucket);
        bool StreamSetExists(::std::string const &bucket, ::std::string const &stream_set);
        bool StreamExists(::std::string const &bucket, ::std::string const &set, ::std::string const &stream);

    protected:
        ::std::string root_path;

        ::std::string StreamFilesystemPath(::std::string const &path);

        // implement virtual functions
        OutputStream * CreateOutputStream(::std::string const &bucket, ::std::string const &set, ::std::string const &stream);

    private:
        SimpleDirectoryStore(SimpleDirectoryStore const &orig);
        SimpleDirectoryStore & operator=(SimpleDirectoryStore const &orig);

};

} // namespace zippylog

#endif
