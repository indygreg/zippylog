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

#include "pblog/pblog.h"

#include <cstddef>
#include <vector>
#include <apr_pools.h>

namespace pblog {

using ::std::vector;
using ::std::string;

class PBLOG_EXPORT Store {
    public:
        /** construct a store from a filesystem path */
        Store(const char *path);
        Store(const char *path, apr_pool_t *p);
        ~Store();

        /** return the path to this store */
        const char * path();

        /** obtain a list of the buckets in the store */
        vector<string> * buckets();

        /** list of stream sets in a bucket */
        vector<string> * stream_sets_in_bucket(const char *bucket);

        /** list of streams in stream set */
        vector<string> * streams_in_stream_set(const char *bucket, const char *stream_set);

        vector<string> * stream_paths();

        string bucket_path(const char *bucket);
        string bucket_directory(const char *bucket);
        string stream_set_path(const char *bucket, const char *stream_set);
        string stream_set_directory(const char *bucket, const char *stream_set);
        string stream_path(const char *bucket, const char *stream_set, const char *stream);
        string path_to_filesystem_path(const char *path);

    protected:
        vector<string> * directories_in_directory(const char *dir);
        vector<string> * files_in_directory(const char *dir);

    private:
        const char* _path;
        apr_pool_t* _p;
};

} // namespace pblog

#endif