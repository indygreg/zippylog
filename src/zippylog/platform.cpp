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
#include <Windows.h>
#endif

namespace zippylog {

void * create_thread(void * func, void *data)
{
#ifdef WINDOWS
    LPTHREAD_START_ROUTINE f = (LPTHREAD_START_ROUTINE)func;
    HANDLE thread = CreateThread(NULL, 0, f, data, 0, NULL);

    return thread;

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

#else
#error "join_thread() not implemented on your platform yet"
#endif
}

bool terminate_thread(void *thread)
{
#ifdef WINDOWS
    DWORD rc = 1;
    return TerminateThread(thread, rc);
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
    strcpy(path, dir.c_str());

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
        else if (info.dwFileAttributes & FILE_ATTRIBUTE_NORMAL) {
            entry.type = 2;
        }

        v.push_back(entry);

    } while(FindNextFile(handle, &info) != 0);

    FindClose(handle);
    return true;

#else
    // TODO implement POSIX version
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

} // namespace
