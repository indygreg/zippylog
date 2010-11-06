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
#include <sys/io.h>
#include <sys/time.h>
#endif

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>

namespace zippylog {

void * create_thread(thread_start_func func, void *data)
{
#ifdef WINDOWS
    LPTHREAD_START_ROUTINE f = (LPTHREAD_START_ROUTINE)func;
    HANDLE thread = CreateThread(NULL, 0, f, data, 0, NULL);

    return thread;

#elif defined(LINUX)
    pthread_t *t = (pthread_t *)malloc(sizeof(pthread_t));
    if (!t) throw "could not allocate thread memory";
    int result = pthread_create(t, NULL, func, data);
    if (result != 0) {
        throw "error creating thread";
    }

    return (void *)t;
#else
#error "Threading not supported on this platform yet"
    // TODO implement pthread support
#endif

    return NULL;
}

bool join_thread(void *thread)
{
#ifdef WINDOWS
    DWORD result = WaitForSingleObject((HANDLE)thread, INFINITE);
    return WAIT_OBJECT_0 == result;

#elif defined(LINUX)
    int result = pthread_join(*(pthread_t *)thread, NULL);
    return result == 0;
#else
#error "join_thread() not implemented on your platform yet"
#endif
}

bool terminate_thread(void *thread)
{
#ifdef WINDOWS
    DWORD rc = 1;
    return TerminateThread(thread, rc);
#elif defined(LINUX)
    int result = pthread_cancel(*(pthread_t *)thread);
    return result == 0;
#else
#error "terminate_thread() not implemented on your platform yet"
#endif
}

// congratulations, this is the 4,234,532,657 time in programming history this
// function has been written!
bool directory_entries(const string dir, vector<dir_entry> &v)
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
        dir_entry entry;
        entry.name = info.cFileName;
        entry.size = info.nFileSizeHigh << 32 + info.nFileSizeLow;
        if (info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            entry.type = 1;
        }
        else if (info.dwFileAttributes & FILE_ATTRIBUTE_NORMAL || info.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE) {
            entry.type = 2;
        }
        else {
            entry.type = 0;
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
        dir_entry entry;

        entry.name = d->d_name;

        switch (d->d_type) {
            case DT_DIR:
                entry.type = 1;
                break;
            case DT_REG:
                entry.type = 2;
                break;
            default:
                entry.type = 0;
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

    if (stat(path.c_str(), &result) != 0) return false;

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
#else
#error "Not supported on this platform yet"
#endif

    return result == 0;
}

bool PathIsDirectory(const string path)
{
    FileStat st;
    if (!stat(path, st)) return false;

    return st.type == DIRECTORY;
}

File::File()
{
    this->open = false;
#ifdef WINDOWS
    this->handle = NULL;
#endif
}

bool OpenFile(File &f, const string path, int flags)
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

    f.handle = h;
    f.fd = _open_osfhandle((long)h, fdflags);
    f.open = true;

    return true;

#endif
    return true;
}

bool FileClose(File &f)
{
#ifdef WINDOWS
    if (!f.open) return true;
    return CloseHandle(f.handle) == TRUE;
#endif
}

bool FileSeek(File &f, int64 offset)
{
#ifdef WINDOWS
    return _lseek(f.fd, offset, SEEK_SET) == offset;
#endif
}

bool FileWrite(File &f, const void *data, size_t length)
{
#ifdef WINDOWS
    DWORD written;

    BOOL result = WriteFile(f.handle, data, length, &written, NULL);

    return result == TRUE && written == length;
#endif
}

bool FlushFile(File &f)
{
#ifdef WINDOWS
    return FlushFileBuffers(f.handle) == TRUE;
#endif
    return false;
}

string PathJoin(const string &a, const string &b)
{
    string s = a;
#ifdef WINDOWS
    s += "\\";
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
#else
#warning "functionality not implemented on this platform"
#endif
}


Timer::Timer()
{
}

Timer::Timer(uint32 microseconds)
{
    this->microseconds = microseconds;

#ifdef WINDOWS
    this->handle = CreateWaitableTimer(NULL, TRUE, NULL);
    if (!this->handle) {
        throw "timer could not be created";
    }
#endif

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
#endif
    }

    return false;
}

bool Timer::Start()
{
    if (this->running) {
        if (!this->Reset()) {
            return false;
        }
    }

#ifdef WINDOWS
    LARGE_INTEGER duetime;
    duetime.QuadPart = -10 * int32(this->microseconds);
    if (!SetWaitableTimer(this->handle, &duetime, 0, NULL, NULL, FALSE)) {
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

#ifdef WINDOWS
    DWORD result = WaitForSingleObject(this->handle, 0);

    if (result == WAIT_OBJECT_0) {
        this->signaled = true;
        return true;
    }

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

DirectoryWatcher::DirectoryWatcher(const string &directory, bool recurse) : started_waiting(false)
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
#else
#warning "not available on this platform yet"
#endif
}

bool DirectoryWatcher::WaitForChanges(int32 timeout)
{
    // return immediately if we have already collected changes
    if (this->changes.size() > 0) return true;

    if (!this->started_waiting) {
#ifdef WINDOWS
        BOOL watch_result = ReadDirectoryChangesW(this->directory,
            &this->results[0], sizeof(this->results), this->recurse,
            FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME | FILE_NOTIFY_CHANGE_SIZE,
            NULL, &this->overlapped, NULL);
        if (!watch_result) {
            throw "could not start waiting for directory changes";
        }

        this->started_waiting = true;
#else
#warning "functionality not implemented on this platform"
#endif
    }

    // now, wait for completion
#ifdef WINDOWS
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

#else
#warning "functionality not implemented on this platform"
#endif

    return true;
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


} // platform namespace

} // namespace
