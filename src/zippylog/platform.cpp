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

#include <zippylog/platform.hpp>

#ifdef WINDOWS
#include <direct.h>
#include <tchar.h>
#include <io.h>
#endif

#ifdef LINUX
#include <pthread.h>
#include <stdlib.h>
#include <dirent.h>
#include <errno.h>
#include <poll.h>
#include <string.h>
#include <stdio.h> // for FILENAME_MAX
#include <sys/io.h>
#include <sys/inotify.h>
#include <sys/time.h>
#include <uuid/uuid.h>
#endif

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>

namespace zippylog {

void windows_error(char *buffer, size_t buffer_size)
{
#ifdef WINDOWS
    DWORD errcode = GetLastError ();
    DWORD rc = FormatMessageA (FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS, NULL, errcode, MAKELANGID(LANG_NEUTRAL,
        SUBLANG_DEFAULT), buffer, buffer_size, NULL );
#endif
}

namespace platform {

#ifdef LINUX
static __thread int system_error = 0;
#endif

void set_system_error()
{
#ifdef LINUX
    system_error = errno;
#elif WINDOWS
    // TODO
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
    // TODO implement
    return false;
#else
#error "get_system_error not implemented on this platform"
#endif
}

// congratulations, this is the 4,234,532,657 time in programming history this
// function has been written!
bool DirectoryEntries(const string &dir, vector<DirectoryEntry> &v)
{
    //complicated case first
#ifdef WINDOWS
    // TODO fix potential buffer overrun
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
        entry.size = info.nFileSizeHigh << 32 + info.nFileSizeLow;
        if (info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            entry.type = FileType::DIRECTORY;
        }
        else if (info.dwFileAttributes & FILE_ATTRIBUTE_NORMAL || info.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE) {
            entry.type = FileType::REGULAR;
        }
        else {
            entry.type = FileType::UNKNOWN;
        }

        v.push_back(entry);

    } while(FindNextFile(handle, &info) != 0);

    FindClose(handle);
    return true;

#elif HAVE_OPENDIR
    DIR * dh = opendir(dir.c_str());
    if (!dh) return false;

    dirent *d = NULL;

