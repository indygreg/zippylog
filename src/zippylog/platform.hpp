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

#elif POSIX
#include <signal.h>
#include <time.h>
#endif

#ifdef MACOS
#include <CoreServices/CoreServices.h>

// this is imported in CoreServices land and conflicts with protocol buffer
// land. it doesn't seem to break anything, so we undef it
#undef TYPE_BOOL
#endif

#include <map>
#include <string>
#include <vector>

namespace zippylog {

// @todo move these last remaining APIs to platform namespace
typedef void * (* thread_start_func)(void *);

void windows_error(char *buffer, size_t buffer_size);

/// Contains OS compatibility classes and routines.
namespace platform {
    /// Record the last known system error
    void set_system_error();

    /// Retrieve the current system error
    ///
    /// If the current system error is not defined, returns false.
    /// Else, returns true and puts the error message in the string variable.
    /// Will reset the current system error on call.
    ///
    /// @param string Last system error string
    /// @return Whether a system error was defined.
    bool get_system_error(::std::string &string);

    /// Sleeps the current thread for specified amount of milliseconds
    ///
    /// @param milliseconds How long to sleep, in milliseconds
    ZIPPYLOG_EXPORT void sleep(uint32 milliseconds);

    /// Defines the type of file
    enum FileType {
        /// A regular file
        REGULAR = 1,

        /// A directory
        DIRECTORY = 2,

        /// A pipe
        PIPE = 3,

        /// A character device
        CHARACTER = 4,

        /// Other/unknown
        UNKNOWN = 5,
    };

    /// Describes an entry in a directory
    typedef struct DirectoryEntry {
        /// The file name
        ::std::string name;

        /// The size of the file, in bytes
        uint64 size;

        /// The type of the entry
        enum FileType type;
    } DirectoryEntry;

    /// Obtain a list of entries in the directory specified
    ///
    /// @return bool Whether the operation completed successfully
    /// @param dir Path to directory to retrieve
    /// @param v Holds the results of the operation
    ZIPPYLOG_EXPORT bool DirectoryEntries(::std::string const &dir,
                                          ::std::vector<DirectoryEntry> &v);

    /// Obtain information about files in a directory
    ///
    /// This can be tought of as a filter for DirectoryEntries() which only
    /// retrieves info on files.
    ///
    /// @return bool Whether the operation completed successfully
    /// @param dir Path to directory to retrieve
    /// @param v Holds the result of the operation
    ZIPPYLOG_EXPORT bool FilesInDirectory(::std::string const &dir,
                                          ::std::vector<DirectoryEntry> &v);

    /// Obtain names of files in a directory
    ///
    /// This is the same as the other FilesInDirectory() except it stores
    /// strings instead of rich objects.
    ///
    /// @return bool Whether the operation completed successfully
    /// @param dir Path to directory to retrieve
    /// @param v Holds the results of the operation
    ZIPPYLOG_EXPORT bool FilesInDirectory(::std::string const &dir,
                                          ::std::vector< ::std::string > &v);

    /// Obtains information about directories in a directory
    ///
    /// @return bool Whether the operation completed successfully
    /// @param dir Path of directory to examine
    /// @param v Holds the results of the operation
    ZIPPYLOG_EXPORT bool DirectoriesInDirectory(::std::string const &dir,
                                                ::std::vector<DirectoryEntry> &v);

    /// Obtains names of directories inside a directory
    ///
    /// @return bool Whether the operation completed successfully
    /// @param dir Path of directory to examine
    /// @param v Holds the results of the operation
    ZIPPYLOG_EXPORT bool DirectoriesInDirectory(::std::string const &dir,
                                                ::std::vector< ::std::string > &v);

    /// Describes a file
    typedef struct FileStat {
        /// The type of this file
        FileType type;

        /// Size of the file, in bytes
        int64 size;
    } FileStat;

    /// Stat a given file path
    ///
    /// @return bool Whether stat completed without error
    /// @param path The path to examine
    /// @param st Result of operation
    ZIPPYLOG_EXPORT bool stat(const ::std::string path, FileStat &st);

    /// Represents a Gregorian time
    typedef struct Time {
        /// Year (0 index)
        int32 year;

        /// Month of year. January is 1
        int32 mon;

        /// Day of month. 1st day is 1
        int32 mday;

        /// Hour of day. Midnight is 0
        int32 hour;

        /// Minute part of time
        int32 min;

        /// Second part of time
        int32 sec;

        /// Partial seconds, in microseconds
        ///
        /// Value should be 0 <= value < 1000000
        int32 usec;

        /// Day of week. 1 is Sunday
        int32 wday;

        /// Day of year. January 1 is 1
        int32 yday;

        /// Whether the time is in daylight savings
        int32 isdst;

        /// Number of microseconds since UNIX epoch
        int64 epoch_micro;

        /// Number of seconds since UNIX epoch
        int32 epoch_sec;
    } Time;

