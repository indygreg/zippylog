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

// represents a stream store
// TODO define an interface for the store API
class ZIPPYLOG_EXPORT Store {
    public:
        // construct a store from a filesystem path
        Store(const ::std::string path);
        ~Store();

        // validates that a path string is sane
        // this checks for things like beginning with a '/', not containing
        // illegal characters like '.', etc
        static bool ValidatePath(const ::std::string path);

        // parses a path string into its components
        // if this returns true, the path is valid and the strings passed
        // by reference are updated to the extracted values
        // if a field is empty, that string is empty()
        static bool ParsePath(const ::std::string path, ::std::string &bucket, ::std::string &set, ::std::string &stream);

        // return the path to a specific bucket
        static ::std::string BucketPath(const ::std::string bucket);

        // return the path to a specific stream set
        static ::std::string StreamsetPath(const ::std::string bucket, const ::std::string set);

        // return the path to a stream within a bucket and set
        static ::std::string StreamPath(const ::std::string bucket, const ::std::string set, const ::std::string stream);

        // return the filesystem path to this store
        const ::std::string StorePath();

        // obtain the set of bucket names in the store
        bool BucketNames(::std::vector< ::std::string > &buckets);

        // obtain the set of stream sets in a specific bucket
        bool StreamSetNames(const ::std::string bucket, ::std::vector< ::std::string > &buckets);

        // obtain the set of stream names in a specific stream set
        bool StreamNames(const ::std::string bucket, const ::std::string set, ::std::vector< ::std::string > &streams);

        // obtain the set of all paths to known buckets
        bool BucketPaths(::std::vector< ::std::string > &paths);

        // obtain the set of all paths to known stream sets
        bool StreamsetPaths(::std::vector< ::std::string > &paths);

        // obtain the set of all paths to all known streams
        bool StreamPaths(::std::vector< ::std::string > &paths);

        // obtain the length of a stream
        bool StreamLength(const ::std::string path, int64 &length);

        bool StoreInfo(protocol::StoreInfo &info);
        bool BucketInfo(const ::std::string bucket, protocol::BucketInfo &info);
        bool StreamsetInfo(const ::std::string bucket, const ::std::string set, protocol::StreamSetInfo &info);
        bool StreamsetInfo(const ::std::string path, protocol::StreamSetInfo &info);

        bool StreamInfo(const ::std::string bucket, const ::std::string set, const ::std::string stream, protocol::StreamInfo &info);
        bool StreamInfo(const ::std::string path, protocol::StreamInfo &info);

        bool GetInputStream(const ::std::string path, InputStream &s);
        bool GetInputStream(const ::std::string bucket, const ::std::string set, const ::std::string stream, InputStream &s);

        // create a bucket
        // if it exists already, will return true
        bool CreateBucket(const ::std::string bucket);

        // create a stream set
        bool CreateStreamset(const ::std::string bucket, const ::std::string stream_set);

        bool BucketExists(const ::std::string bucket);
        bool StreamsetExists(const ::std::string bucket, const ::std::string stream_set);

        // return the filesystem path for the given store path
        // no validation of input path is performed
        string PathToFilesystemPath(const ::std::string path);

        // Writes an envelope calculating the stream based on the time
        // if no time parameter is defined, time is assumed to be now()
        bool WriteEnvelope(const ::std::string bucket, const ::std::string set, Envelope &e, int64 time=-1);
        bool WriteEnvelope(const ::std::string &bucket, const ::std::string &set, const void *data, int length, int64 time=-1);

        bool WriteData(const ::std::string bucket, const ::std::string set, const void *data, int length, int64 time=-1);
        bool WriteString(const ::std::string bucket, const ::std::string set, const ::std::string &s, int64 time=-1);

        // returns the name of a stream for a particular time value
        static ::std::string StreamNameForTime(int64 time, int seconds_per_file);
        static ::std::string StreamNameForTime(platform::Time &time, int seconds_per_file);

        // flushes all registered output streams
        // output streams will cache data in an im-memory buffer. when the
        // buffer is full, it will then write to the underlying file
        // descriptor. This API not only forces the buffer to write to
        // the file descriptor but also calls the system API to flush the
        // descriptor to its backing store.
        bool FlushOutputStreams();

    protected:
        bool ObtainOutputStream(const ::std::string bucket, const ::std::string set, int seconds_per_file, OpenOutputStream &stream, int64 time=-1);

        string StreamFilesystemPath(const ::std::string path);

    private:
        // disable copy constructor and assignment operator
        Store(const Store &orig);
        Store & operator=(const Store &orig);

        string _path;
        ::std::map< ::std::string, OpenOutputStream> out_streams;
};

} // namespace zippylog

#endif
