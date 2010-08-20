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

#include "pblog/store.hpp"

#include <apr_pools.h>
#include <apr_file_info.h>

using namespace ::std;

namespace pblog {

Store::Store(const char *path)
{
    apr_pool_t *p;
    apr_pool_create(&p, NULL);
    Store(path, p);
}

Store::Store(const char *path, apr_pool_t *p)
{
    /* TODO perform validation */
    this->_path = path;
    this->_p = p;
}

Store::~Store()
{
    apr_pool_destroy(this->_p);
}

vector<string> * Store::buckets()
{
    return this->directories_in_directory(this->_path);
}

vector<string> * Store::stream_sets_in_bucket(const string bucket)
{
    return this->directories_in_directory(this->bucket_directory(bucket));
}

vector<string> * Store::streams_in_stream_set(const string bucket, const string stream_set)
{
    vector<string> *files = this->files_in_directory(this->stream_set_directory(bucket, stream_set));

    for (size_t i = files->size() - 1; i; --i) {
        if (files->at(i).substr(-6, 6).compare(".pblog")) {
            files->pop_back();
        }
    }

    return files;
}

vector<string> * Store::stream_paths()
{
    vector<string> *l = new vector<string>();
    vector<string> *buckets = this->buckets();

    for (size_t i = buckets->size(); i; --i) {
        vector<string> *sets = this->stream_sets_in_bucket(buckets->at(i-1));

        for (size_t j = sets->size(); j; --j) {
            vector<string> *streams = this->streams_in_stream_set(buckets->at(i-1), sets->at(j-1));

            for (size_t k = streams->size(); k; --k) {
                l->push_back(this->stream_path(
                    buckets->at(i-1),
                    sets->at(j-1),
                    streams->at(k-1)
                ));
            }

            delete streams;
        }

        delete sets;
    }

    delete buckets;

    return l;
}

string Store::bucket_path(const string bucket)
{
    string s = "/";
    s.append(bucket);
    return s;
}

string Store::bucket_directory(const string bucket)
{
    string s;
    s.append(this->_path);
    s.append("/");
    s.append(bucket);
    return s;
}

string Store::stream_set_path(const string bucket, const string stream_set)
{
    string s = this->bucket_path(bucket);
    s.append("/");
    s.append(stream_set);
    return s;
}

string Store::stream_set_directory(const string bucket, const string stream_set)
{
    string s = this->bucket_directory(bucket);
    s.append("/");
    s.append(stream_set);
    return s;
}

string Store::stream_path(const string bucket, const string stream_set, const string filename)
{
    string s = this->stream_set_path(bucket, stream_set);
    s.append("/");
    s.append(filename);

    return s;
}

string Store::path_to_filesystem_path(const string path)
{
    string s = string(this->_path);
    s.append("/");
    s.append(path);

    return s;
}

bool Store::stream_info(const string bucket, const string stream_set, const string stream, protocol::StreamInfo &info)
{
    info.set_path(stream);
    // TODO verify stream exists and populate other stuff

    return true;
}

bool Store::stream_set_info(const string bucket, const string stream_set, protocol::StreamSetInfo &info)
{
    info.set_path(stream_set);

    vector<string> *streams = this->streams_in_stream_set(bucket, stream_set);

    for (size_t i = 0; i < streams->size(); i++) {
        protocol::StreamInfo * new_si = info.add_stream();
        this->stream_info(bucket, stream_set, streams->at(i), *new_si);
    }

    delete streams;
    return true;
}

bool Store::bucket_info(const string bucket, protocol::BucketInfo &info)
{
    info.set_path(bucket);

    vector<string> *stream_sets = this->stream_sets_in_bucket(bucket);
    for (size_t i = 0; i < stream_sets->size(); i++) {
        protocol::StreamSetInfo *ss = info.add_stream_set();
        this->stream_set_info(bucket, stream_sets->at(i), *ss);
    }
    delete stream_sets;

    return true;
}

bool Store::store_info(protocol::StoreInfo &info)
{
    vector<string> * buckets = this->buckets();
    for (size_t i = 0; i < buckets->size(); i++) {
        protocol::BucketInfo *bucket = info.add_bucket();
        this->bucket_info(buckets->at(i), *bucket);
    }
    delete buckets;

    return true;
}

bool Store::parse_stream_path(const string path, string &bucket, string &set, string &stream)
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

bool Store::get_input_stream(const string path, InputStream &s)
{
    return s.OpenFile(this->path_to_filesystem_path(path));
}

bool Store::get_input_stream(const string bucket, const string stream_set, const string stream, InputStream &s)
{
    string path = this->path_to_filesystem_path(this->stream_path(bucket, stream_set, stream));

    return s.OpenFile(path);
}

vector<string> * Store::directories_in_directory(const string dir)
{
    apr_status_t st;
    apr_dir_t *e;
    apr_finfo_t info;

    vector<string> *v = new vector<string>();

    st = apr_dir_open(&e, dir.c_str(), this->_p);
    if (st != APR_SUCCESS) {
        return v;
    }


    while (true) {
        st = apr_dir_read(&info, APR_FINFO_TYPE, e);
        if (st != APR_SUCCESS) break;

        if (info.filetype == APR_DIR) {
            if (info.name[0] != '.') {
                v->push_back(info.name);
            }
        }
    }

    return v;
}

vector<string> * Store::files_in_directory(const string dir)
{
    apr_status_t st;
    apr_dir_t *e;
    apr_finfo_t info;

    vector<string> *v = new vector<string>();

    st = apr_dir_open(&e, dir.c_str(), this->_p);
    if (st != APR_SUCCESS) {
        return v;
    }

    while (true) {
        st = apr_dir_read(&info, APR_FINFO_TYPE, e);
        if (st != APR_SUCCESS) break;
        if (info.filetype == APR_REG) {
            v->push_back(info.name);
        }
    }

    return v;
}

} // namespace pblog