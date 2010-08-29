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

// this file contains cross-platform APIs
// if this gets too large, we should consider using a framework, like APR

#ifndef PBLOG_PLATFORM_HPP_
#define PBLOG_PLATFORM_HPP_

#include <pblog/pblog.h>

#include <string>
#include <vector>

using ::std::string;
using ::std::vector;

namespace pblog {

struct dir_entry {
    string name;
    uint64 size;
    char type;
};

class PBLOG_EXPORT Platform {
    public:
        // TODO define a type for function pointer so compiler can save us
        static void * create_thread(void * func, void *data);
        static bool join_thread(void *thread);

        static bool directory_entries(const string dir, vector<dir_entry> &v);
};

} // namespace

#endif