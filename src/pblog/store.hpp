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

#ifndef PBLOG_STORE_HPP_
#define PBLOG_STORE_HPP_

#include <pblog/pblog.h>

#include <pblog/protocol.pb.h>
#include <pblog/stream.hpp>

#include <vector>

namespace pblog {

using ::std::vector;
using ::std::string;

// represents a stream store
// functions are reentrant and thread-safe unless otherwise specified
// TODO define an interface for the store API
class PBLOG_EXPORT Store {
    public:
        /** construct a store from a filesystem path */
        Store(const string path);
        ~Store();

        // validates that a path string is sane
        // this checks for things like beginning with a '/', not containing
        // illegal characters like '.', etc
        static bool ValidatePath(const string path);

        static bool ParsePath(const string path, string &bucket, string &set, string &stream);

        // return the filesystem path to this store
        const string StorePath();

        // obtain the set of bucket names in the store
        bool BucketNames(vector<string> &buckets);

        // obtain the set of stream sets in a specific bucket
        bool StreamSetNames(const string bucket, vector<string> &buckets);

        // obtain the set of stream names in a specific stream set
        bool StreamNames(const string bucket, const string set, vector<string> &streams);

        // return the path to a specific bucket
        string BucketPath(const string bucket);

        // return the path to a specific stream set
        string StreamsetPath(const string bucket, const string set);

        // return the path to a stream within a bucket and set
        string StreamPath(const string bucket, const string set, const string stream);

        // obtain the set of all paths to known buckets
        bool BucketPaths(vector<string> &paths);

        // obtain the set of all paths to known stream sets
        bool StreamsetPaths(vector<string> &paths);

        // obtain the set of all paths to all known streams
        bool StreamPaths(vector<string> &paths);

        // obtain the length of a stream
        bool StreamLength(const string path, int64 &length);

        bool StoreInfo(protocol::StoreInfo &info);
        bool BucketInfo(const string bucket, protocol::BucketInfo &info);
        bool StreamsetInfo(const string bucket, const string set, protocol::StreamSetInfo &info);
        bool StreamsetInfo(const string path, protocol::StreamSetInfo &info);

        bool StreamInfo(const string bucket, const string set, const string stream, protocol::StreamInfo &info);
        bool StreamInfo(const string path, protocol::StreamInfo &info);

        bool GetInputStream(const string path, InputStream &s);
        bool GetInputStream(const string bucket, const string set, const string stream, InputStream &s);

    protected:
        bool directories_in_directory(const string dir, vector<string> &v);
        bool files_in_directory(const string dir, vector<string> &v);

        string PathToFilesystemPath(const string path);
        string StreamFilesystemPath(const string path);


    private:
        string _path;
};

} // namespace pblog

#endif
