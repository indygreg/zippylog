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
#include <zippylog/broker.hpp>
#include <zippylog/store.hpp>

#include <iostream>
#include <string>

using ::zippylog::server::Broker;
using ::std::cout;
using ::std::endl;

int main(int argc, const char * const argv[])
{
    if (argc != 2) {
        cout << "Usage: pblogd /path/to/config/file.lua" << endl;
        return 1;
    }

    try {
        Broker broker(argv[1]);
        broker.run();
    }
    catch (...) {
        cout << "received an exception" << endl;
        return 1;
    }

    zippylog_shutdown();

    return 0;
}
