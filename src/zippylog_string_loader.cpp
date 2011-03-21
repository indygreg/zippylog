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

#include <zippylog/zippylog.hpp>
#include <zippylog/device/string_loader.hpp>
#include <zippylog/util.hpp>

#include <sstream>
#include <string>
#include <vector>

using ::std::cout;
using ::std::endl;
using ::std::string;
using ::std::ostringstream;
using ::std::vector;
using ::zippylog::device::StringLoader;
using ::zippylog::device::StringLoaderStartParams;
using ::zippylog::util::GetArgumentValueAndPop;

string usage =
"zippylog_string_loader - loads string data into zippylog\n"
"\n"
"Usage: zippylog_string_loader [arguments]\n"
"\n"
"  Output Control (must specify one)\n"
"    --stdout             Send text version of produced envelopes to stdout\n"
"    --server <endpoint>  0MQ endpoint of a zippylog server to write to\n"
"    --store <path>       Path to store to write to\n"
"\n"
"  Output Options (required if writing to a server or store)\n"
"    --default-bucket     Default bucket in store to write to\n"
"    --default-stream-set Default stream set in store to write to\n"
"\n"
"  Processing Options\n"
"    --lua-file <path>    Lua file to load\n"
"\n"
"If a Lua file is loaded, the following functions are called:\n"
"\n"
"    zippylog_load_string - Called on every string being loaded\n"
"\n"
"The default reading behavior is to read 1 line at a time from stdin.\n"
"\n"
"The default loading behavior is to convert every string into an\n"
"envelope whose string data field is set to the loaded string. If Lua is used,\n"
"loaded strings can be changed, split, combined, or converted to messages or\n"
"envelopes directly. Lua can also be used to overwrite the default bucket\n"
"and/or stream set for envelopes.\n"
"\n"
"The program exits when stdin is closed or an unrecoverable error\n"
"occurs. In the case of abnormal exit, the return code will be 1 and an\n"
"error will be printed to stderr.\n"
"\n"
"Usage Notes\n"
"\n"
"--stdout is likely only useful for testing Lua scripts.\n"
"\n"
"Examples\n"
"\n"
"  zippylog_string_loader --store simpledirectory:///var/zippylog/store00 \\\n"
"    --default-bucket systemlogs --default-stream-set syslog \\\n"
"    < /var/log/syslog\n"
"\n"
"  zippylog_string_loader --server tcp://10.1.0.1:52483 \\\n"
"    --default-bucket test --default-stream-set foo --lua-file ~/parse.lua \\\n"
"    < ~/some/file\n"
"\n"
"  find / | zippylog_string_loader --stdout --lua-file ~/test.lua\n"
;

static bool ParseCommandArguments(
    vector<string> args,
    StringLoaderStartParams &params,
    string &error)
{
    if (args.size() < 2) {
        error = usage;

        return false;
    }

    // get rid of first element, the program name
    args.erase(args.begin());

    for (size_t i = 0; i < args.size();) {
        if (args[i] == "--stdout") {
            params.output_stream = &::std::cout;
            i++;
            continue;
        }
        else if (args[i] == "--store") {
            string value;
            if (!GetArgumentValueAndPop(args, i, value)) {
                error = "--store argument requires a value. None found";
                return false;
            }
            params.store_path = value;
            continue;
        }
        else if (args[i] == "--default-bucket") {
            string value;
            if (!GetArgumentValueAndPop(args, i, value)) {
                error = "--default-bucket argument requires a value";
                return false;
            }
            params.default_bucket = value;
            continue;
        }
        else if (args[i] == "--default-stream-set") {
            string value;
            if (!GetArgumentValueAndPop(args, i, value)) {
                error = "--default-stream-set argument requires a value";
                return false;
            }
            params.default_set = value;
            continue;
        }
        else if (args[i] == "--lua-file") {
            string value;
            if (!GetArgumentValueAndPop(args, i, value)) {
                error = "--lua-file argument requires a value";
                return false;
            }
            params.lua_file = value;
            continue;
        }

        i++;
    }

    if (params.store_path.length() > 0) {
        if (params.default_bucket.length() < 1) {
            error = "--default-bucket required when --store is defined";
            return false;
        }

        if (params.default_set.length() < 1) {
            error = "--default-stream-set required when --store is defined";
            return false;
        }
    }

    return true;
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
        StringLoaderStartParams params;

        if (!ParseCommandArguments(args, params, error)) {
            cout << error << endl;
            return 1;
        }

        params.input_stream = &::std::cin;

        bool active = true;
        params.active = &active;

        StringLoader loader(params);

        loader.Run();

        ::zippylog::shutdown_library();
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
    catch (::std::exception e) {
        cout << "Exception: " << e.what() << endl;
        return 1;
    }
    catch (...) {
        cout << "received an exception" << endl;
        return 1;
    }

    return 0;
}
