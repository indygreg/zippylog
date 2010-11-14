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

/// The zippylog_string_writer program converts string data to zippylog data.
///
/// Currently, it is limited to reading from stdin. However, with shell
/// redirection, it is quite simple to send a file's contents to stdin
/// on this process.
///
/// The following command arguments are recognized:
///
///    --lua-file - File containing Lua code for processing.
///    --stdout - Send string output to stdout
///    --store <path> - Path to store to write to.
///    --default-bucket <name> - Default bucket in store to send envelopes to
///    --default-stream-set <name> - Default stream set in store to send envelopes to

#include <zippylog/zippylog.hpp>
#include <zippylog/device/string_receiver.hpp>
#include <zippylog/device/string_writer.hpp>
#include <zippylog/util.hpp>

#include <string>
#include <vector>

using ::std::cout;
using ::std::endl;
using ::std::string;
using ::std::vector;
using ::zippylog::device::StringReceiver;
using ::zippylog::device::StringReceiverStartParams;
using ::zippylog::device::StringWriterStartParams;
using ::zippylog::device::StringWriter;
using ::zippylog::util::GetArgumentValueAndPop;

static bool ParseCommandArguments(
    vector<string> args,
    StringReceiverStartParams &sr_params,
    StringWriterStartParams &sw_params,
    string &error)
{
    if (args.size() < 2) {
        error = "you must pass at least one argument to this program";
        return false;
    }

    // get rid of first element, the program name
    args.erase(args.begin());

    for (size_t i = 0; i < args.size();) {
        if (args[i] == "--stdout") {
            sw_params.output_stream = &::std::cout;
            i++;
            continue;
        }
        else if (args[i] == "--store") {
            string value;
            if (!GetArgumentValueAndPop(args, i, value)) {
                error = "--store argument requires a value. None found";
                return false;
            }
            sw_params.store_path = value;
            continue;
        }
        else if (args[i] == "--default-bucket") {
            string value;
            if (!GetArgumentValueAndPop(args, i, value)) {
                error = "--default-bucket argument requires a value";
                return false;
            }
            sw_params.default_bucket = value;
            continue;
        }
        else if (args[i] == "--default-stream-set") {
            string value;
            if (!GetArgumentValueAndPop(args, i, value)) {
                error = "--default-stream-set argument requires a value";
                return false;
            }
            sw_params.default_stream_set = value;
            continue;
        }
        else if (args[i] == "--lua-file") {
            string value;
            if (!GetArgumentValueAndPop(args, i, value)) {
                error = "--lua-file argument requires a value";
                return false;
            }
            sr_params.lua_file = value;
            continue;
        }

        i++;
    }

    if (sw_params.store_path.length() > 0) {
        if (sw_params.default_bucket.length() < 1) {
            error = "--default-bucket required when --store is defined";
            return false;
        }

        if (sw_params.default_stream_set.length() < 1) {
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
        StringReceiverStartParams sr_params;
        StringWriterStartParams sw_params;

        if (!ParseCommandArguments(args, sr_params, sw_params, error)) {
            cout << error << endl;
            return 1;
        }

        sr_params.stream = &::std::cin;

        StringReceiver receiver(sr_params);
        sw_params.receiver = &receiver;

        StringWriter writer(sw_params);
        bool active = true;

        writer.Run(&active);

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
    catch (...) {
        cout << "received an exception" << endl;
        return 1;
    }

    return 0;
}
