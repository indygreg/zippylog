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
#include <zippylog/zippylogd.pb.h>

namespace zippylog {

using ::zippylog::zippylogd::StoreWatcherStartup;
using ::zippylog::zippylogd::StoreWatcherShutdown;

StoreWatcher::StoreWatcher(StoreWatcherStartParams params) :
    watcher(params.store_path, true),
    logging_sock(NULL),
    _store(params.store_path)
{
    if (!params.active) {
        throw "active semaphore cannot be NULL";
    }

    this->_ctx = params.zctx;
    this->_endpoint = params.endpoint;
    this->logging_endpoint = params.logging_endpoint;
    this->active = params.active;

    platform::UUID uuid;
    platform::CreateUUID(uuid);

    this->id = string((const char *)&uuid, sizeof(uuid));

    this->logging_sock = new socket_t(*this->_ctx, ZMQ_PUSH);
    this->logging_sock->connect(this->logging_endpoint.c_str());
}

StoreWatcher::~StoreWatcher()
{
    if (this->logging_sock) delete this->logging_sock;
}

void StoreWatcher::Run()
{
    {
        StoreWatcherStartup log = StoreWatcherStartup();
        log.set_id(this->id);
        Envelope logenvelope = Envelope();
        log.add_to_envelope(&logenvelope);
        zeromq::send_envelope(this->logging_sock, logenvelope);
    }

    while (*this->active) {
        // wait up to 250 milliseconds for change
        // this will return immediately if there has been a change, so the
        // delay likely only impacts thread shutdown
        if (!this->watcher.WaitForChanges(250000)) continue;

        vector<platform::DirectoryChange> changes;

        if (!this->watcher.GetChanges(changes)) {
            throw "could not obtain directory changes... weird";
        }

        vector<platform::DirectoryChange>::iterator itor = changes.begin();
        for (; itor != changes.end(); itor++) {
            string store_path;

            if (itor->Path[0] == '/') {
                store_path = itor->Path;
            }
            else {
                store_path = "/" + itor->Path;
            }

            // replace backslashes with forward slashes (Windows sanity)
            for (size_t i = store_path.length(); i; i--) {
                if (store_path[i-1] == '\\') store_path[i-1] = '/';
            }

            string fs_path = this->_store.PathToFilesystemPath(itor->Path);
            platform::FileStat stat;
            platform::stat(fs_path, stat);

            switch (itor->Action) {
                case platform::DirectoryChange::ADDED:
                    this->HandleAdded(store_path, stat);
                    break;

                case platform::DirectoryChange::DELETED:
                    this->HandleDeleted(store_path);
                    break;

                case platform::DirectoryChange::MODIFIED:
                    this->HandleModified(store_path, stat);
                    break;

                default:
                    throw "unknown action seen. buggy code.";
            }
        }
    }

    {
        StoreWatcherShutdown log = StoreWatcherShutdown();
        log.set_id(this->id);
        Envelope logenvelope = Envelope();
        log.add_to_envelope(&logenvelope);
        zeromq::send_envelope(this->logging_sock, logenvelope);
    }
}

} // namespace
