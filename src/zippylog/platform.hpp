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

#ifdef WINDOWS
#include <winsock2.h>
#include <Windows.h>
#endif

#include <string>
#include <vector>

namespace zippylog {

using ::std::string;
using ::std::vector;

// TODO move these to platform namespace
struct dir_entry {
    string name;
    uint64 size;
    char type;
};

typedef void * (* thread_start_func)(void *);

ZIPPYLOG_EXPORT void * create_thread(thread_start_func, void *data);

ZIPPYLOG_EXPORT bool join_thread(void *thread);

ZIPPYLOG_EXPORT bool terminate_thread(void *thread);

ZIPPYLOG_EXPORT bool directory_entries(const string dir, vector<dir_entry> &v);

ZIPPYLOG_EXPORT void windows_error(char *buffer, size_t buffer_size);

// new namespace where all APIs should be
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

    // joins two filesystem paths and returns the result
    string PathJoin(const string &a, const string &b);

    typedef struct UUID {
        unsigned char data[16];
    } UUID;

    // creates a new UUID
    // UUID type is not defined yet
    bool CreateUUID(UUID &u);

    class File {
    public:
        File();
#ifdef WINDOWS
        void * handle;
#endif
        int fd;

        bool open;
    };

    enum FileFlags {
        READ = 1,
        WRITE = 2,
        APPEND = 3,
        CREATE = 4,
        TRUNCATE = 5,
        BINARY = 6,
    };

    bool OpenFile(File &f, const string path, int flags);
    bool FileClose(File &f);
    bool FileWrite(File &f, const void *data, size_t length);

    bool FlushFile(File &f);

    // seek to the specified offset in the file
    bool FileSeek(File &f, int64 offset);

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

    // represents a change in a directory
    class DirectoryChange {
    public:
        DirectoryChange();

        // path that was changed
        // if a rename, this will be the new name
        string Path;

        // how the path changed
        enum Action {
            ADDED = 1,
            DELETED = 2,
            MODIFIED = 3,
            RENAMED = 4,
        } Action;

        // if path was renamed, this will be set to old name
        string OldName;
    };

    class DirectoryWatcher {
    public:
        DirectoryWatcher();
        DirectoryWatcher(const DirectoryWatcher &orig);
        DirectoryWatcher & operator=(const DirectoryWatcher &orig);
        ~DirectoryWatcher();

        // create a construct that watches the specified directory
        DirectoryWatcher(const string &directory, bool recurse=true);

        // Wait up to N microseconds for changes to the directory or forever,
        // if -1 is given as the timeout value
        //
        // Returns true if there are changes to the directory. Returns false
        // otherwise.
        //
        // If returns true, call GetChanges() to return the list of changes.
        // Function may return true immediately if changes are already
        // available but haven't been collected with GetChanges().
        bool WaitForChanges(int32 timeout);

        // returns collected changes to directory
        bool GetChanges(vector<DirectoryChange> &changes);

    protected:
        string path;
        bool recurse;
        vector<DirectoryChange> changes;
        bool started_waiting;

#ifdef WINDOWS
        HANDLE directory;
        HANDLE completion_port;
        BYTE results[32768];
        OVERLAPPED overlapped;
#endif
    };
}

} // namespace

#endif