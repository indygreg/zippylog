//  Copyright 2011 Gregory Szorc
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

#include <zippylog/zippylog.hpp>

#ifdef WINDOWS
#include <winsock2.h>
#include <Windows.h>

#elif LINUX
// @todo should be pulled from build tools
#define HAVE_OPENDIR 1

#include <signal.h>
#include <time.h>
#endif

#include <map>
#include <string>
#include <vector>

namespace zippylog {

// @todo move these last remaining APIs to platform namespace
typedef void * (* thread_start_func)(void *);

void windows_error(char *buffer, size_t buffer_size);

// new namespace where all APIs should be
namespace platform {
    // records the last known system error
    void set_system_error();

    // attempts to retrieve the current system error
    // if the current system error is not defined, returns false
    // else, returns true and puts the error message in the string variable
    // will reset the current system error on call
    bool get_system_error(::std::string &string);

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
        ::std::string name;
        uint64 size;
        enum FileType type;
    } DirectoryEntry;

    ZIPPYLOG_EXPORT bool DirectoryEntries(const ::std::string &dir, ::std::vector<DirectoryEntry> &v);
    ZIPPYLOG_EXPORT bool FilesInDirectory(const ::std::string &dir, ::std::vector<DirectoryEntry> &v);
    ZIPPYLOG_EXPORT bool FilesInDirectory(const ::std::string &dir, ::std::vector< ::std::string > &v);
    ZIPPYLOG_EXPORT bool DirectoriesInDirectory(const ::std::string &dir, ::std::vector<DirectoryEntry> &v);
    ZIPPYLOG_EXPORT bool DirectoriesInDirectory(const ::std::string &dir, ::std::vector< ::std::string > &v);

    typedef struct FileStat {
        FileType type;
        int64 size;
    } FileStat;

    ZIPPYLOG_EXPORT bool stat(const ::std::string path, FileStat &st);

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

    ZIPPYLOG_EXPORT bool MakeDirectory(const ::std::string path);

    ZIPPYLOG_EXPORT bool PathIsDirectory(const ::std::string path);

    ZIPPYLOG_EXPORT bool PathIsRegularFile(const ::std::string &path);

    // obtains a list of directories in a directory
    // recursively descends the path and finds all child directories
    ZIPPYLOG_EXPORT bool DirectoriesInTree(const ::std::string &path, ::std::vector< ::std::string > &paths);

    /// Recursively remove a specified directory
    ZIPPYLOG_EXPORT bool RemoveDirectory(const ::std::string &path);

    // joins two filesystem paths and returns the result
    ZIPPYLOG_EXPORT ::std::string PathJoin(const ::std::string &a, const ::std::string &b);

    typedef struct UUID {
        unsigned char data[16];
    } UUID;

    // creates a new UUID
    ZIPPYLOG_EXPORT bool CreateUUID(UUID &u);

    /// Creates a new UUID and returns the result in a string
    ///
    /// Throws an exception if a UUID could not be created.
    ///
    /// @param format If true, string is a formatted UUID, else it is the raw
    /// 16 bytes
    ZIPPYLOG_EXPORT ::std::string CreateUUID(bool format = false);

    /// Formats the passed UUID data into a string
    ///
    /// String has the common XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX format
    ZIPPYLOG_EXPORT bool FormatUUID(UUID &e, ::std::string &s);

    class ZIPPYLOG_EXPORT File {
    public:
        File();

        // open a file at path with the FileFlags specified
        // returns whether file opened successfully
        bool Open(const ::std::string &path, int flags);

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

        /// Obtain an exclusive write lock on the entire file
        ///
        /// Returns true only if the write lock could be obtained
        bool WriteLockEntire();

    enum OpenFlags {
        /// Open file for reading
        READ       = 0x01,

        /// Open file for writing
        WRITE      = 0x02,

        /// Open file in append writing mode. Writes will always be
        /// appended to the file
        APPEND     = 0x04,

        /// Create the file if it doesn't exist
        CREATE     = 0x08,

        /// Truncate existing files on open
        TRUNCATE   = 0x10,

        /// Open the file in binary mode
        BINARY     = 0x20,

        /// Obtain an exclusive write lock on the file
        ///
        /// If the write lock cannot be obtained, the open will fail
        WRITE_LOCK = 0x40,
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
    // @todo expose timers as file descriptors someday
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
        ::std::string Path;

        // how the path changed
        enum Action {
            ADDED = 1,
            DELETED = 2,
            MODIFIED = 3,
            RENAMED = 4,
        } Action;

        // if path was renamed, this will be set to old name
        ::std::string OldName;
    };

    class ZIPPYLOG_EXPORT DirectoryWatcher {
    public:
        ~DirectoryWatcher();

        // create a construct that watches the specified directory
        DirectoryWatcher(const ::std::string &directory, bool recurse=true);

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
        bool GetChanges(::std::vector<DirectoryChange> &changes);

    protected:
        ::std::string path;
        bool recurse;
        ::std::vector<DirectoryChange> changes;

    private:
        DirectoryWatcher();
        DirectoryWatcher(const DirectoryWatcher &orig);
        DirectoryWatcher & operator=(const DirectoryWatcher &orig);

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
        ::std::map<int, ::std::string> directories;
#endif
    };

    class ZIPPYLOG_EXPORT Thread {
    public:
        Thread(thread_start_func, void *data);
        ~Thread();

        bool Join();
        bool Abort();

        /// Returns whether the thread is alive (running)
        ///
        /// This function was added as a stop-gap until a more robust could be
        /// devised. Please don't use it. The implementation sucks.
        bool Alive();

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

    /// A conditional variable that can be waited upon for completion
    ///
    /// Each variable can be waited upon by multiple simultaneous consumers.
    /// If Signal() is called and multiple threads are stuck in Wait(), then
    /// they will all be notified. It is possible for a thread to be in Wait()
    /// but not receive the signal if it has just entered the function but
    /// hasn't registered with the OS yet.
    ///
    /// Once signaled, these variables stay signaled until Reset() is called.
    /// This allows them to function as flags.
    ///
    /// ConditionalWait variables are guaranteed to only work within the same
    /// process. If they are used among multiple processes, results are
    /// undefined (possibly explosions).
    class ConditionalWait {
        public:
            /// Create a new conditional wait variable in the unsignaled state
            ConditionalWait();

            ~ConditionalWait();

            /// Signals the condition variable
            ///
            /// Once this has been called, the variable is marked as signaled
            /// and all calls to Wait() immediately return true until Reset()
            /// is called.
            bool Signal();

            /// Resets the condition variable
            ///
            /// Clears the signaled flag of the variable.
            bool Reset();

            /// Waits for the variable to be signaled
            ///
            /// The parameter is the timeout at which to give up waiting. If -1,
            /// will wait forever. If 0, will test the condition and move on
            /// if signaled. Value is in microseconds from call time.
            bool Wait(int32 timeout_microseconds);

    protected:
        /// whether the condition variable is signaled
        bool signaled;
#ifdef POSIX
        pthread_cond_t cond;
        pthread_mutex_t mutex;
#elif WINDOWS
        HANDLE cond;
        HANDLE mutex;
#endif
    };
}

} // namespace

#endif
