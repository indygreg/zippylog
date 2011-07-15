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

#include <zippylog/platform.hpp>

#ifdef WINDOWS
#include <direct.h>
#include <tchar.h>
#include <io.h>
#endif

#ifdef LINUX
#include <assert.h>
#include <pthread.h>
#include <stdlib.h>
#include <dirent.h>
#include <fcntl.h>
#include <errno.h>
#include <poll.h>
#include <string.h>
#include <sys/io.h>
#include <sys/inotify.h>
#include <sys/time.h>
#include <uuid/uuid.h>
#endif

#include <fcntl.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>

using ::std::invalid_argument;
using ::std::logic_error;
using ::std::map;
using ::std::string;
using ::std::vector;

namespace zippylog {

namespace platform {

#ifdef LINUX
static __thread int system_error = 0;
#endif

void set_system_error()
{
#ifdef LINUX
    system_error = errno;
#elif WINDOWS

#else
#error "set_system_error() not implemented on this platform"
#endif
}

bool get_system_error(string &s)
{
#ifdef LINUX
    if (!system_error) return false;

    char * error = strerror(system_error);
    s.clear();
    s.append(error);

    system_error = 0;

    return true;

#elif WINDOWS
    DWORD errcode = GetLastError();
    char error[256];
    DWORD rc = FormatMessageA (FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS, NULL, errcode, MAKELANGID(LANG_NEUTRAL,
        SUBLANG_DEFAULT), &error[0], sizeof(error), NULL );

    s.assign(&error[0], sizeof(error));
    return true;
#else
#error "get_system_error not implemented on this platform"
#endif
}

// congratulations, this is the 4,234,532,657 time in programming history this
// function has been written!
bool DirectoryEntries(string const &dir, vector<DirectoryEntry> &v)
{
    //complicated case first
#ifdef WINDOWS
    // @todo fix potential buffer overrun
    char path[8192];
    strcpy_s(path, sizeof(path), dir.c_str());

    // we need to wildcard the path, cuz that's how Windows works
    char * end = strrchr(path, '\0');
    end[0] = '\\';
    end[1] = '*';
    end[2] = '\0';

    WIN32_FIND_DATA info;
    HANDLE handle = FindFirstFile(path, &info);
    if (INVALID_HANDLE_VALUE == handle) {
        return false;
    }

    do {
        DirectoryEntry entry;
        entry.name = info.cFileName;
        entry.size = (info.nFileSizeHigh * (MAXDWORD+1)) + info.nFileSizeLow;
        if (info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            entry.type = ::zippylog::platform::DIRECTORY;
        }
        else if (info.dwFileAttributes & FILE_ATTRIBUTE_NORMAL || info.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE) {
            entry.type = ::zippylog::platform::REGULAR;
        }
        else {
            entry.type = ::zippylog::platform::UNKNOWN;
        }

        v.push_back(entry);

    } while(FindNextFile(handle, &info) != 0);

    FindClose(handle);
    return true;

#elif HAVE_OPENDIR
    DIR * dh = opendir(dir.c_str());
    if (!dh) return false;

    dirent *d = NULL;

    while ((d = readdir(dh))) {
        DirectoryEntry entry;

        entry.name = d->d_name;

        switch (d->d_type) {
            case DT_DIR:
                entry.type = DIRECTORY;
                break;
            case DT_REG:
                entry.type = REGULAR;
                break;
            default:
                entry.type = UNKNOWN;
                break;
        }

        v.push_back(entry);
    }

    closedir(dh);

    return true;
#else
#error "directory traversal not implemented on this platform yet"
#endif

    return false;
}

void sleep(uint32 milliseconds)
{
#ifdef WINDOWS
    Sleep(milliseconds);
#elif LINUX
    struct timespec tv;
    tv.tv_sec = milliseconds / 1000;
    tv.tv_nsec = (milliseconds * 1000000) % 1000000000;

    nanosleep(&tv, NULL);
#else
#error "sleep() is not implemented on this platform"
#endif
}

bool stat(const string path, FileStat &st)
{
#ifdef WINDOWS
    struct _stat64 result;

    if (_stat64(path.c_str(), &result) != 0) {
        return false;
    }

    if (result.st_mode & _S_IFDIR) {
        st.type = DIRECTORY;
    }
    else if (result.st_mode & _S_IFREG) {
        st.type = REGULAR;
    }
    else if (result.st_mode & _S_IFIFO) {
        st.type = PIPE;
    }
    else {
        st.type = UNKNOWN;
    }

#elif LINUX
    struct stat result;

    if (stat(path.c_str(), &result) != 0) {
        set_system_error();
        return false;
    }

    if (result.st_mode & S_IFDIR) {
        st.type = DIRECTORY;
    }
    else if (result.st_mode & S_IFREG) {
        st.type = REGULAR;
    }
    else if (result.st_mode & S_IFIFO) {
        st.type = PIPE;
    }
    else {
        st.type = UNKNOWN;
    }
#endif

    st.size = result.st_size;

    return true;
}

bool TimeNow(Time &t)
{
#ifdef WINDOWS
    // the default resolution of the time on Windows is pretty crappy
    // it might be about 1 millisecond
    // the internets indicate that synchronizing the time to
    // QueryPerformanceCounters() is a popular approach to increase the
    // resolution.
    // @todo make windows time resolution better
    FILETIME time;
    GetSystemTimeAsFileTime(&time);
    ULARGE_INTEGER wintime;
    wintime.HighPart = time.dwHighDateTime;
    wintime.LowPart = time.dwLowDateTime;

    // Windows time is in 100 nanosecond increments
    // convert to microseconds and subtract offset
    t.epoch_micro = wintime.QuadPart / 10 - 11644473600000000;

    // @todo is this compiler hack acceptable?
    t.epoch_sec = (int32)(t.epoch_micro / 1000000);
    t.usec = t.epoch_micro % 1000000;
#elif LINUX
    struct timeval tv;
    if (gettimeofday(&tv, NULL) != 0) return false;

    t.epoch_micro = tv.tv_sec * 1000000 + tv.tv_usec;
    t.epoch_sec = tv.tv_sec;
    t.usec = tv.tv_usec;
#else
#error "not supported on this platform yet"
#endif

    time_t tt = t.epoch_sec;
    tm *gtm = gmtime(&tt);
    t.year = gtm->tm_year + 1900;
    t.mon = gtm->tm_mon + 1;
    t.mday = gtm->tm_mday;
    t.hour = gtm->tm_hour;
    t.min = gtm->tm_min;
    t.sec = gtm->tm_sec;
    t.isdst = gtm->tm_isdst;
    t.wday = gtm->tm_wday;
    t.yday = gtm->tm_yday;

    return true;
}

bool UnixMicroTimeToZippyTime(int64 from, Time &to)
{
    to.epoch_micro = from;
    to.epoch_sec = (int32)(from / 1000000);
    to.usec = from % 1000000;

    time_t tt = to.epoch_sec;
    tm *gtm = gmtime(&tt);
    to.year = gtm->tm_year + 1900;
    to.mon = gtm->tm_mon + 1;
    to.mday = gtm->tm_mday;
    to.hour = gtm->tm_hour;
    to.min = gtm->tm_min;
    to.sec = gtm->tm_sec;
    to.isdst = gtm->tm_isdst;
    to.wday = gtm->tm_wday;
    to.yday = gtm->tm_yday;

    return true;
}

bool MakeDirectory(const string path)
{
    // @todo permissions should be passed as argument
#ifdef WINDOWS
    // @todo use native Windows API
    int result = mkdir(path.c_str());
#elif LINUX
    int result = mkdir(path.c_str(), 0775);
    if (result == -1) {
        set_system_error();
        return false;
    }
#else
#error "Not supported on this platform yet"
#endif

    return result == 0;
}

bool PathIsDirectory(const string path)
{
    FileStat st;
    int result = stat(path, st);
    if (result == -1) {
        set_system_error();
        return false;
    }

    return st.type == DIRECTORY;
}

bool PathIsRegularFile(string const &path)
{
    FileStat st;
    int result = stat(path, st);
    if (result == -1) {
        set_system_error();
        return false;
    }

    return st.type == REGULAR;
}

bool DirectoriesInDirectory(string const &dir, vector<DirectoryEntry> &v)
{
    v.clear();
    vector<DirectoryEntry> entries;
    if (!DirectoryEntries(dir, entries)) return false;

    vector<DirectoryEntry>::iterator i = entries.begin();
    for (; i != entries.end(); i++) {
        if (i->type == DIRECTORY && i->name[0] != '.') {
            v.push_back(*i);
        }
    }

    return true;
}

bool DirectoriesInDirectory(string const &dir, vector<string> &v)
{
    vector<DirectoryEntry> entries;
    if (!platform::DirectoriesInDirectory(dir, entries)) return false;

    v.clear();
    vector<DirectoryEntry>::iterator i = entries.begin();
    for (; i != entries.end(); i++) {
        v.push_back(i->name);
    }

    return true;
}

bool FilesInDirectory(string const &dir, vector<DirectoryEntry> &v)
{
    v.clear();
    vector<DirectoryEntry> entries;
    if (!DirectoryEntries(dir, entries)) return false;

    vector<DirectoryEntry>::iterator i = entries.begin();
    for (; i != entries.end(); i++) {
        if (i->type == REGULAR) {
            v.push_back(*i);
        }
    }

    return true;
}

bool FilesInDirectory(string const &dir, vector<string> &v)
{
    v.clear();
    vector<DirectoryEntry> entries;
    if (!FilesInDirectory(dir, entries)) return false;

    vector<DirectoryEntry>::iterator i = entries.begin();
    for (; i != entries.end(); i++) {
        v.push_back(i->name);
    }

    return true;
}

bool DirectoriesInTree(string const &path, vector<string> &paths)
{
    vector<DirectoryEntry> entries;

    if (!DirectoryEntries(path, entries)) {
        return false;
    }

    vector<DirectoryEntry>::iterator i = entries.begin();
    for (; i != entries.end(); i++) {
        if (i->type != DIRECTORY) continue;

        if (i->name[0] == '.') continue;

        string full_path = PathJoin(path, i->name);

        paths.push_back(full_path);

        DirectoriesInTree(full_path, paths);
    }

    return true;

}

bool RemoveDirectory(string const &path)
{
    if (!PathIsDirectory(path)) {
        return false;
    }

    vector<DirectoryEntry> entries;
    if (!DirectoryEntries(path, entries)) {
        return false;
    }

    vector<DirectoryEntry>::iterator i = entries.begin();
    for(; i != entries.end(); i++) {
        if (i->name[0] == '.') continue;

        string full_path = PathJoin(path, i->name);

        if (i->type == DIRECTORY) {
            if (!RemoveDirectory(full_path)) {
                return true;
            }
        }
        else if (i->type == REGULAR) {
            unlink(full_path.c_str());
        }
    }

    rmdir(path.c_str());

    return true;
}

File::File()
{
    this->open = false;
#ifdef WINDOWS
    this->handle = NULL;
#endif

    this->fd = 0;
}

bool File::Open(string const &path, int flags)
{
#ifdef WINDOWS
    DWORD access = 0;
    DWORD share = 0;
    DWORD creation = OPEN_EXISTING;
    DWORD attributes = 0;
    int fdflags = 0;

    if (flags & READ) {
        access |= GENERIC_READ;
        share |= FILE_SHARE_READ;
    }

    if (flags & WRITE) {
        // allow write sharing unless we want a lock
        if (!(flags & WRITE_LOCK)) {
            share |= FILE_SHARE_WRITE;
        }

        access |= GENERIC_WRITE;
    }

    if (flags & APPEND) {
        access |= FILE_APPEND_DATA;
        fdflags |= _O_APPEND;
        share |= FILE_SHARE_WRITE;
    }

    if (flags & CREATE) {
        creation = OPEN_ALWAYS;
    }
    else if (flags & TRUNCATE) {
        creation = CREATE_ALWAYS;
    }

    // @todo probably an issue w/ ANSI vs Unicode functions for file name
    HANDLE h = CreateFile(path.c_str(), access, share, NULL, creation, attributes, NULL);

    if (h == INVALID_HANDLE_VALUE) return false;

    this->handle = h;
    this->fd = _open_osfhandle((long)h, fdflags);
    this->open = true;

    return true;

#elif LINUX
    int open_flags = 0;
    mode_t mode = 0;

    // start with mutually exclusively file access rules
    if (flags & READ && flags & WRITE) {
        open_flags |= O_RDWR;
    }
    else if (flags & READ) {
        open_flags |= O_RDONLY;
    }
    else if (flags & WRITE) {
        open_flags |= O_WRONLY;
    }
    else {
        throw invalid_argument("must have one of READ or WRITE flag set");
    }

    if (flags & APPEND) open_flags |= O_APPEND;
    if (flags & CREATE) {
        open_flags |= O_CREAT;
        mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP;
    }

    if (flags & TRUNCATE) open_flags |= O_TRUNC;
    // BINARY has no meaning on Linux

    this->fd = ::open(path.c_str(), open_flags, mode);
    if (this->fd == -1) {
        set_system_error();
        return false;
    }

    this->open = true;

    if (flags & WRITE_LOCK) {
        if (!this->WriteLockEntire()) {
            this->open = false;
            ::close(this->fd);
            return false;
        }
    }

    return true;
#else
#error "File::Open() not supported on this platform"
#endif
    return false;
}

bool File::Close()
{
#ifdef WINDOWS
    if (!this->open) return true;
    return CloseHandle(this->handle) == TRUE;
#elif LINUX
    if (this->fd > 0) {
        close(this->fd);
        this->fd = 0;
        return true;
    }

    return false;
#else
#error "File::Close() not supported on this platform"
#endif

    return false;
}

bool File::Seek(int64 offset)
{
#ifdef WINDOWS
    // @todo bug with negative numbers

    // @todo fix this limitation
    if (offset < 0) {
        throw invalid_argument("cannot support seeking back");
    }

    LONG high = (offset & 0xffffffff00000000) >> 32;

    if (high > 0) {
        throw invalid_argument("only supports 32 bit seeking currently");
    }

    LONG low = (offset & 0x00000000ffffffff);
    DWORD result = SetFilePointer(this->handle, low, &high, FILE_BEGIN);

    if (result == INVALID_SET_FILE_POINTER) return false;

    return (result == (offset & 0x00000000ffffffff));
#elif LINUX
    return lseek(this->fd, offset, SEEK_SET) == offset;
#else
#error "File::Seek() not implemented on this platform"
#endif
}

bool File::Write(const void *data, size_t length)
{
#ifdef WINDOWS
    DWORD written;

    BOOL result = WriteFile(this->handle, data, length, &written, NULL);

    return result == TRUE && written == length;
#elif LINUX
    ssize_t result = write(this->fd, data, length);
    if (result == -1) {
        set_system_error();
        return false;
    }

    return (size_t)result == length;
#else
#error "File::Seek() not implemented on this platform"
#endif

    return false;
}

bool File::Flush()
{
#ifdef WINDOWS
    return FlushFileBuffers(this->handle) == TRUE;
#elif LINUX
    if (fsync(this->fd) == -1) {
        set_system_error();
        return false;
    }
    return true;
#else
#error "File::Flush() not implemented on this platform"
#endif
    return false;
}

int File::FileDescriptor()
{
    if (!this->open) return 0;

    return this->fd;
}

bool File::WriteLockEntire()
{
    if (!this->open) {
        throw Exception("called WriteLockEntire on unopened file");
    }

#ifdef WINDOWS
    throw Exception("WriteLockEntire() is not meant to be called on Windows at this time");
#elif LINUX
    flock fl;
    fl.l_type = F_WRLCK;
    fl.l_start = 0;
    fl.l_whence = SEEK_SET;
    fl.l_len = 0;

    // non-blocking call
    int result = fcntl(this->fd, F_SETLK, &fl);

    // POSIX says an errno of EACCESS or EAGAIN means we could not obtain
    // the lock, although it is EAGAIN on most implementations
    if (result == -1) {
        if (errno == EACCES || errno == EAGAIN) {
            return false;
        }

        set_system_error();
        return false;
    }

    return true;
#else
#error "File::WriteLockEntire() not implemented on this platform"
#endif
}

string PathJoin(string const &a, string const &b)
{
    string s = a;
#ifdef WINDOWS
    s += "\\";
#elif LINUX
    s += "/";
#endif

    s += b;

    return s;
}

bool CreateUUID(UUID &u)
{
#ifdef WINDOWS
    ::UUID uuid;
    RPC_STATUS result = UuidCreate(&uuid);
    if (result != RPC_S_OK && result != RPC_S_UUID_LOCAL_ONLY) return false;

    memcpy(&u, &uuid, 16);
    return true;
#elif LINUX
    uuid_t uuid;
    uuid_generate_time(uuid);
    memcpy(&u, &uuid, 16);
    return true;
#else
#warning "functionality not implemented on this platform"
#endif
}

string CreateUUID(bool format)
{
    UUID u;
    if (!CreateUUID(u)) {
        throw Exception("could not create UUID");
    }

    if (format) {
        string formatted;
        if (!FormatUUID(u, formatted)) {
            throw Exception("could not format UUID");
        }

        return formatted;
    }

    return string((const char *)&u, sizeof(u));
}

bool FormatUUID(UUID &u, ::std::string &s)
{
    // formatted UUIDs are 36 characters and we have a NULL
    char buf[37];

#ifdef WINDOWS
    _snprintf_s(&buf[0], sizeof(buf), sizeof(buf),
#else
    snprintf(&buf[0], sizeof(buf),
#endif
        "%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x",
        u.data[0], u.data[1], u.data[2], u.data[3], u.data[4], u.data[5],
        u.data[6], u.data[7], u.data[8], u.data[9], u.data[10], u.data[11],
        u.data[12], u.data[13], u.data[14], u.data[15]
    );
    s.assign(&buf[0], 36);

    return true;
}


Timer::Timer() : signaled(false), running(false), initialized(false)
{
}

Timer::~Timer()
{
#if LINUX
    if (this->initialized) timer_delete(this->timer);
#endif
}

Timer::Timer(uint32 microseconds) : signaled(false), running(false), initialized(false)
{
    this->microseconds = microseconds;
}

void Timer::Initialize()
{
#ifdef WINDOWS
    this->handle = CreateWaitableTimer(NULL, TRUE, NULL);
    if (!this->handle) {
        throw Exception("timer could not be created");
    }
#elif LINUX
    struct sigevent evp;
    memset(&evp, 0, sizeof(evp));
    evp.sigev_notify = SIGEV_NONE;

    int result = timer_create(CLOCK_MONOTONIC, &evp, &this->timer);
    if (result == -1) {
        set_system_error();
        throw Exception("could not create timer");
    }
#else
#error "Timer::Initialize() is not implemented on this platform"
#endif

    this->initialized = true;
    this->signaled = false;
    this->running = false;
}

bool Timer::Reset()
{
    this->signaled = false;

    if (this->running) {
#ifdef WINDOWS
        if (CancelWaitableTimer(this->handle)) {
            this->running = false;
            return true;
        }

        return false;
#elif LINUX
        struct itimerspec v;
        memset(&v, 0, sizeof(v));
        int result = timer_settime(this->timer, 0, &v, NULL);
        if (result == -1) {
            set_system_error();
            return false;
        }

        this->running = false;
        return true;

#endif
    }

    return false;
}

bool Timer::Start(uint32 microseconds)
{
    if (!this->initialized) {
        this->Initialize();
    }

    if (this->running) {
        if (!this->Reset()) {
            return false;
        }
    }

    if (microseconds > 0) {
        this->microseconds = microseconds;
    }

#ifdef WINDOWS
    LARGE_INTEGER duetime;
    duetime.QuadPart = -10 * int32(this->microseconds);
    if (!SetWaitableTimer(this->handle, &duetime, 0, NULL, NULL, FALSE)) {
        return false;
    }

    this->running = true;
    return true;

#elif LINUX
    struct itimerspec v;
    memset(&v, 0, sizeof(v));

    v.it_value.tv_sec = this->microseconds / 1000000;

    // we don't get our nanosecond precision, but do we really care?
    v.it_value.tv_nsec = (this->microseconds % 1000000) * 1000;

    int result = timer_settime(this->timer, 0, &v, NULL);
    if (result == -1) {
        set_system_error();
        return false;
    }

    this->running = true;
    return true;
#endif

    return false;
}

bool Timer::Signaled()
{
    if (this->signaled) return true;
    if (!this->running) return false;

#ifdef WINDOWS
    DWORD result = WaitForSingleObject(this->handle, 0);

    if (result == WAIT_OBJECT_0) {
        this->signaled = true;
        return true;
    }

#elif LINUX
    struct itimerspec v;
    int result = timer_gettime(this->timer, &v);
    if (result == -1) {
        set_system_error();
        // @todo this API breaks convention
        throw Exception("could not obtain timer result");
    }

    if (v.it_value.tv_sec == 0 && v.it_value.tv_nsec == 0) {
        this->signaled = true;
        return true;
    }

    return false;
#endif

    return false;
}

DirectoryChange::DirectoryChange() {}

DirectoryWatcher::~DirectoryWatcher()
{
    // @todo implement
}

DirectoryWatcher::DirectoryWatcher(string const &directory, bool recurse)
#ifdef WINDOWS
    : directory(NULL), started_waiting(false), completion_port(NULL)
#endif
{
    this->path = directory;
    this->recurse = recurse;

#ifdef WINDOWS
    memset(&this->overlapped, 0, sizeof(this->overlapped));

    this->directory = CreateFile(
        this->path.c_str(),
        GENERIC_READ,
        FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL | FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
        NULL
    );

    if (this->directory == INVALID_HANDLE_VALUE) {
        throw Exception("invalid handle");
    }

    this->completion_port = CreateIoCompletionPort(this->directory, NULL, NULL, 0);
    if (!this->completion_port) {
        throw Exception("could not create I/O Completion Port");
    }

#elif LINUX
    // we use inotify
    this->fd = inotify_init();
    if (this->fd == -1) {
        throw Exception("could not initialize inotify descriptor");
    }

    // add watch for root
    int watch = inotify_add_watch(this->fd, this->path.c_str(), IN_CREATE | IN_DELETE | IN_MODIFY);
    if (watch == -1) {
        set_system_error();
        throw Exception("could not add inotify watch for directory");
    }

    this->directories[watch] = "";

    // inotify doesn't support recursive watches, so we need to do it manually
    vector<string> paths;
    if (!DirectoriesInTree(this->path, paths)) {
        throw Exception("could not obtain directories in tree");
    }

    vector<string>::iterator i = paths.begin();
    for (; i != paths.end(); i++) {
        watch = inotify_add_watch(this->fd, i->c_str(), IN_CREATE | IN_DELETE | IN_MODIFY);
        if (watch == -1) {
            set_system_error();
            throw Exception("could not add inotify watch for directory");
        }

        this->directories[watch] = i->substr(this->path.length(), FILENAME_MAX);
    }

#else
#warning "not available on this platform yet"
#endif
}

bool DirectoryWatcher::WaitForChanges(int32 timeout)
{
    // return immediately if we have already collected changes
    if (this->changes.size() > 0) return true;

#ifdef WINDOWS
    if (!this->started_waiting) {
        BOOL watch_result = ReadDirectoryChangesW(this->directory,
            &this->results[0], sizeof(this->results), this->recurse,
            FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME | FILE_NOTIFY_CHANGE_SIZE,
            NULL, &this->overlapped, NULL);
        if (!watch_result) {
            throw Exception("could not start waiting for directory changes");
        }

        this->started_waiting = true;
    }

    DWORD milliseconds = time < 0 ? INFINITE : timeout / 1000;
    DWORD bytes_transferred = 0;
    ULONG_PTR key = 0;
    OVERLAPPED * ol = NULL;

    if (!GetQueuedCompletionStatus(this->completion_port,
        &bytes_transferred, &key, &ol, milliseconds))
    {
        // @todo we might want to check some error conditions here
        // otherwise, the function could abort immediately and cause a runaway
        // thread
        return false;
    }

    // else, we have results!
    this->started_waiting = false;

    size_t results_offset = 0;
    FILE_NOTIFY_INFORMATION *info = NULL;
    char filename[8192];

    do {
        info = (FILE_NOTIFY_INFORMATION *)&this->results[results_offset];
        results_offset += info->NextEntryOffset;

        int result = WideCharToMultiByte(
            CP_UTF8, 0, info->FileName, info->FileNameLength / 2,
            &filename[0], sizeof(filename), NULL, NULL);

        filename[result] = '\0';
        if (!result) continue;

        DirectoryChange change;
        change.Path = filename;

        switch (info->Action) {
            case FILE_ACTION_RENAMED_NEW_NAME:
            case FILE_ACTION_ADDED:
                change.Action = change.ADDED;
                break;

            case FILE_ACTION_RENAMED_OLD_NAME:
            case FILE_ACTION_REMOVED:
                change.Action = change.DELETED;
                break;

            case FILE_ACTION_MODIFIED:
                change.Action = change.MODIFIED;
                break;
        }

        this->changes.push_back(change);

    } while (info->NextEntryOffset != 0);

    return true;

#elif LINUX
    // the inotify descriptor is watching the moment it is created
    // so, we see if data is available and we read it

    struct pollfd pfd = { this->fd, POLLIN, 0 };
    int result = poll(&pfd, 1, timeout / 1000);

    if (result == -1) {
        set_system_error();
        throw Exception("error polling inotify descriptor");
    }

    // no data available
    if (result == 0) return false;

    // we now read off inotify_event structs from the descriptor
    char buf[(sizeof(struct inotify_event) + FILENAME_MAX)*256] = {0};

    ssize_t available = read(this->fd, &buf, sizeof(buf));
    if (available < 0) {
        set_system_error();
        throw Exception("could not read from inotify descriptor");
    }

    ssize_t i = 0;
    while (i < available) {
        struct inotify_event *e = (struct inotify_event *)&buf[i];

        map<int, string>::iterator watch = this->directories.find(e->wd);
        if (watch == this->directories.end()) {
            throw Exception("unknown watch descriptor seen");
        }

        // watch was removed (we don't care how)
        if (e->mask & IN_IGNORED) {
            this->directories.erase(e->wd);
            i += sizeof(struct inotify_event) + e->len;
            continue;
        }

        string name = string(e->name, strlen(e->name)); // e->name can contain trailing NULL bytes

        DirectoryChange change;
        change.Path = PathJoin(watch->second, name);
        string fs_path = PathJoin(this->path, change.Path);

        if (e->mask & IN_CREATE) {
            change.Action = change.ADDED;

            // add inotify watcher if this is a directory
            FileStat st;
            if (!stat(fs_path, st)) {
                throw Exception("could not stat newly created file. weird");
            }

            if (st.type == DIRECTORY) {
                int watch = inotify_add_watch(this->fd, fs_path.c_str(), IN_CREATE | IN_DELETE | IN_MODIFY );
                if (watch == -1) {
                    set_system_error();
                    throw Exception("could not add watch for directory: " + fs_path);
                }

                this->directories[watch] = change.Path;
            }

        }
        else if (e->mask & IN_DELETE) {
            change.Action = change.DELETED;
        }
        else if (e->mask & IN_MODIFY) {
            change.Action = change.MODIFIED;
        }
        else {
            throw Exception("unknown change mask seen. likely coding bug");
        }

        this->changes.push_back(change);

        i += sizeof(struct inotify_event) + e->len;
    }

    return true;

#else
#error "functionality not implemented on this platform"
#endif

    return false;
}

bool DirectoryWatcher::GetChanges(vector<DirectoryChange> &out)
{
    out.clear();

    for (size_t i = 0; i < this->changes.size(); i++) {
        out.push_back(this->changes[i]);
    }

    this->changes.clear();

    return true;
}

Thread::Thread(thread_start_func func, void *data)
{
#ifdef WINDOWS
    LPTHREAD_START_ROUTINE f = (LPTHREAD_START_ROUTINE)func;
    this->thread = CreateThread(NULL, 0, f, data, 0, NULL);
#elif LINUX
    int result = pthread_create(&this->thread, NULL, func, data);
    if (result != 0) {
        throw Exception("error creating thread");
    }
#else
#error "not supported on this platform yet"
#endif
}

Thread::~Thread()
{
#ifdef WINDOWS
    CloseHandle(this->thread);
#endif
}

bool Thread::Join()
{
#ifdef WINDOWS
    DWORD result = WaitForSingleObject(this->thread, INFINITE);
    return WAIT_OBJECT_0 == result;

#elif LINUX
    int result = pthread_join(this->thread, NULL);
    return result == 0;
#else
#error "join_thread() not implemented on your platform yet"
#endif

    return false;
}

bool Thread::Abort()
{
#ifdef WINDOWS
    DWORD rc = 1;
    return TerminateThread(this->thread, rc) == TRUE;
#elif LINUX
    int result = pthread_cancel(this->thread);
    return result == 0;
#else
#error "terminate_thread() not implemented on your platform yet"
#endif

}

bool Thread::Alive()
{
#ifdef WINDOWS
    DWORD code = 0;
    BOOL result = GetExitCodeThread(this->thread, &code);
    if (result == 0) {
        throw Exception("TODO react to bad GetExitCodeThread() result");
    }

    return code == STILL_ACTIVE;
#elif LINUX
    // @todo not reliable since thread could be recycled
    int result = pthread_kill(this->thread, 0);
    if (result == 0) return true;
    if (result == ESRCH) return false;

    // error on side of caution
    return true;
#else
#error "Thread::Alive() not implemented on your platform yet";
#endif
}

ConditionalWait::ConditionalWait() : signaled(false)
#ifdef WINDOWS
    ,cond(NULL)
#endif
{
#ifdef POSIX
    // @todo verify POSIX says no error codes for this (Linux says none)
    assert(!pthread_cond_init(&this->cond, NULL));
    assert(!pthread_mutex_init(&this->mutex, NULL));
#elif WINDOWS
    this->cond = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (!this->cond) {
        throw Exception("could not create Event handle");
    }
#else
#error "ConditionalWait constructor not implemented on your platform";
#endif
}

ConditionalWait::~ConditionalWait()
{
#ifdef POSIX
    int result;
    do {
        result = pthread_cond_destroy(&this->cond);
        if (result == EBUSY) {
            pthread_cond_signal(&this->cond);
        }
    } while (result == EBUSY);

    pthread_mutex_destroy(&this->mutex);
#elif WINDOWS
    if (this->cond) CloseHandle(this->cond);
#else
#error "ConditionalWait destructor not implemented on your platform";
#endif
}

bool ConditionalWait::Signal()
{
    this->signaled = true;

#ifdef POSIX
    return pthread_cond_broadcast(&this->cond) == 0;
#elif WINDOWS
    return SetEvent(this->cond) != 0;
#else
#error "ConditionalWait::Signal() not implemented on your platform";
#endif
}

bool ConditionalWait::Reset()
{
    this->signaled = false;

#ifdef WINDOWS
    return ResetEvent(this->cond) != 0;
#endif

    return true;
}

bool ConditionalWait::Wait(int32 microseconds)
{
    if (microseconds < -1) {
        throw invalid_argument("timeout cannot be less than -1");
    }

    if (this->signaled) return true;

#ifdef POSIX
    bool ret;
    if (microseconds == -1) {
        assert(!pthread_mutex_lock(&this->mutex));
        assert(!pthread_cond_wait(&this->cond, &this->mutex));
        assert(!pthread_mutex_unlock(&this->mutex));
        return true;
    }
    else if (microseconds == 0) {
        int result = pthread_mutex_trylock(&this->mutex);
        if (result == EBUSY) return false;
    }
    else {
        assert(!pthread_mutex_lock(&this->mutex));
    }

    struct timeval now;
    gettimeofday(&now, NULL);

    struct timespec ts;
    ts.tv_sec = now.tv_sec;
    ts.tv_nsec = now.tv_usec * 1000;

    ts.tv_sec += microseconds / 1000000;
    ts.tv_nsec += 1000 * (microseconds % 1000000);
    if (ts.tv_nsec > 1000000000) {
        ts.tv_sec++;
        ts.tv_nsec -= 1000000000;
    }

    int result;
    do {
        result = pthread_cond_timedwait(&this->cond, &this->mutex, &ts);
    } while (result == EINTR);

    if (result == 0) {
        ret = true;
    }
    else if (result == ETIMEDOUT) {
        ret = false;
    }
    else {
        pthread_mutex_unlock(&this->mutex);
        throw logic_error("Unknown result code from pthread_cond_timedwait(): " + result);
    }

    pthread_mutex_unlock(&this->mutex);

    return ret;
#elif WINDOWS


    DWORD result;
    if (microseconds == -1) {
        result = WaitForSingleObject(this->cond, INFINITE);
    }
    else {
        result = WaitForSingleObject(this->cond, microseconds / 1000);
    }

    if (result == WAIT_OBJECT_0) {
        return true;
    }
    else if (result == WAIT_TIMEOUT) {
        return false;
    }
    else if (result == WAIT_ABANDONED) {
        throw Exception("wait abandoned");
    }
    else if (result == WAIT_FAILED) {
        throw Exception("wait failed");
    }
    else {
        throw logic_error("unknown result from WaitForSingleObject: " + result);
    }
#else
#error "ConditionalWait::Wait() not implemented on your platform";
#endif
}

} // platform namespace

} // namespace
