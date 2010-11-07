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

#include <zippylog/zippylog.h>
#include <zippylog/zippylogd/broker.hpp>
#include <zippylog/store.hpp>

#include <iostream>
#include <string>

using ::zippylog::zippylogd::Broker;
using ::std::cout;
using ::std::endl;

#ifdef LINUX
static volatile sig_atomic_t active = 1;

void signal_handler(int signo)
{
    active = 0;

    signal(signo, SIG_DFL);
}
#endif

int main(int argc, const char * const argv[])
{
    if (argc != 2) {
        cout << "Usage: zippylogd /path/to/config/file" << endl;
        return 1;
    }

#ifdef LINUX
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
#endif

    try {
        zippylog_init();

        Broker broker(argv[1]);

#ifdef LINUX
        broker.RunAsync();
        while (active) pause();
#elif WINDOWS
        broker.run();
#else
#error "not implemented on this platform"
#endif
    }
    catch (...) {
        cout << "received an exception" << endl;
        return 1;
    }

    zippylog_shutdown();

    return 0;
}
