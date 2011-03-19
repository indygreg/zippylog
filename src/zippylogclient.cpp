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
#include <zippylog/util.hpp>

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include <stdlib.h>

using ::std::cout;
using ::std::endl;
using ::std::ostringstream;
using ::std::string;
using ::std::vector;
using ::zippylog::client::Client;
using ::zippylog::client::StoreMirrorState;
using ::zippylog::client::StreamSegment;
using ::zippylog::protocol::StoreInfo;
using ::zippylog::Exception;
using ::zippylog::util::GetArgumentValueAndPop;

class ZippylogclientParams {
public:
    ZippylogclientParams() :
        store_info(false),
        mirror(false),
        mirror_interval(-1)
    { }

    bool store_info;

    /// whether to mirror
    bool mirror;

    /// path to state file (not required)
    string mirror_state;

    /// sleep period between mirror operations. if -1, only mirror once
    int mirror_interval;

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
<< "along with optional arguments that control behavior."                           << endl
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
<< "  --mirror          Fetches all content from remote stream."                    << endl
<< "  --mirror-state    Path to file to hold mirror state. If file exists, state"   << endl
<< "                    will be read from it. If does not exist, it will be"  << endl
<< "                    created. Upon mirroring, this file will be updated with"    << endl
<< "                    the latest mirror state."                                   << endl
<< "  --mirror-interval If specified, mirroring will repeat after sleeping N"       << endl
<< "                    seconds where N is the value after this argument."          << endl
<< "  --storeinfo       Obtain the store info and exit"                             << endl
        ;
        error = usage.str();

        return false;
    }

    // get rid of first element, the program name
    args.erase(args.begin());

    for (size_t i = 0; i < args.size();) {
        string arg = args[i];

        if (arg == "--storeinfo") {
            params.store_info = true;
        }
        else if (arg == "--mirror") {
            params.mirror = true;
        }
        else if (arg == "--mirror-state") {
            string value;
            if (!GetArgumentValueAndPop(args, i, value)) {
                error = "--mirror-state requires a value";
                return false;
            }
            params.mirror_state = value;
            params.mirror = true;
            continue;
        }
        else if (arg == "--mirror-interval") {
            string value;
            if (!GetArgumentValueAndPop(args, i, value)) {
                error = "--mirror-interval requires a value";
                return false;
            }

            int result = atoi(value.c_str());
            if (errno == EINVAL) {
                error = "--mirror-interval value could not be parsed. is it a positive integer?";
                return false;
            }

            params.mirror_interval = result;
            params.mirror = true;
            continue;
        }
        else if (arg[0] != '-') {
            params.servers.push_back(arg);
        }

        i++;
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

// TODO actually do something
void MirrorCallback(const string & path, uint64 start_offset, StreamSegment & segment, void * data)
{
    cout << path << ": " << start_offset << "-" << segment.EndOffset << endl;
}

int Mirror(vector<Client *> &clients, vector<StoreMirrorState> &states)
{
    if (!states.size()) {
        for (int i = 0; i < clients.size(); i++) {
            StoreMirrorState s;
            states.push_back(s);
        }
    }

    for (int i = 0; i < clients.size(); i++) {
        clients[i]->Mirror(states[i], MirrorCallback, NULL);
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
        else if (params.mirror) {
            vector<StoreMirrorState> states;

            // TODO handle interrupts and exit gracefully
            while (true) {
                Mirror(clients, states);

                // TODO write out state file

                if (params.mirror_interval > 0) {
                    ::zippylog::platform::sleep(params.mirror_interval * 1000);
                    continue;
                }

                break;
            }
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

