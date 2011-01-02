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

#pragma once

#include <zippylog/client.hpp>

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using ::std::cout;
using ::std::endl;
using ::std::ostringstream;
using ::std::string;
using ::std::vector;
using ::zippylog::client::Client;
using ::zippylog::protocol::StoreInfo;
using ::zippylog::Exception;

class ZippylogclientParams {
public:
    ZippylogclientParams() :
        store_info(false)
    { }

    bool store_info;

    vector<string> servers;
};

static bool ParseCommandArguments(
    vector<string> args,
    ZippylogclientParams &params,
    string &error)
{
    if (args.size() < 2) {
        ostringstream usage;
        usage
<< "zippylogclient - A client for zippylog servers"                                 << endl
<<                                                                                     endl
<< "Usage: zippylogclient [options] server0 [server1 ... serverN]"                  << endl
<<                                                                                     endl
<< "The program is invoked with arguments pointing to 1 or more zippylog servers"   << endl
<< "zippylog servers along with optional arguments that control behavior."          << endl
<<                                                                                     endl
<< "SERVER ARGUMENTS"                                                               << endl
<<                                                                                     endl
<< "The server arguments are 0MQ endpoints/URIs of zippylog server agents."         << endl
<< "e.g. tcp://myhost.mydomain:52483 or ipc:///tmp/zippylogd"                       << endl
<<                                                                                     endl
<< "PROGRAM OPTIONS"                                                                << endl
<<                                                                                     endl
<< "The following program options control behavior:"                                << endl
<<                                                                                     endl
<< "  --storeinfo    Obtain the store info and exit"                                << endl
        ;
        error = usage.str();

        return false;
    }

    // get rid of first element, the program name
    args.erase(args.begin());

    for (int i = 0; i < args.size(); i++) {
        string arg = args[i];

        if (arg == "--storeinfo") {
            params.store_info = true;
        }
        else if (arg[0] != '-') {
            params.servers.push_back(arg);
        }
    }

    return true;
}

int ShowStoreInfo(vector<Client *> &clients)
{
    for (vector<Client *>::iterator itor = clients.begin(); itor != clients.end(); itor++) {
        StoreInfo si;

        if ((*itor)->StoreInfo(si, 10000000)) {
            cout << si.DebugString() << endl;
        }
    }

    return 0;
}

int main(int argc, const char * const argv[])
{
    try {
        ::zippylog::initialize_library();

        vector<string> args;
        for (int i = 0; i < argc; i++) {
            args.push_back(argv[i]);
        }

        string error;
        ZippylogclientParams params;

        if (!ParseCommandArguments(args, params, error)) {
            cout << error << endl;
            return 1;
        }

        vector<Client *> clients;

        ::zmq::context_t ctx(2);

        for (size_t i = 0; i < params.servers.size(); i++) {
            clients.push_back(new Client(&ctx, params.servers[i]));
        }

        int result;

        if (params.store_info) {
            result = ShowStoreInfo(clients);
        }

        else {
            cout << "Not sure what to do!" << endl;
            return 1;
        }

        for (vector<Client *>::iterator itor = clients.begin(); itor != clients.end(); itor++) {
            delete *itor;
        }
        clients.clear();

        ::zippylog::shutdown_library();
    }
    catch (Exception e) {
        cout << "zippylog Exception:" << endl;
        cout << e.what() << endl;
        return 1;
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

    return 0;
}