    ZIPPYLOG_EXPORT bool TimeNow(Time &t);

    /// Convert the number of microseconds since UNIX epoch to zippylog time
    ///
    /// @param from number of microseconds since UNIX epoch
    /// @param to holds the results of the operation
    /// @return bool Whether the operation completed without error
    ZIPPYLOG_EXPORT bool UnixMicroTimeToZippyTime(int64 from, Time &to);

    /// Create a directory at the specified path
    ///
    /// @param path Path of directory to create
    /// @return bool Whether the directory was created successfully
    ZIPPYLOG_EXPORT bool MakeDirectory(const ::std::string path);

    /// Test whether a path is a directory
    ///
    /// @param path Path to test
    /// @return bool Whether path exists and is a directory
    ZIPPYLOG_EXPORT bool PathIsDirectory(const ::std::string path);

    /// Test whether a path is a file
    ///
    /// @param path Path to test
    /// @return bool Whether path exists and is a regular file
    ZIPPYLOG_EXPORT bool PathIsRegularFile(::std::string const &path);

    /// Recursively find all directories in a given directory
    ///
    /// Upon successful completion, the passed vector will be filled with path
    /// of all child directories. Paths will be relative to path specified.
    ///
    /// @param path Path to traverse
    /// @param paths Holds results of operation
    /// @param bool Whether operation executed without error
    ZIPPYLOG_EXPORT bool DirectoriesInTree(::std::string const &path,
                                           ::std::vector< ::std::string > &paths);

    /// Recursively remove a specified directory
    ///
    /// @param path Path to directory to remove
    /// @return bool Whether the directory and all its descendents were removed
    ZIPPYLOG_EXPORT bool RemoveDirectory(::std::string const &path);

    /// Joints two filesystem paths
    ///
    /// @param a first path compoent
    /// @param b second path component
    /// @return string Joined path
    ZIPPYLOG_EXPORT ::std::string PathJoin(::std::string const &a, ::std::string const &b);

    /// Represents a UUID
    typedef struct UUID {
        /// Raw UUID byte data
        unsigned char data[16];
    } UUID;

    /// Generate a new UUID
    ///
    /// @param u Record to hold new UUID
    /// @return bool Whether new UUID was created successfully
    ZIPPYLOG_EXPORT bool CreateUUID(UUID &u);

    /// Creates a new UUID and returns the result in a string
    ///
    /// Throws an exception if a UUID could not be created.
    ///
    /// @param format If true, string is a formatted UUID, else it is the raw
    /// 16 bytes
    /// @return string Result of operation
    ZIPPYLOG_EXPORT ::std::string CreateUUID(bool format = false);

    /// Formats the passed UUID data into a string
    ///
    /// String has the common XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX format
    ///
    /// @param e UUID to format
    /// @param s String to hold formatted UUID
    /// @return Whether operation completed without error
    ZIPPYLOG_EXPORT bool FormatUUID(UUID &e, ::std::string &s);

    /// Represents a hild handle
    class ZIPPYLOG_EXPORT File {
    public:
        /// Construct a new file handle that points to nothing
        File();

        /// Open a file at path with the FileFlags specified
        ///
        /// @param path Path to file to open
        /// @param flags File open flags
        /// @return Whether the file was opened successfully
        bool Open(::std::string const &path, int flags);

        /// Close the file
        ///
        /// @return Whether the file was closed without issue
        bool Close();

        // Write data to the file
        ///
        /// @param data Pointer to data being written
        /// @param length How much data from source buffer to write
        /// @return Whether all requested data was written
        bool Write(const void *data, size_t length);

        /// Flush contents to underlying store
        //
        /// @return Whether the flush executed successfully
        bool Flush();

        /// Seeks to the specified offset in the file
        ///
        /// @param offset Absolute offset to seek to
        /// @return Whether seek completed successfully
        bool Seek(int64 offset);

        /// Obtain a file descriptor for this file
        ///
        /// @return non-zero and non-negative filev value on success
        int FileDescriptor();

        /// Obtain an exclusive write lock on the entire file
        ///
        /// Returns true only if the write lock could be obtained
        bool WriteLockEntire();

    /// Bitflags to specify how to open a file
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

    /// Represents a countdown timer
    ///
    /// Countdown timers start at a specified value and mark a flag when that
    /// wall time has elapsed.
    ///
    /// @todo expose timers as file descriptors someday
    class ZIPPYLOG_EXPORT Timer {
    public:
        // create a null timer. this does nothing and is present so some structs have
        // a default constructor
        Timer();
        ~Timer();

        /// Create a new timer that fires at some time in the future
        ///
        /// @param microseconds Number of microseconds in future to fire at
        Timer(uint32 microseconds);

        /// Whether the timer has signaled yet
        ///
        /// @return true if fired or false otherwise
        bool Signaled();

