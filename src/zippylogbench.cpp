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
#include <zippylog/envelope.hpp>
#include <zippylog/platform.hpp>
#include <zippylog/request_processor.pb.h>

extern "C" {
#include <lua.h>
#include <lauxlib.h>
}

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include <assert.h>

using ::std::cerr;
using ::std::cout;
using ::std::endl;
using ::std::ostringstream;
using ::std::string;
using ::std::vector;
using ::zippylog::Envelope;

#define TIMER_START(iterations) { \
    int TIMER_ITERATIONS = iterations; \
    ::zippylog::platform::Time __tstart; \
    ::zippylog::platform::TimeNow(__tstart); \
    for (int TIMER_LOOP_VALUE = TIMER_ITERATIONS; TIMER_LOOP_VALUE; TIMER_LOOP_VALUE--) { \


#define TIMER_END(description) } \
    ::zippylog::platform::Time __tend; \
    ::zippylog::platform::TimeNow(__tend); \
    uint64 elapsed = __tend.epoch_micro - __tstart.epoch_micro; \
    print_result(description, elapsed, TIMER_ITERATIONS); \
    }

class ZippylogbenchParams {
public:
    ZippylogbenchParams() :
        do_all(false),
        do_lua_function_calls(false)
    { }

    bool do_all;
    bool do_lua_function_calls;
};

static bool ParseCommandArguments(
    vector<string> args,
    ZippylogbenchParams &params,
    string &error)
{
    ostringstream usage;
    usage
        << "zippylogbench - benchmark various zippylog operations"                         << endl
        <<                                                                                    endl
        << "Usage: zippylogbench [options]"                                                << endl
        <<                                                                                    endl
        << "Where valid options are the following:"                                        << endl
        <<                                                                                    endl
        << "  --all                - Run all benchmarks. This is the default behavior"     << endl
        << "  --lua-function-calls - Lua function call timing"                             << endl
    ;
    // get rid of first element, the program name
    args.erase(args.begin());

    // no arguments means run with everything
    if (args.size() == 0) {
        params.do_all = true;
        return true;
    }
    else if (args.size() == 1) {
        if (args[0] == "--help" || args[0] == "-h" || args[0] == "-?") {
            error = usage.str();
            return false;
        }
    }

    for (int i = 0; i < args.size(); i++) {
        string arg = args[i];
        if (arg == "--all") {
            params.do_all = true;
        }
        else if (arg == "--lua-function-calls") {
            params.do_lua_function_calls = true;
        }
    }

    return true;
}


void print_result(string what, uint64 microseconds, uint64 ops)
{
    double seconds = (1.00 * microseconds) / 1000000.00;
    double rate = (1.00 * ops) / seconds;

    cout << what << " " << rate << "/s" << endl;
}

void run_lua_function_calls(ZippylogbenchParams &params)
{
    lua_State *L = luaL_newstate();
    const char *function = "function test()\n    return nil\nend";
    assert(!luaL_loadstring(L, function));

    TIMER_START(1000000);
    lua_getglobal(L, "test");
    lua_pcall(L, 0, 1, 0);
    lua_pop(L, 1);
    TIMER_END("lua.pcall_empty_function");
}

void run_zmq_benches(ZippylogbenchParams &params)
{
    TIMER_START(1000000);
    ::zmq::message_t *msg = new ::zmq::message_t();
    delete msg;
    TIMER_END("zmq.empty_message_allocation");

    TIMER_START(1000000);
    ::zmq::message_t *msg = new ::zmq::message_t(100);
    delete msg;
    TIMER_END("zmq.100_byte_message_allocation");

    TIMER_START(1000000);
    ::zmq::message_t *msg = new ::zmq::message_t(1000);
    delete msg;
    TIMER_END("zmq.1000_byte_message_allocation");

    TIMER_START(100000);
    ::zmq::message_t *msg = new ::zmq::message_t(10000);
    delete msg;
    TIMER_END("zmq.10000_byte_message_allocation");

    TIMER_START(10000);
    ::zmq::message_t *msg = new ::zmq::message_t(100000);
    delete msg;
    TIMER_END("zmq.100000_byte_message_allocation");
}

void run_envelope_benches(ZippylogbenchParams &params)
{
    TIMER_START(1000000);
    Envelope *e = new Envelope();
    delete e;
    TIMER_END("zippylog.envelope.empty_allocations");

    {
        Envelope e;
        string s;
        TIMER_START(1000000);
        e.Serialize(s);
        TIMER_END("zippylog.envelope.empty_serialize");
    }

    {
        Envelope e;
        ::zmq::message_t msg;
        TIMER_START(1000000);
        e.ToZmqMessage(msg);
        TIMER_END("zippylog.envelope.empty_zmq_serialize");
    }

    {
        Envelope e;
        ::zmq::message_t msg;
        TIMER_START(1000000);
        e.ToProtocolZmqMessage(msg);
        TIMER_END("zippylog.envelope.empty_zmq_protocol_serialize");
    }

    {
        Envelope empty;
        string serialized;
        empty.Serialize(serialized);
        void * buffer = (void *)serialized.data();
        int length = serialized.length();
        TIMER_START(1000000);
        Envelope e(buffer, length);
        TIMER_END("zippylog.envelope.empty_construct_from_buffer");
    }

    {
        Envelope empty;
        string serialized;
        empty.Serialize(serialized);
        TIMER_START(1000000);
        Envelope e(serialized);
        TIMER_END("zippylog.envelope.empty_construct_from_string");
    }

    {
        Envelope empty;
        ::zmq::message_t msg;
        empty.ToZmqMessage(msg);
        TIMER_START(1000000);
        Envelope e(msg);
        TIMER_END("zippylog.envelope.empty.construct_from_zmq");
    }

    {
        Envelope empty;
        TIMER_START(1000000);
        Envelope e(empty);
        TIMER_END("zippylog.envelope.empty_copy_constructor");
    }

    /*
    TIMER_START(1000000);
    Envelope e;
    ::zippylog::request_processor::Destroy msg;
    msg.add_to_envelope(e);
    TIMER_END("zippylog.envelope.create_and_add_message");
    */
}

void run_benchmarks(ZippylogbenchParams &params)
{
    if (params.do_lua_function_calls || params.do_all) {
        run_lua_function_calls(params);
    }

    run_zmq_benches(params);
    run_envelope_benches(params);
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
        ZippylogbenchParams params;
        if (!ParseCommandArguments(args, params, error)) {
            cerr << error << endl;
            return 1;
        }

        run_benchmarks(params);

        ::zippylog::shutdown_library();
    }
    catch (...) {
        cout << "received an exception" << endl;
        return 1;
    }

    return 0;
}
