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

#include <pblog/store.hpp>

#include <pblog/platform.hpp>

#include <sys/types.h>
#include <sys/stat.h>

using ::std::string;

namespace pblog {

Store::Store(const string path)
{
    struct _stat64 stat;
    if (_stat64(path.c_str(), &stat)) {
        throw "store path does not exist or could not be read";
    }

    this->_path = path;
}

Store::~Store()
{
}

bool Store::ValidatePath(const string path)
{
    if (path[0] != '/') {
        return false;
    }

    int32 seen_paths = 1;
    for (size_t i = path.length() - 1; i; i--) {
        if (path[i] == '/') {
            seen_paths++;
            if (seen_paths > 3) return false;
            continue;
        }
        else if (path[i] >= 48 && path[i] <= 57) {
            continue;
        }
        else if (path[i] >= 65 && path[i] <= 90) {
            continue;
        }
        else if (path[i] >= 97 && path[i] <= 122) {
            continue;
        }
        else if (path[i] == 45 || path[i] == 95) {
            continue;
        }

        return false;
    }

    return true;
}

bool Store::ParsePath(const string path, string &bucket, string &set, string &stream)
{
    string::size_type offsets[3];

    if (path.at(0) != '/') {
        return false;
    }

    string::size_type off = 1;
    for (size_t i = 0; i < 3; i++) {
        off = path.find_first_of("/", off);
        if (off == string::npos) {
            return false;
        }

        offsets[i] = off++;
    }

    bucket = path.substr(0, offsets[0] - 1);
    set = path.substr(offsets[0] + 1, offsets[1] - offsets[0]);
    stream = path.substr(offsets[1] + 1, offsets[2] - offsets[1]);

    return true;
}


const string Store::StorePath()
{
    return this->_path;
}

bool Store::BucketNames(vector<string> &buckets)
{
    return this->directories_in_directory(this->_path, buckets);
}

bool Store::StreamSetNames(const string bucket, vector<string> &sets)
{
    return this->directories_in_directory(this->PathToFilesystemPath(this->BucketPath(bucket)), sets);
}

bool Store::StreamNames(const string bucket, const string set, vector<string> &streams)
{
    this->files_in_directory(this->PathToFilesystemPath(this->StreamsetPath(bucket, set)), streams);

    for (size_t i = streams.size(); i; --i) {
        if (streams[i-1].substr(streams[i-i].length() - 6, 6).compare(".pblog")) {
            streams.pop_back();
        }
        streams[i-1] = streams[i-1].substr(0, streams[i-1].length() - 6);
    }

    return true;
}

string Store::BucketPath(const string bucket)
{
    string s = "/" + bucket;
    return s;
}

string Store::StreamsetPath(const string bucket, const string stream_set)
{
    string s = this->BucketPath(bucket);
    s.append("/");
    s.append(stream_set);
    return s;
}

string Store::StreamPath(const string bucket, const string stream_set, const string filename)
{
    string s = this->StreamsetPath(bucket, stream_set);
    s.append("/");
    s.append(filename);

    return s;
}

bool Store::BucketPaths(vector<string> &paths)
{
    vector<string> buckets;
    this->BucketNames(buckets);
    for (size_t i = 0; i < buckets.size(); i++) {
        paths.push_back(this->BucketPath(buckets[i]));
    }

    return true;
}

bool Store::StreamsetPaths(vector<string> &paths)
{
    vector<string> buckets;
    this->BucketNames(buckets);
    for (size_t i = 0; i < buckets.size(); i++) {
        vector<string> sets;
        this->StreamSetNames(buckets[i], sets);
        for (size_t j = 0; j < sets.size(); j++) {
            paths.push_back(this->StreamsetPath(buckets[i], sets[j]));
        }
    }

    return true;
}

bool Store::StreamLength(const string path, int64 &length)
{
    if (!ValidatePath(path)) return false;

    string full = this->StreamFilesystemPath(path);

    struct _stat64 stat;
    if (_stat64(full.c_str(), &stat)) {
        return false;
    }

    length = stat.st_size;
    return true;
}

bool Store::StreamInfo(const string path, pblog::protocol::StreamInfo &info)
{
    if (!ValidatePath(path)) return false;

    string bucket, set, stream;
    if (!ParsePath(path, bucket, set, stream)) {
        return false;
    }

    return StreamInfo(bucket, set, stream, info);

}

bool Store::StreamInfo(const string bucket, const string stream_set, const string stream, protocol::StreamInfo &info)
{
    info.set_path(stream);
    // TODO verify stream exists and populate other stuff

    int64 length = -1;
    if (this->StreamLength(this->StreamPath(bucket, stream_set, stream), length)) {
        info.set_length(length);
    }
    else {
        return false;
    }

    return true;
}

bool Store::StreamsetInfo(const string bucket, const string stream_set, protocol::StreamSetInfo &info)
{
    info.set_path(stream_set);

    vector<string> streams;
    this->StreamNames(bucket, stream_set, streams);

    for (size_t i = 0; i < streams.size(); i++) {
        protocol::StreamInfo * new_si = info.add_stream();
        this->StreamInfo(bucket, stream_set, streams[i], *new_si);
    }
    return true;
}

bool Store::StreamsetInfo(const string path, pblog::protocol::StreamSetInfo &info)
{
    if (!ValidatePath(path)) return false;

    string bucket, set, stream;
    if (!ParsePath(path, bucket, set, stream)) return false;

    return StreamsetInfo(bucket, set, info);
}

bool Store::BucketInfo(const string bucket, protocol::BucketInfo &info)
{
    info.set_path(bucket);

    vector<string> stream_sets;
    this->StreamSetNames(bucket, stream_sets);

    for (size_t i = 0; i < stream_sets.size(); i++) {
        protocol::StreamSetInfo *ss = info.add_stream_set();
        this->StreamsetInfo(bucket, stream_sets[i], *ss);
    }

    return true;
}

bool Store::StoreInfo(protocol::StoreInfo &info)
{
    vector<string> buckets;
    this->BucketNames(buckets);
    for (size_t i = 0; i < buckets.size(); i++) {
        protocol::BucketInfo *bucket = info.add_bucket();
        this->BucketInfo(buckets[i], *bucket);
    }

    return true;
}

bool Store::GetInputStream(const string path, InputStream &s)
{
    if (!ValidatePath(path)) return false;

    return s.OpenFile(this->StreamFilesystemPath(path));
}

bool Store::GetInputStream(const string bucket, const string stream_set, const string stream, InputStream &s)
{
    string path = this->StreamFilesystemPath(this->StreamPath(bucket, stream_set, stream));

    return s.OpenFile(path);
}


string Store::PathToFilesystemPath(const string path)
{
    string s = string(this->_path);
    s.append("/");
    s.append(path);

    return s;
}

string Store::StreamFilesystemPath(const string path)
{
    return this->PathToFilesystemPath(path) + ".pblog";
}

bool Store::directories_in_directory(const string dir, vector<string> &v)
{
    vector<dir_entry> entries;
    if (!directory_entries(dir, entries)) return false;

    for (size_t i = 0; i < entries.size(); i++) {
        if (entries[i].type == 1 && entries[i].name[0] != '.') {
            v.push_back(entries[i].name);
        }
    }

    return true;
}

bool Store::files_in_directory(const string dir, vector<string> &v)
{
    vector<dir_entry> entries;
    if (!directory_entries(dir, entries)) return false;

    for (size_t i = 0; i < entries.size(); i++) {
        if (entries[i].type == 2) {
            v.push_back(entries[i].name);
        }
    }

    return true;
}

} // namespace pblog