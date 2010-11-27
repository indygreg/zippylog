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

#include <zippylog/zippylog.hpp>
#include <zippylog/device/server.hpp>

#include <iostream>
#include <string>

using ::std::cout;
using ::std::endl;
using ::std::string;
using ::std::vector;
using ::zippylog::device::Server;
using ::zippylog::device::ServerStartParams;

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
    if (argc < 2) {
        cout << "Usage: zippylogd /path/to/config/file" << endl;
        return 1;
    }

#ifdef LINUX
    // TODO this signal handling is horribly naive
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
#endif

    try {
        ::zippylog::initialize_library();

        ServerStartParams config;
        string error;

        if (!Server::ParseConfig(argv[1], config, error)) {
            cout << "Error loading config file: " << error << endl;
            return 1;
        }

        Server server(config);

#ifdef LINUX
        server.RunAsync();
        while (active) pause();
#elif WINDOWS
        server.Run();
#else
#error "not implemented on this platform"
#endif
    }
    catch (::std::exception e) {
        cout << "Exception: " << e.what() << endl;
        return 1;
    }

    ::zippylog::shutdown_library();

    return 0;
}