        /// Reset the timer
        ///
        /// This will unarm the timer and prepare it to be executed again.
        /// This is called automatically by Start() if restarting an existing timer.
        /// It is more of an internal API but exposed in case it is needed.
        ///
        /// @return Whether the operation completed successfully
        bool Reset();

        /// Starts the timer
        ///
        /// If the parameter is non-zero, the timer will assume the interval
        /// specified. Otherwise, it will use the last-used interval.
        ///
        /// @param microseconds When the timer should alarm, in microseconds
        /// @return Whether the timer was armed successfully
        bool Start(uint32 microseconds = 0);

    protected:
        /// Initialize the internal timer state
        void Initialize();

        /// Configured alarm interval, in microseconds
        uint32 microseconds;

        /// Whether the timer has alarmed
        bool signaled;

        /// Whether the timer is currently armed
        bool running;

        /// Whether the timer state is initialized
        bool initialized;

#ifdef WINDOWS
        /// Holds timer structure
        void * handle;
#elif LINUX
        /// Holds timer structure
        timer_t timer;
#elif MACOS
        /// Time we armed the timer
        uint64 time_start;
#endif

    };

    /// Represents a change in a directory
    class ZIPPYLOG_EXPORT DirectoryChange {
    public:
        /// Construct an empty representation
        DirectoryChange();

        /// Path that was changed
        ///
        /// If a rename, this will be the new name.
        ::std::string Path;

        /// How the path changed
        enum Action {
            ADDED = 1,
            DELETED = 2,
            MODIFIED = 3,
            RENAMED = 4,
        } Action;

        /// If path was renamed, this will be set to old name.
        ::std::string OldName;
    };

    /// Entity that watches a directory(tree) for changes
    ///
    /// Directory watchers register with the operating system to receive
    /// notifications when a directory changes.
    class ZIPPYLOG_EXPORT DirectoryWatcher {
    public:
        ~DirectoryWatcher();

        /// Create a directory watcher that watches a specific path
        ///
        /// Directory watchers can watch either single directories or whole
        /// directory trees. If recursively watching a tree, the watcher will
        /// automatically watch newly-created directories, as they are
        /// added.
        ///
        /// @param directory Path of directory to watch
        /// @param recurse Whether to watch all child directories
        DirectoryWatcher(::std::string const &directory, bool recurse=true);

        /// Wait up to N microseconds for changes to the directory or forever,
        /// if -1 is given as the timeout value
        ///
        /// Returns true if there are changes to the directory. Returns false
        /// otherwise.
        ///
        /// If returns true, call GetChanges() to return the list of changes.
        /// Function may return true immediately if changes are already
        /// available but haven't been collected with GetChanges().
        ///
        /// @param timeout How long to wait for a change, in microseconds
        /// @return Whether there were changes
        bool WaitForChanges(int32 timeout);

        /// Returns collected changes to directory
        ///
        /// @param changes Container for results
        /// @return Whether there is data in the container
        bool GetChanges(::std::vector<DirectoryChange> &changes);

#ifdef MACOS
        static void EventStreamCallback(ConstFSEventStreamRef stream, void *data, size_t numEvents, void *eventPaths,
                                        const FSEventStreamEventFlags eventFlags[], const FSEventStreamEventId eventIds[]);
#endif

    protected:
        ::std::string path;
        bool recurse;
        ::std::vector<DirectoryChange> changes;
#ifdef WINDOWS
        void StartWatching();
#endif

    private:
        DirectoryWatcher();
        DirectoryWatcher(DirectoryWatcher const &orig);
        DirectoryWatcher & operator=(DirectoryWatcher const &orig);

#ifdef WINDOWS
        HANDLE directory;
        HANDLE completion_port;
        BYTE results[32768];
        OVERLAPPED overlapped;
#elif HAVE_INOTIFY
        // inotify descriptor
        int fd;

        // maps watch descriptors to directories
        ::std::map<int, ::std::string> directories;
#elif MACOS
        FSEventStreamRef stream;
        FSEventStreamContext context;
        CFRunLoopRef loop;
#endif
    };

    /// Representation of an operating system thread
    class ZIPPYLOG_EXPORT Thread {
    public:
        /// Construct a new thread that executes the specified function
        ///
        /// @param f Function to execute when thread starts
        /// @param data Arbitrary data to be passed to thread start function
        Thread(thread_start_func f, void *data);
        ~Thread();

        /// Wait for the thread to finish execution
        bool Join();

        /// Forcefully abort the thread
        bool Abort();

        /// Returns whether the thread is alive (running)
        ///
        /// This function was added as a stop-gap until a more robust could be
        /// devised. Please don't use it. The implementation sucks.
        bool Alive();

    protected:
#ifdef WINDOWS
        HANDLE thread;
#elif HAVE_PTHREAD
        pthread_t thread;
#endif

    private:
        Thread(Thread const &orig);
        Thread & operator=(Thread const &orig);
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
    class ZIPPYLOG_EXPORT ConditionalWait {
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
