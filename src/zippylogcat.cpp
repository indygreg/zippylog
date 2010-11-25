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
#include <zippylog/envelope.hpp>
#include <zippylog/platform.hpp>
#include <zippylog/stream.hpp>

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using ::std::cerr;
using ::std::cout;
using ::std::endl;
using ::std::ostringstream;
using ::std::string;
using ::std::vector;
using ::zippylog::Envelope;
using ::zippylog::FileInputStream;

class ZippylogcatSource {
public:
    enum Type {
        FILENAME = 1,
    };

    Type type;
    string filename;
};

class ZippylogcatParams {
public:
    vector<ZippylogcatSource> sources;
};

static bool ParseCommandArguments(
    vector<string> args,
    ZippylogcatParams &params,
    string &error)
{
    if (args.size() < 2) {
        ostringstream usage;
        usage
            << "Usage: " << args[0] << " [options] source0 [source1...]"          << endl
            <<                                                                       endl
            << "Each source can be one of the following:"                         << endl
            <<                                                                       endl
            << "    * A filename. It is assumed the file is a zippylog stream"    << endl
            << "    * A directory. It is assumed the directory is a stream set"   << endl
            << "      which contains multiple zippylog stream files"              << endl
            << "    * A 0MQ socket endpoint to a zippylog server plus store path" << endl
            << "      identifier. The store path is defined after a pound"        << endl
            << "      character (#). e.g. 'tcp://127.0.0.1:52483#/bucketA'"       << endl
            <<                                                                       endl
            << "Sources will be read in full in the order they are defined."      << endl
            <<                                                                       endl
            << "The following arguments control behavior:"                        << endl
            <<                                                                       endl
        ;
        error = usage.str();

        return false;
    }

    // get rid of first element, the program name
    args.erase(args.begin());

    for (int i = 0; i < args.size(); i++) {
        if (args[i][0] == '-') {
            error = "unrecognized argument: " + args[i];
            return false;
        }
    }

    for (int i = 0; i < args.size(); i++) {
        string arg = args[i];

        ZippylogcatSource source;

        // if it looks like a URI
        if (arg.find("://") != string::npos) {

        }
        else {
            ::zippylog::platform::FileStat st;
            if (!::zippylog::platform::stat(arg, st)) {
                error = "file path does not seem to exist or isn't readable: " + arg;
                return false;
            }

            source.type = ZippylogcatSource::FILENAME;
            source.filename = arg;

            params.sources.push_back(source);
        }
    }

    return true;
}

bool ProcessFilename(string filename)
{
    try {
        FileInputStream is(filename);

        uint32 read = 0;
        Envelope e;
        while (is.ReadEnvelope(e, read)) {
            cout << e.ToString();
        }
    }
    catch (::std::exception e) {
        cerr << "Exception when processing file: " << e.what() << endl;
        return false;
    }
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
        ZippylogcatParams params;

        if (!ParseCommandArguments(args, params, error)) {
            cout << error << endl;
            return 1;
        }

        for (int i = 0; i < params.sources.size(); i++) {
            ZippylogcatSource source = params.sources[i];

            if (source.type == ZippylogcatSource::FILENAME) {
                if (!ProcessFilename(source.filename)) break;
            }
        }

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
