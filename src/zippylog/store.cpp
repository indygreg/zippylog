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

#include <zippylog/store.hpp>

#include <zippylog/platform.hpp>

#include <stdio.h>

using ::std::map;
using ::std::string;
using ::std::vector;

namespace zippylog {

Store::~Store()
{
    map<string, OpenOutputStream>::iterator i = this->out_streams.begin();
    for (; i != this->out_streams.end(); i++) {
        if (i->second.stream) delete i->second.stream;
    }

    vector<InputStream *>::iterator iter = this->input_streams.begin();
    for (; iter != this->input_streams.end(); iter++) {
        if (*iter) delete *iter;
    }
}

Store * Store::CreateStore(const string &s)
{
    string::size_type i = s.find_first_of("://");
    if (i == string::npos) {
        throw "store path not in expected URI format";
    }

    string proto = s.substr(0, i);
    if (proto != "simpledirectory") {
        throw "store protocol not understood";
    }

    string path = s.substr(i + 3, s.length() - i - 3);
    SimpleDirectoryStore *store = new SimpleDirectoryStore(path);
    return store;
}

bool Store::ValidatePath(const string path)
{
    if (path[0] != '/') {
        return false;
    }

    int32 seen_paths = 1;
    for (size_t i = 1; i < path.length(); i++) {
        if (path[i] == '/') {
            // two slashes in a row is not allowed
            if (path[i-1] == '/') return false;

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
        // allow file extensions for stream paths
        else if (seen_paths == 3 && path[i] == '.') {
            if (path.substr(i, path.length() - i).compare(".zippylog") == 0)
                return true;
        }

        return false;
    }

    return true;
}

bool Store::ParsePath(const string &path, string &bucket, string &set, string &stream)
{
    if (!Store::ValidatePath(path)) return false;

    bucket.clear();
    set.clear();
    stream.clear();

    if (path.length() == 1) {
        return true;
    }

    int field = 0;
    string::size_type curr = 1;
    string::size_type off = 1;
    for (size_t i = 0; i < 3; i++) {
        off = path.find_first_of("/", off);

        if (off == string::npos) {
            switch (field) {
                case 0:
                    bucket = path.substr(1, path.length() - 1);
                    break;

                case 1:
                    set = path.substr(curr, path.length() - curr);
                    break;

                case 2:
                    stream = path.substr(curr, path.length() - curr);

                    if (stream.length() > 9) {
                        if (!stream.compare(stream.length() - 9, 9, ".zippylog")) {
                            stream = stream.erase(stream.length() - 9);
                        }
                    }

                    break;
            }

            return true;
        }

        switch (field) {
            case 0:
                bucket = path.substr(1, off - 1);
                break;

            case 1:
                set = path.substr(curr, off - curr);
                break;

            case 2:
                return false;
        }

        curr = ++off;
        field++;
    }

    return true;
}

string Store::BucketPath(const string &bucket)
{
    string s = "/" + bucket;
    return s;
}

string Store::StreamsetPath(const string &bucket, const string &stream_set)
{
    string s = BucketPath(bucket);
    s.append("/");
    s.append(stream_set);
    return s;
}

string Store::StreamPath(const string &bucket, const string &stream_set, const string &filename)
{
    string s = StreamsetPath(bucket, stream_set);
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
    paths.clear();

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

bool Store::StreamPaths(vector<string> &paths)
{
    paths.clear();

    vector<string> buckets;
    this->BucketNames(buckets);
    for (vector<string>::iterator bucket = buckets.begin(); bucket != buckets.end(); bucket++) {
        vector<string> sets;
        this->StreamSetNames(*bucket, sets);

        for (vector<string>::iterator set = sets.begin(); set != sets.end(); set++) {
            vector<string> p;
            this->StreamNames(*bucket, *set, p);

            for (vector<string>::iterator stream = p.begin(); stream != p.end(); stream++) {
                paths.push_back(Store::StreamPath(*bucket, *set, *stream));
            }
        }
    }

    return true;
}

bool Store::StreamInfo(const string &path, zippylog::protocol::StreamInfo &info)
{
    if (!ValidatePath(path)) return false;

    string bucket, set, stream;
    if (!ParsePath(path, bucket, set, stream)) {
        return false;
    }

    return this->StreamInfo(bucket, set, stream, info);

}

bool Store::StreamInfo(const string &bucket, const string &stream_set, const string &stream, protocol::StreamInfo &info)
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

bool Store::StreamsetInfo(const string &bucket, const string &stream_set, protocol::StreamSetInfo &info)
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

bool Store::StreamsetInfo(const string &path, zippylog::protocol::StreamSetInfo &info)
{
    if (!ValidatePath(path)) return false;

    string bucket, set, stream;
    if (!ParsePath(path, bucket, set, stream)) return false;

    return this->StreamsetInfo(bucket, set, info);
}

bool Store::BucketInfo(const string &bucket, protocol::BucketInfo &info)
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

InputStream * Store::GetInputStream(const string &path)
{
    string bucket, set, stream;
    if (!ParsePath(path, bucket, set, stream)) return NULL;

    return this->GetInputStream(bucket, set, path);
}

bool Store::WriteEnvelope(const string &bucket, const string &set, Envelope &e, int64 time)
{
    OpenOutputStream os;
    if (!this->ObtainOutputStream(bucket, set, 3600, os, time)) {
        return false;
    }

    return os.stream->WriteEnvelope(e);
}

bool Store::WriteEnvelope(const string &bucket, const string &set, const void *data, int length, int64 time)
{
    OpenOutputStream os;
    if (!this->ObtainOutputStream(bucket, set, 3600, os, time)) return false;

    return os.stream->WriteEnvelope(data, length);
}

bool Store::ObtainOutputStream(const string &bucket, const string &set, int seconds_per_file, OpenOutputStream &os, int64 time)
{
    platform::Time t;

    if (time < 0) {
        platform::TimeNow(t);
    }
    else {
        platform::UnixMicroTimeToZippyTime(time, t);
    }

    string stream = Store::StreamNameForTime(t, seconds_per_file);

    string store_path = Store::StreamPath(bucket, set, stream);

    map<string, OpenOutputStream>::iterator found = this->out_streams.find(store_path);

    if (found != this->out_streams.end()) {
        os.stream = found->second.stream;
        os.last_write_time = found->second.last_write_time;
        return true;
    }
    // else
    os.stream = this->CreateOutputStream(bucket, set, stream);
    if (!os.stream) return false;
    os.last_write_time = -1;

    this->out_streams[store_path] = os;
    return true;
}

bool Store::FlushOutputStreams()
{
    map<string, OpenOutputStream>::iterator i = this->out_streams.begin();

    for (; i != this->out_streams.end(); i++) {
        i->second.stream->Flush();
    }

    return true;
}

string Store::StreamNameForTime(platform::Time &t, int seconds_per_file)
{

    // daily
    if (seconds_per_file == 86400) {
        char s[11];
        sprintf(&s[0], "%04d-%02d-%02d", t.year, t.mon, t.mday);
        return string(&s[0], 10);
    }

    // hourly
    if (seconds_per_file == 3600) {
        char s[14];
        sprintf(&s[0], "%04d-%02d-%02d-%02d", t.year, t.mon, t.mday, t.hour);
        return string(&s[0], 13);
    }

    if (seconds_per_file > 3600 || seconds_per_file < 4) {
        throw "seconds_per_file value not valid";
    }

    // this could be optimized into a static table for faster lookup

    if (3600 % seconds_per_file > 0) {
        throw "seconds_per_file does not divide into 3600 evenly";
    }

    int series_max = 3600 / seconds_per_file;
    int seconds_since_hour = t.min * 60 + t.sec;
    int current = seconds_since_hour / seconds_per_file + 1;

    char s[22];
    sprintf(&s[0], "%04d-%02d-%02d-%02d-%03d-%03d", t.year, t.mon, t.mday, t.hour, current, series_max);

    return string(&s[0], 21);
}

string Store::StreamNameForTime(int64 time, int seconds_per_file)
{
    platform::Time t;
    platform::UnixMicroTimeToZippyTime(time, t);

    return Store::StreamNameForTime(t, seconds_per_file);
}

SimpleDirectoryStore::SimpleDirectoryStore(const string &path) : root_path(path)
{
    if (!platform::PathIsDirectory(path)) {
        throw "store path does not exist or could not be read";
    }
}

const string SimpleDirectoryStore::RootDirectoryPath() const
{
    return this->root_path;
}

bool SimpleDirectoryStore::BucketNames(vector<string> &buckets)
{
    return platform::DirectoriesInDirectory(this->root_path, buckets);
}

bool SimpleDirectoryStore::StreamSetNames(const string &bucket, vector<string> &sets)
{
    return platform::DirectoriesInDirectory(
        this->PathToFilesystemPath(this->BucketPath(bucket)),
        sets
    );
}

bool SimpleDirectoryStore::StreamNames(const string &bucket, const string &set, vector<string> &streams)
{
    platform::FilesInDirectory(
        this->PathToFilesystemPath(this->StreamsetPath(bucket, set)),
        streams
    );

    vector<string>::iterator i = streams.begin();
    for (; i != streams.end(); ) {
        if (i->length() < 10 || i->substr(i->length() - 9, 9).compare(".zippylog")) {
            i = streams.erase(i);
            continue;
        }
        *i = i->substr(0, i->length() - 9);
        i++;
    }

    return true;
}

bool SimpleDirectoryStore::StreamLength(const string &path, int64 &length)
{
    if (!ValidatePath(path)) return false;

    string full = this->StreamFilesystemPath(path);

    platform::FileStat stat;
    if (!platform::stat(full, stat)) {
        return false;
    }

    length = stat.size;
    return true;
}

bool SimpleDirectoryStore::CreateBucket(const string &bucket)
{
    return platform::MakeDirectory(this->PathToFilesystemPath(Store::BucketPath(bucket)));
}

bool SimpleDirectoryStore::CreateStreamset(const string &bucket, const string &set)
{
    return platform::MakeDirectory(this->PathToFilesystemPath(Store::StreamsetPath(bucket, set)));
}

string SimpleDirectoryStore::PathToFilesystemPath(const string &path) const
{
    string s = string(this->root_path);
    s.append("/");
    s.append(path);

    return s;
}

string SimpleDirectoryStore::StreamFilesystemPath(const string &path)
{
    return this->PathToFilesystemPath(path).append(".zippylog");
}

bool SimpleDirectoryStore::BucketExists(const string &bucket)
{
    return platform::PathIsDirectory(this->PathToFilesystemPath(Store::BucketPath(bucket)));
}

bool SimpleDirectoryStore::StreamsetExists(const string &bucket, const string &set)
{
    return platform::PathIsDirectory(this->PathToFilesystemPath(Store::StreamsetPath(bucket, set)));
}

InputStream * SimpleDirectoryStore::GetInputStream(const string &bucket, const string &stream_set, const string &stream)
{
    string path = this->StreamFilesystemPath(this->StreamPath(bucket, stream_set, stream));

    FileInputStream *s = new FileInputStream(path);

    this->input_streams.push_back(s);

    return s;
}

OutputStream * SimpleDirectoryStore::CreateOutputStream(const string &bucket, const string &set, const string &stream)
{
    if (!this->BucketExists(bucket)) {
        if (!this->CreateBucket(bucket)) return NULL;
    }

    if (!this->StreamsetExists(bucket, set)) {
        if (!this->CreateStreamset(bucket, set)) return NULL;
    }

    string path = Store::StreamPath(bucket, set, stream);
    string fs_path = this->PathToFilesystemPath(path);

    return new FileOutputStream(fs_path);
}

} // namespace zippylog