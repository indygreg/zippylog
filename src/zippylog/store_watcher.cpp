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

#include <zippylog/store_watcher.hpp>

#include <zippylog/platform.hpp>
#include <zippylog/protocol.pb.h>
#include <zippylog/zeromq.hpp>

#ifdef WINDOWS
#include <WinBase.h>
#include <tchar.h>
#endif

namespace zippylog {

StoreWatcher::StoreWatcher(zippylog::Store *store, zmq::context_t *ctx, const string endpoint)
{
    this->_store = store;
    this->_ctx = ctx;
    this->_endpoint = endpoint;

    this->socket = new socket_t(*this->_ctx, ZMQ_PUB);
    this->socket->bind(this->_endpoint.c_str());
}

void StoreWatcher::run()
{
    HANDLE directory;
    BYTE results[32768];
    DWORD results_length;
    DWORD results_length_written;
    BOOL watch_result;

    directory = CreateFile(
        this->_store->StorePath().c_str(),
        GENERIC_READ,
        FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL | FILE_FLAG_BACKUP_SEMANTICS,
        NULL
    );
    if (directory == INVALID_HANDLE_VALUE) {

    }

    char filename[8192];

    while (true) {
        watch_result = ReadDirectoryChangesW(directory, &results[0], sizeof(results), true,
            FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME | FILE_NOTIFY_CHANGE_SIZE,
            &results_length_written, NULL, NULL);
        if (watch_result == 0) {
            // TODO log error and continue if it wasn't too severe
            char error[8192];
            windows_error(&error[0], sizeof(error));
            throw error;
            break;
        }

        // results is a FILE_NOTIFY_INFORMATION structure
        size_t results_offset = 0;
        FILE_NOTIFY_INFORMATION *info = NULL;
        do {
            info = (FILE_NOTIFY_INFORMATION *)&results[results_offset];
            results_offset += info->NextEntryOffset;

            int result = WideCharToMultiByte(
                CP_UTF8,
                0,
                info->FileName,
                info->FileNameLength / 2,
                &filename[0],
                sizeof(filename),
                NULL,
                NULL
            );
            filename[result] = '\0';
            if (!result) {
                continue;
            }

            string store_path = "/";
            store_path.append(filename);

            // replace backslashes with forward slashes (Windows sanity)
            string::size_type off;
            while ((off = store_path.find_first_of("\\", 0)) != string::npos) {
                store_path[off] = '/';
            }

            switch (info->Action) {
                case FILE_ACTION_RENAMED_NEW_NAME:
                case FILE_ACTION_ADDED:
                    this->HandleAdded(store_path);
                    break;

                case FILE_ACTION_RENAMED_OLD_NAME:
                case FILE_ACTION_REMOVED:
                    this->HandleDeleted(store_path);
                    break;

                case FILE_ACTION_MODIFIED:
                    this->HandleModified(store_path);
                    break;
            }
        } while (info->NextEntryOffset != 0);
    }

}

void StoreWatcher::SendChangeMessage(Envelope &e)
{
    zeromq::send_envelope(this->socket, e);
}

void StoreWatcher::HandleAdded(string path)
{
    string bucket, set, stream;
    if (!Store::ParsePath(path, bucket, set, stream)) return;

    // TODO need to validate streams are files, buckets/sets are directories

    Envelope e = Envelope();

    if (!stream.empty()) {
        protocol::StoreChangeStreamAdded m = protocol::StoreChangeStreamAdded();
        m.set_bucket(bucket);
        m.set_stream_set(set);
        m.set_stream(stream);
        m.add_to_envelope(&e);
        this->SendChangeMessage(e);
    }
    else if (!set.empty()) {
        protocol::StoreChangeStreamSetAdded m = protocol::StoreChangeStreamSetAdded();
        m.set_bucket(bucket);
        m.set_stream_set(set);
        m.add_to_envelope(&e);
        this->SendChangeMessage(e);
    }
    else {
        protocol::StoreChangeBucketAdded m = protocol::StoreChangeBucketAdded();
        m.set_bucket(bucket);
        m.add_to_envelope(&e);
        this->SendChangeMessage(e);
    }

}

void StoreWatcher::HandleDeleted(string path)
{
    string bucket, set, stream;
    if (!Store::ParsePath(path, bucket, set, stream)) return;

    Envelope e = Envelope();

    if (!stream.empty()) {
        protocol::StoreChangeStreamDeleted m = protocol::StoreChangeStreamDeleted();
        m.set_bucket(bucket);
        m.set_stream_set(set);
        m.set_stream(stream);
        m.add_to_envelope(&e);
        this->SendChangeMessage(e);
    }
    else if (!set.empty()) {
        protocol::StoreChangeStreamSetDeleted m = protocol::StoreChangeStreamSetDeleted();
        m.set_bucket(bucket);
        m.set_stream_set(set);
        m.add_to_envelope(&e);
        this->SendChangeMessage(e);
    }
    else {
        protocol::StoreChangeBucketDeleted m = protocol::StoreChangeBucketDeleted();
        m.set_bucket(bucket);
        m.add_to_envelope(&e);
        this->SendChangeMessage(e);
    }
}

void StoreWatcher::HandleModified(string path)
{
    string bucket, set, stream;
    if (!Store::ParsePath(path, bucket, set, stream)) return;

    Envelope e = Envelope();

    if (!stream.empty()) {
        protocol::StoreChangeStreamAppended m = protocol::StoreChangeStreamAppended();
        m.set_bucket(bucket);
        m.set_stream_set(set);
        m.set_stream(stream);
        m.add_to_envelope(&e);
        this->SendChangeMessage(e);
    }
    // what else is there? buckets and stream sets don't really modify, do they?
}

} // namespace
