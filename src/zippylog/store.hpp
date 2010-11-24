//  Copyright 2010 Gregory Szorc
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

struct OpenOutputStream {
    OutputStream *stream;
    int64 last_write_time;
};

/// Represents a stream store
///
/// A stream store is a collection of buckets, stream sets, and streams. The
/// layout of a stream store is well-defined by the zippylog project.
///
/// Stores are often backed by regular filesystems, but this isn't a hard
/// requirements.
///
/// The implementation here is for a file-backed store. This should change to
/// an abstract base class.
/// TODO define an interface for the store API
///
/// Store objects are currently not thread safe. It is recommended that
/// multithreaded applications instantiate multiple class instances for
/// individual stores. However, even this is not thread safe because writing
/// is not safe across threads nor processes. Therefore, to write to a single
/// store across many threads or processes, it is recommended to make a
/// specific class instance the designated writer. All writes then go through
/// it. The StoreWriter device performs this task well.
/// TODO create locks on files so store writers in multiple processes can't
/// interfere.
class ZIPPYLOG_EXPORT Store {
    public:
        // construct a store from a filesystem path
        Store(const ::std::string path);
        ~Store();

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
        static bool ParsePath(const ::std::string path, ::std::string &bucket, ::std::string &set, ::std::string &stream);

        /// Return the path to a specific bucket
        static ::std::string BucketPath(const ::std::string bucket);

        /// Return the path to a specific stream set
        static ::std::string StreamsetPath(const ::std::string bucket, const ::std::string set);

        /// Return the path to a stream within a bucket and set
        static ::std::string StreamPath(const ::std::string bucket, const ::std::string set, const ::std::string stream);

        // Return the filesystem path to this store
        const ::std::string StorePath();

        /// Obtain the set of bucket names in the store
        bool BucketNames(::std::vector< ::std::string > &buckets);

        /// Obtain the set of stream sets in a specific bucket
        bool StreamSetNames(const ::std::string bucket, ::std::vector< ::std::string > &buckets);

        /// Obtain the set of stream names in a specific stream set
        bool StreamNames(const ::std::string bucket, const ::std::string set, ::std::vector< ::std::string > &streams);

        /// Obtain the set of all paths to known buckets
        bool BucketPaths(::std::vector< ::std::string > &paths);

        /// Obtain the set of all paths to known stream sets
        bool StreamsetPaths(::std::vector< ::std::string > &paths);

        /// Obtain the set of all paths to all known streams
        bool StreamPaths(::std::vector< ::std::string > &paths);

        /// Obtain the length of a stream
        bool StreamLength(const ::std::string path, int64 &length);

        /// Obtain a store info message describing this store
        bool StoreInfo(protocol::StoreInfo &info);

        /// Obtain a bucket info message describing the specified bucket
        bool BucketInfo(const ::std::string bucket, protocol::BucketInfo &info);

        /// Obtain a stream set info for the stream set specified
        bool StreamsetInfo(const ::std::string bucket, const ::std::string set, protocol::StreamSetInfo &info);

        /// Obtain a stream set info message for the stream set identifier by a path
        bool StreamsetInfo(const ::std::string path, protocol::StreamSetInfo &info);

        bool StreamInfo(const ::std::string bucket, const ::std::string set, const ::std::string stream, protocol::StreamInfo &info);
        bool StreamInfo(const ::std::string path, protocol::StreamInfo &info);

        /// Obtain an input stream from the store
        ///
        /// Returned pointer is NULL if the stream could not be opened for any
        /// reason. The returned memory address is owned by the store but the
        /// returned object is given to the caller and only the caller. This
        /// means the object will be cleaned up when the store is destroyed and
        /// the caller need not worry about thread contention on the object.
        InputStream * GetInputStream(const ::std::string &path);

        /// Obtain an input stream from the store.
        ///
        /// See the above function for usage.
        InputStream * GetInputStream(const ::std::string &bucket, const ::std::string &set, const ::std::string &stream);

        // create a bucket
        // if it exists already, will return true
        bool CreateBucket(const ::std::string bucket);

        // create a stream set
        bool CreateStreamset(const ::std::string bucket, const ::std::string stream_set);

        bool BucketExists(const ::std::string bucket);
        bool StreamsetExists(const ::std::string bucket, const ::std::string stream_set);

        // return the filesystem path for the given store path
        // no validation of input path is performed
        ::std::string PathToFilesystemPath(const ::std::string path);

        /// Writes an envelope to the stream set specified
        ///
        /// The stream written to corresponds to the time passed. If -1 (the
        /// default), the current time will be used. If a positive number, this
        /// represents the number of microseconds since UNIX epoch and the
        /// stream will be calculated from that.
        bool WriteEnvelope(const ::std::string bucket, const ::std::string set, Envelope &e, int64 time=-1);

        /// Writes envelope data from a buffer to the stream set specified
        ///
        /// Data is written with the proper serialization format for
        /// envelopes. Time semantics are equivalent to the function above.
        bool WriteEnvelope(const ::std::string &bucket, const ::std::string &set, const void *data, int length, int64 time=-1);

        /// returns the name of a stream for a particular time value
        static ::std::string StreamNameForTime(int64 time, int seconds_per_file);
        static ::std::string StreamNameForTime(platform::Time &time, int seconds_per_file);

        /// Flushes all registered output streams
        ///
        /// Output streams will cache data in an im-memory buffer. When the
        /// buffer is full, it will then write to the underlying file
        /// descriptor. This API not only forces the buffer to write to
        /// the file descriptor but also calls the system API to flush the
        /// descriptor to its backing store.
        bool FlushOutputStreams();

    protected:
        bool ObtainOutputStream(const ::std::string bucket, const ::std::string set, int seconds_per_file, OpenOutputStream &stream, int64 time=-1);

        ::std::string StreamFilesystemPath(const ::std::string path);

        /// holds pointers to retrieved input streams
        ///
        /// streams will be destroyed upon object destruction
        ::std::vector<InputStream *> input_streams;
    private:
        // disable copy constructor and assignment operator
        Store(const Store &orig);
        Store & operator=(const Store &orig);

        ::std::string _path;
        ::std::map< ::std::string, OpenOutputStream> out_streams;
};

} // namespace zippylog

#endif
