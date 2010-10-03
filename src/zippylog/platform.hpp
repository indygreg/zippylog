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
// in the ideal world, we would use an existing library, like APR. In the early
// days of this project, it was decided to keep the code as free from external
// dependencies as possible. As this namespace becomes larger and larger, we
// have to start questioning that decision.

#ifndef ZIPPYLOG_PLATFORM_HPP_
#define ZIPPYLOG_PLATFORM_HPP_

#include <zippylog/zippylog.h>

#include <string>
#include <vector>

using ::std::string;
using ::std::vector;

namespace zippylog {

// TODO move these to platform namespace
struct dir_entry {
    string name;
    uint64 size;
    char type;
};

// TODO define a type for function pointer so compiler can save us
ZIPPYLOG_EXPORT void * create_thread(void * func, void *data);

ZIPPYLOG_EXPORT bool join_thread(void *thread);

ZIPPYLOG_EXPORT bool terminate_thread(void *thread);

ZIPPYLOG_EXPORT bool directory_entries(const string dir, vector<dir_entry> &v);

ZIPPYLOG_EXPORT void windows_error(char *buffer, size_t buffer_size);

namespace platform {
    enum FileType {
        REGULAR = 1,
        DIRECTORY = 2,
        PIPE = 3,
        CHARACTER = 4,
        UNKNOWN = 5,
    };

    typedef struct FileStat {
        FileType type;
        int64 size;
    } FileStat;

    ZIPPYLOG_EXPORT bool stat(const string path, FileStat &st);

    typedef struct Time {
        int32 year;
        int32 mon;
        int32 mday;
        int32 hour;
        int32 min;
        int32 sec;
        int32 usec;
        int32 wday;
        int32 yday;
        int32 isdst;
        int64 epoch_micro;
        int32 epoch_sec;
    } Time;

    ZIPPYLOG_EXPORT bool TimeNow(Time &t);

    // converts number of microseconds since UNIX epoch into a zippylog Time struct
    ZIPPYLOG_EXPORT bool UnixMicroTimeToZippyTime(int64 from, Time &to);

    ZIPPYLOG_EXPORT bool MakeDirectory(const string path);

    ZIPPYLOG_EXPORT bool PathIsDirectory(const string path);

    typedef struct UUID {
        unsigned char data[16];
    } UUID;

    // creates a new UUID
    // UUID type is not defined yet
    bool CreateUUID(UUID &u);

    typedef struct File {
#ifdef WINDOWS
        void * handle;
#endif
        int fd;
    } File;

    enum FileFlags {
        READ = 1,
        WRITE = 2,
        APPEND = 3,
        CREATE = 4,
        TRUNCATE = 5,
        BINARY = 6,
    };

    bool OpenFile(File &f, const string path, int flags);

    // in the ideal world, timers would be represented as file descriptors and
    // we could include them in the zmq poll() event loop. this is easily done
    // on Linux. unfortunately, Windows requires a little bit more effort.
    // TODO expose timers as file descriptors someday
    class ZIPPYLOG_EXPORT Timer {
    public:
        // create a null timer. this does nothing and is present so some structs have
        // a default constructor
        Timer();

        // create a new timer that fires N microseconds from now
        Timer(uint32 microseconds);

        // whether the timer has signaled yet
        bool Signaled();

        // reset the timer
        // this will unarm the timer and prepare it to be executed again
        // this is called automatically by Start() if restarting an existing timer
        // it is more of an internal API but exposed in case it is needed
        bool Reset();

        // starts the timer
        bool Start();

    protected:
        uint32 microseconds;
        bool signaled;
        bool running;

#ifdef WINDOWS
        void * handle;
#endif
    };
}

} // namespace

#endif