    while (d = readdir(dh)) {
        DirectoryEntry entry;

        entry.name = d->d_name;

        switch (d->d_type) {
            case DT_DIR:
                entry.type = DIRECTORY;
                break;
            case DT_REG:
                entry.type = FILE;
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
    tv.tv_usec = (milliseconds * 1000000) % 1000000000;

    nanosleep(&tv);
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
    // TODO make windows time resolution better
    FILETIME time;
    GetSystemTimeAsFileTime(&time);
    ULARGE_INTEGER wintime;
    wintime.HighPart = time.dwHighDateTime;
    wintime.LowPart = time.dwLowDateTime;

    // Windows time is in 100 nanosecond increments
    // convert to microseconds and subtract offset
    t.epoch_micro = wintime.QuadPart / 10 - 11644473600000000;
    t.epoch_sec = t.epoch_micro / 1000000;
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
    to.epoch_sec = from / 1000000;
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
    // TODO permissions should be passed as argument
#ifdef WINDOWS
    // TODO use native Windows API
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

bool DirectoriesInDirectory(const string &dir, vector<DirectoryEntry> &v)
{
    v.clear();
    vector<DirectoryEntry> entries;
    if (!DirectoryEntries(dir, entries)) return false;

    vector<DirectoryEntry>::iterator i = entries.begin();
    for (; i != entries.end(); i++) {
        if (i->type == FileType::DIRECTORY && i->name[0] != '.') {
            v.push_back(*i);
        }
    }

    return true;
}

bool DirectoriesInDirectory(const string &dir, vector<string> &v)
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

bool FilesInDirectory(const string &dir, vector<DirectoryEntry> &v)
{
    v.clear();
    vector<DirectoryEntry> entries;
    if (!DirectoryEntries(dir, entries)) return false;

    vector<DirectoryEntry>::iterator i = entries.begin();
    for (; i != entries.end(); i++) {
        if (i->type == FileType::REGULAR) {
            v.push_back(*i);
        }
    }

    return true;
}

bool FilesInDirectory(const string &dir, vector<string> &v)
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

bool DirectoriesInTree(const string &path, vector<string> &paths)
{
    vector<DirectoryEntry> entries;

    if (!DirectoryEntries(path, entries)) {
        return false;
    }

    vector<DirectoryEntry>::iterator i = entries.begin();
    for (; i != entries.end(); i++) {
        if (i->type != FileType::DIRECTORY) continue;

        if (i->name[0] == '.') continue;

        string full_path = PathJoin(path, i->name);

        paths.push_back(full_path);

        DirectoriesInTree(full_path, paths);
    }

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

bool File::Open(const string &path, int flags)
{
#ifdef WINDOWS
    DWORD access = 0;
    // kindergarten taught me sharing is good. why doesn't Windows know this?
    DWORD share = FILE_SHARE_READ | FILE_SHARE_WRITE;
    DWORD creation = 0;
    DWORD attributes = 0;
    int fdflags = 0;

    if (flags & READ) access |= GENERIC_READ;
    if (flags & WRITE) access |= GENERIC_WRITE;
    if (flags & APPEND) {
        access |= FILE_APPEND_DATA;
        fdflags |= _O_APPEND;
    }

    if (flags & CREATE) {
        creation = OPEN_ALWAYS;
    }
    else if (flags & TRUNCATE) {
        creation = CREATE_ALWAYS;
    }

    // TODO probably an issue w/ ANSI vs Unicode functions for file name
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
        throw "must have one of READ or WRITE flag set";
    }

    if (flags & APPEND) open_flags |= O_APPEND;
    if (flags & CREATE) {
        open_flags |= O_CREAT;
        mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP;
    }

    if (flags & TRUNCATE) open_flags |= O_TRUNC;
    // BINARY has no meaning on Linux

    this->fd = open(path.c_str(), open_flags, mode);
    if (this->fd == -1) {
        set_system_error();
        return false;
    }

    this->open = true;

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
    return _lseek(this->fd, offset, SEEK_SET) == offset;
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

    return result == length;
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
    if (this->open) return 0;

    return this->fd;
}

string PathJoin(const string &a, const string &b)
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


Timer::Timer() : initialized(false)
{
}

Timer::~Timer()
{
#if LINUX
    timer_delete(this->timer);
#endif
}

Timer::Timer(uint32 microseconds) : initialized(false)
{
    this->microseconds = microseconds;
}

void Timer::Initialize()
{
#ifdef WINDOWS
    this->handle = CreateWaitableTimer(NULL, TRUE, NULL);
    if (!this->handle) {
        throw "timer could not be created";
    }
#elif LINUX
    struct sigevent evp;
    memset(&evp, 0, sizeof(evp));
    evp.sigev_notify = SIGEV_NONE;

    int result = timer_create(CLOCK_REALTIME, &evp, &this->timer);
    if (result == -1) {
        set_system_error();
        throw "could not create timer";
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
    v.it_value.tv_nsec = (this->microseconds * 1000) % 1000000;

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
        // TODO this API breaks convention
        throw "could not obtain timer result";
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

DirectoryWatcher::DirectoryWatcher()
{
    throw "default constructor not available";
}

DirectoryWatcher::DirectoryWatcher(const DirectoryWatcher &orig)
{
    throw "copy constructor not available";
}

DirectoryWatcher & DirectoryWatcher::operator=(const DirectoryWatcher &orig)
{
    throw "assignment operator not available";
}

DirectoryWatcher::~DirectoryWatcher()
{
    // TODO implement
}

DirectoryWatcher::DirectoryWatcher(const string &directory, bool recurse)
#ifdef WINDOWS
    : started_waiting(false)
#endif
{
    this->path = directory;
    this->recurse = recurse;

#ifdef WINDOWS
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
        throw "invalid handle";
    }

    this->completion_port = CreateIoCompletionPort(this->directory, NULL, NULL, 0);
    if (!this->completion_port) {
        throw "could not create I/O Completion Port";
    }

    memset(&this->overlapped, 0, sizeof(this->overlapped));
#elif LINUX
    // we use inotify
    this->fd = inotify_init();
    if (this->fd == -1) {
        throw "could not initialize inotify descriptor";
    }

    // add watch for root
    int watch = inotify_add_watch(this->fd, this->path.c_str(), IN_CREATE | IN_DELETE | IN_MODIFY);
    if (watch == -1) {
        set_system_error();
        throw "could not add inotify watch for directory";
    }

    this->directories[watch] = "";

    // inotify doesn't support recursive watches, so we need to do it manually
    vector<string> paths;
    if (!DirectoriesInTree(this->path, paths)) {
        throw "could not obtain directories in tree";
    }

    vector<string>::iterator i = paths.begin();
    for (; i != paths.end(); i++) {
        watch = inotify_add_watch(this->fd, i->c_str(), IN_CREATE | IN_DELETE | IN_MODIFY);
        if (watch == -1) {
            set_system_error();
            throw "could not add inotify watch for directory";
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
            throw "could not start waiting for directory changes";
        }

        this->started_waiting = true;
    }

    DWORD milliseconds = time < 0 ? INFINITE : timeout / 1000;
    DWORD bytes_transferred = 0;
    ULONG key = 0;
    LPOVERLAPPED ol = NULL;

    if (!GetQueuedCompletionStatus(this->completion_port,
        &bytes_transferred, &key, &ol, milliseconds))
    {
        // TODO look at MSDN docs for function and verify we shouldn't do more
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
        throw "error polling inotify descriptor";
    }

    // no data available
    if (result == 0) return false;

    // we now read off inotify_event structs from the descriptor
    char buf[(sizeof(struct inotify_event) + FILENAME_MAX)*256] = {0};

    ssize_t available = read(this->fd, &buf, sizeof(buf));
    if (available < 0) {
        set_system_error();
        throw "could not read from inotify descriptor";
    }

    ssize_t i = 0;
    while (i < available) {
        struct inotify_event *e = (struct inotify_event *)&buf[i];

        map<int, string>::iterator watch = this->directories.find(e->wd);
        if (watch == this->directories.end()) {
            throw "unknown watch descriptor seen";
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
                throw "could not stat newly created file. weird";
            }

            if (st.type == DIRECTORY) {
                int watch = inotify_add_watch(this->fd, fs_path.c_str(), IN_CREATE | IN_DELETE | IN_MODIFY );
                if (watch == -1) {
                    set_system_error();
                    throw "could not add watch for directory: " + fs_path;
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
            throw "unknown change mask seen. likely coding bug";
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
        throw "error creating thread";
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
    return TerminateThread(this->thread, rc);
#elif LINUX
    int result = pthread_cancel(this->thread);
    return result == 0;
#else
#error "terminate_thread() not implemented on your platform yet"
#endif

}

} // platform namespace

} // namespace
