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

vector<string> * Store::stream_sets_in_bucket(const char *bucket)
{
    return this->directories_in_directory(this->bucket_directory(bucket).c_str());
}

vector<string> * Store::streams_in_stream_set(const char *bucket, const char *stream_set)
{
    vector<string> *files = this->files_in_directory(this->stream_set_directory(bucket, stream_set).c_str());

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
        vector<string> *sets = this->stream_sets_in_bucket(buckets->at(i-1).c_str());

        for (size_t j = sets->size(); j; --j) {
            vector<string> *streams = this->streams_in_stream_set(buckets->at(i-1).c_str(), sets->at(j-1).c_str());

            for (size_t k = streams->size(); k; --k) {
                l->push_back(this->stream_path(
                    buckets->at(i-1).c_str(),
                    sets->at(j-1).c_str(),
                    streams->at(k-1).c_str()
                ));
            }

            delete streams;
        }

        delete sets;
    }

    delete buckets;

    return l;
}

string Store::bucket_path(const char *bucket)
{
    string s = "/";
    s.append(bucket);
    return s;
}

string Store::bucket_directory(const char *bucket)
{
    string s;
    s.append(this->_path);
    s.append("/");
    s.append(bucket);
    return s;
}

string Store::stream_set_path(const char *bucket, const char *stream_set)
{
    string s = this->bucket_path(bucket);
    s.append("/");
    s.append(stream_set);
    return s;
}

string Store::stream_set_directory(const char *bucket, const char *stream_set)
{
    string s = this->bucket_directory(bucket);
    s.append("/");
    s.append(stream_set);
    return s;
}

string Store::stream_path(const char *bucket, const char *stream_set, const char *filename)
{
    string s = this->stream_set_path(bucket, stream_set);
    s.append("/");
    s.append(filename);

    return s;
}

string Store::path_to_filesystem_path(const char *path)
{
    string s = string(this->_path);
    s.append("/");
    s.append(path);

    return s;
}

protocol::StoreInfo Store::store_info()
{
    protocol::StoreInfo info = protocol::StoreInfo();

    vector<string> * buckets = this->buckets();

    for (size_t i = 0; i < buckets->size(); i++) {
        protocol::BucketInfo *bucket = info.add_bucket();
        bucket->set_path(buckets->at(i));

        vector<string> * sets = this->stream_sets_in_bucket(buckets->at(i).c_str());
        for (size_t j = 0; j < sets->size(); j++) {
            protocol::StreamSetInfo *ss = bucket->add_stream_set();
            ss->set_path(sets->at(j));

            vector<string> *streams = this->streams_in_stream_set(buckets->at(i).c_str(), sets->at(j).c_str());
            for (size_t k = 0; k < streams->size(); k++) {
                protocol::StreamInfo *stream = ss->add_stream();
                stream->set_path(streams->at(k));
            }

            delete streams;

        }

        delete sets;
    }

    delete buckets;

    return info;
}

vector<string> * Store::directories_in_directory(const char *dir)
{
    apr_status_t st;
    apr_dir_t *e;
    apr_finfo_t info;

    vector<string> *v = new vector<string>();

    st = apr_dir_open(&e, dir, this->_p);
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

vector<string> * Store::files_in_directory(const char *dir)
{
    apr_status_t st;
    apr_dir_t *e;
    apr_finfo_t info;

    vector<string> *v = new vector<string>();

    st = apr_dir_open(&e, dir, this->_p);
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