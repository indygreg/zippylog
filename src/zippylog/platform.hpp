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
// in the ideal world, we would use an existing library, like APR or Boost. In
// the early days of this project, it was decided to keep the code as free
// from external dependencies as possible. As this namespace becomes larger
// and larger, we have to start questioning that decision.

#ifndef ZIPPYLOG_PLATFORM_HPP_
#define ZIPPYLOG_PLATFORM_HPP_

#include <zippylog/zippylog.h>

#ifdef WINDOWS
#include <winsock2.h>
#include <Windows.h>

#elif LINUX
// TODO should be pulled from build tools
#define HAVE_OPENDIR 1

#include <signal.h>
#include <time.h>
#endif

#include <map>
#include <string>
#include <vector>

namespace zippylog {

// TODO move these last remaining APIs to platform namespace
typedef void * (* thread_start_func)(void *);

void windows_error(char *buffer, size_t buffer_size);

// new namespace where all APIs should be
namespace platform {
    using ::std::map;
    using ::std::string;
    using ::std::vector;

    // records the last known system error
    void set_system_error();

    // attempts to retrieve the current system error
    // if the current system error is not defined, returns false
    // else, returns true and puts the error message in the string variable
    // will reset the current system error on call
    bool get_system_error(string &string);

    // sleeps the current thread for specified amount of milliseconds
    ZIPPYLOG_EXPORT void sleep(uint32 milliseconds);

    enum FileType {
        REGULAR = 1,
        DIRECTORY = 2,
        PIPE = 3,
        CHARACTER = 4,
        UNKNOWN = 5,
    };

    typedef struct DirectoryEntry {
        string name;
        uint64 size;
        enum FileType type;
    } DirectoryEntry;

    ZIPPYLOG_EXPORT bool DirectoryEntries(const string &dir, vector<DirectoryEntry> &v);
    ZIPPYLOG_EXPORT bool FilesInDirectory(const string &dir, vector<DirectoryEntry> &v);
    ZIPPYLOG_EXPORT bool FilesInDirectory(const string &dir, vector<string> &v);
    ZIPPYLOG_EXPORT bool DirectoriesInDirectory(const string &dir, vector<DirectoryEntry> &v);
    ZIPPYLOG_EXPORT bool DirectoriesInDirectory(const string &dir, vector<string> &v);

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

    // obtains a list of directories in a directory
    // recursively descends the path and finds all child directories
    ZIPPYLOG_EXPORT bool DirectoriesInTree(const string &path, vector<string> &paths);

    // joins two filesystem paths and returns the result
    ZIPPYLOG_EXPORT string PathJoin(const string &a, const string &b);

    typedef struct UUID {
        unsigned char data[16];
    } UUID;

    // creates a new UUID
    // UUID type is not defined yet
    ZIPPYLOG_EXPORT bool CreateUUID(UUID &u);

    class ZIPPYLOG_EXPORT File {
    public:
        File();

        // open a file at path with the FileFlags specified
        // returns whether file opened successfully
        bool Open(const string &path, int flags);

        // close the file
        bool Close();

        // write data to the file
        // returns whether all data was written
        bool Write(const void *data, size_t length);

        // flush contents to underlying store
        //
        // this clear all buffered data
        bool Flush();

        // seeks to the specified offset in the file
        bool Seek(int64 offset);

        // obtain a file descriptor for this file
        // returns 0 if file descriptor not available, file not open, etc
        int FileDescriptor();

    enum OpenFlags {
        READ     = 0x01,
        WRITE    = 0x02,
        APPEND   = 0x04,
        CREATE   = 0x08,
        TRUNCATE = 0x10,
        BINARY   = 0x20,
    };

    protected:
#ifdef WINDOWS
        void * handle;
#endif
        int fd;

        bool open;
    };

    // in the ideal world, timers would be represented as file descriptors and
    // we could include them in the zmq poll() event loop. this is easily done
    // on Linux. unfortunately, Windows requires a little bit more effort.
    // TODO expose timers as file descriptors someday
    class ZIPPYLOG_EXPORT Timer {
    public:
        // create a null timer. this does nothing and is present so some structs have
        // a default constructor
        Timer();
        ~Timer();

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
        //
        // if the parameter is non-zero, the timer will assume the interval
        // specified
        bool Start(uint32 microseconds = 0);

    protected:
        void Initialize();

        uint32 microseconds;
        bool signaled;
        bool running;
        bool initialized;

#ifdef WINDOWS
        void * handle;
#elif LINUX
        timer_t timer;
#endif

    };

    // represents a change in a directory
    class ZIPPYLOG_EXPORT DirectoryChange {
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

    class ZIPPYLOG_EXPORT DirectoryWatcher {
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

#ifdef WINDOWS
        HANDLE directory;
        HANDLE completion_port;
        BYTE results[32768];
        OVERLAPPED overlapped;
        bool started_waiting;
#elif LINUX
        // inotify descriptor
        int fd;

        // maps watch descriptors to directories
        map<int, string> directories;
#endif
    };

    class ZIPPYLOG_EXPORT Thread {
    public:
        Thread(thread_start_func, void *data);
        ~Thread();

        bool Join();
        bool Abort();

    protected:
#ifdef WINDOWS
        HANDLE thread;
#elif LINUX
        pthread_t thread;
#endif

    private:
        Thread(const Thread &orig);
        Thread & operator=(const Thread &orig);
    };
}

} // namespace

#endif
