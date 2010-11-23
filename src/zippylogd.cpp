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
#include <zippylog/zippylogd/broker.hpp>

#include <iostream>
#include <string>

using ::zippylog::zippylogd::Broker;
using ::std::cout;
using ::std::endl;
using ::std::string;
using ::std::vector;

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
#ifdef LINUX
    // TODO this signal handling is horribly naive
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
#endif

    try {
        ::zippylog::initialize_library();

        //::zippylog::zippylogd::Zippylogd instance(params);
        //instance.Run();

        // TODO remove broker code in favor of zippylogd class
        Broker broker(argv[1]);

#ifdef LINUX
        broker.RunAsync();
        while (active) pause();
#elif WINDOWS
        broker.Run();
#else
#error "not implemented on this platform"
#endif
    }
    catch (string s) {
        cout << "Exception:" << endl;
        cout << s;
        return 1;
    }
    catch (char * s) {
        cout << "Exception: " << s << endl;
        return 1;
    }
    catch (...) {
        cout << "received an exception" << endl;
        return 1;
    }

    ::zippylog::shutdown_library();

    return 0;
}
